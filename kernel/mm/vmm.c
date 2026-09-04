#include "mm.h"
#include "kernel/bootloader.h"
#include "types.h"
#include "kernel/lib/kprintf.h"
#include "kernel/lib/memory.h"
#include "arch/regs.h"

/*
https://cs61.seas.harvard.edu/site/2018/Kernel3Old/
*/

#define PML4_INDEX(addr) ((addr >> 39) & 0x1FF)
#define PDPT_INDEX(addr) ((addr >> 30) & 0x1FF)
#define PD_INDEX(addr)   ((addr >> 21) & 0x1FF)
#define PT_INDEX(addr)   ((addr >> 12) & 0x1FF)
#define PAGEOFFSET(addr)   (addr & 0xFFF)
#define PD_INDEX(addr) (((addr) >> 21) & 0x1FF)
#define PT_INDEX(addr) (((addr) >> 12) & 0x1FF)


#define PAGE_PRESENT (1ULL << 0)
#define PAGE_WRITABLE (1ULL << 1)
#define PAGE_USER_ACCESSIBLE (1ULL << 2)
#define PAGE_WRITE_THROUGH (1ULL << 3)
#define PAGE_DISABLE_CACHE (1ULL << 4)
#define PAGE_ACCESSED (1ULL << 5)
#define PAGE_DIRTY (1ULL << 6)
#define PAGE_HUGE (1ULL << 7)
#define PAGE_GLOBAL (1ULL << 8)
#define PAGE_COW (1ULL << 9)
#define PAGE_EXECUTABLE (1ULL << 63)

#define PAGE_RECURSIVE_BASE 0xFFFFFF0000000000ULL /* This address doesnt seem to work. Find another entry in PML4 that works */

/* initial top level page table */
static pml4_t* pml4;

static pdpt_t* pdpt;
static pagedir_t* pd;
static pagetbl_t* pml1;

static uint32_t max_phys_address;
static uint32_t max_linear_address;

static pte_t vmmAllocPage(uint64_t flags);
static uint64_t vmmVirtualToPhysical(virtaddr_t virtual);

/* This will be used to get the virtual addresses of page table entry. Used for recursive mapping */
static uint64_t vmmPageTableVirtAddress(virtaddr_t pml4_indx, virtaddr_t pdpt_indx, virtaddr_t pd_iddx, virtaddr_t pt_indx );

/* Will use Limine's identity mapped pages for now as it maps the total amount of physical memory we have.
    Later will need to switch to and manage the kernel's own page tables but Limine's initial PTs should work well for now.
*/

void vmmInit()
{
    
    uint64_t bits = getAddressWidth();
    uint64_t offset = g_hhdm_req.response->offset;

    max_phys_address = bits & 0xFF;
    max_linear_address = bits >> 8 & 0xFF;
    
    kprintf("vmm.c: Max physical address bits: %d\n", max_phys_address);
    kprintf("vmm.c: Max linear address bits: %d\n", max_linear_address);

    uint64_t cr3 = readCR3();
    pml4 = (pml4_t*)vmmPhysicalToVirtual(cr3);

    kprintf("vmm.c: PML4 at 0x%lx\n", (virtaddr_t)pml4);

    /* test the mapping function */
    physaddr_t phys = (physaddr_t)pmmAllocPage();
    virtaddr_t virt = 0xFFFF900000000000ULL; /* This address should be unused */

    if (phys == 0) 
    {
        kprintf("vmm.c: Could not allocate page\n");
        return;
    }
    
    kprintf("vmm.c: Mapping physical address 0x%lx to virtual address 0x%lx\n", phys, virt);
    
    vmmMapPage(phys, virt, PAGE_PRESENT | PAGE_WRITABLE);

    volatile uint64_t* test = (volatile uint64_t*)virt;
    *test = 0x1234567890ABCDEFULL;
    
    volatile uint64_t* hhdm = (volatile uint64_t*)(phys + g_hhdm_req.response->offset);
    
    if (*test == 0x1234567890ABCDEFULL && *hhdm == 0x1234567890ABCDEFULL) 
    {
        kprintf("vmm.c: Value of test pointer and hhdm pointer is the same\n");
    } 
    else 
    {
        kprintf("vmm.c: Writing to virtual address failed \n");
        kprintf("Through mapping: 0x%lx\n", *test);
        kprintf("Through HHDM:    0x%lx\n", *hhdm);
    }
    
}

/*https://www.youtube.com/watch?v=hIsL0rGZzfs*/
/* TODO: Add some checks to see if entries are valid */
/* This function is slow. Should use recursive mapping instead of walking through manually */

void vmmMapPage(physaddr_t physical, virtaddr_t virtual, uint64_t flags)
{
    uint64_t address_mask = (((uint64_t)1 << max_phys_address) - 1) << 12;
    flags |= PAGE_PRESENT; /* Page will always be set to present */

    if(!(pml4->entries[PML4_INDEX(virtual)] & PAGE_PRESENT))
    {
        pdpte_t pdpte_phys = (pdpte_t)pmmAllocPage();
        pdpte_t* pdpte = (pdpte_t*)vmmPhysicalToVirtual(pdpte_phys);

        memset(pdpte, 0, PAGE_SIZE);
        pml4->entries[PML4_INDEX(virtual)] = pdpte_phys | PAGE_PRESENT | PAGE_WRITABLE;
    }

    
    pdpt_t *pdpt = (pdpt_t*)vmmPhysicalToVirtual(pml4->entries[PML4_INDEX(virtual)] & address_mask);

    if(!(pdpt->entries[PDPT_INDEX(virtual)] & PAGE_PRESENT))
    {
        pde_t pde_phys = (pde_t)pmmAllocPage();
        pde_t* pde = (pde_t*)vmmPhysicalToVirtual(pde_phys);

        memset(pde, 0, PAGE_SIZE);
        pdpt->entries[PDPT_INDEX(virtual)] = pde_phys | PAGE_PRESENT | PAGE_WRITABLE;
    }

    pagedir_t *pd = (pagedir_t*)vmmPhysicalToVirtual(pdpt->entries[PDPT_INDEX(virtual)] & address_mask);

    if(!(pd->entries[PD_INDEX(virtual)] & PAGE_PRESENT))
    {
        pte_t pte_phys = (pte_t)pmmAllocPage();

        pte_t* pte = (pte_t*)vmmPhysicalToVirtual(pte_phys);
        memset(pte, 0, PAGE_SIZE);
        pd->entries[PD_INDEX(virtual)] = (uint64_t)pte_phys | PAGE_PRESENT | PAGE_WRITABLE;
    }

    pagetbl_t *pml1 = (pagetbl_t*)vmmPhysicalToVirtual(pd->entries[PD_INDEX(virtual)] & address_mask);
    
    pml1->entries[PT_INDEX(virtual)] = physical | flags;

    /* Flush the tlb for this address */
    asm volatile("invlpg (%0)" :: "r"(virtual) : "memory");
}


uint64_t vmmPhysicalToVirtual(physaddr_t physical)
{
    virtaddr_t virtual = physical + g_hhdm_req.response->offset;
    return virtual;
}

uint64_t vmmVirtualToPhysical(virtaddr_t virtual)
{
    physaddr_t physical = virtual - g_hhdm_req.response->offset;
    return physical;
}

uint64_t vmmPageTableVirtAddress(virtaddr_t pml4_indx, virtaddr_t pdpt_indx, virtaddr_t pd_indx, virtaddr_t pt_indx)
{
    return PAGE_RECURSIVE_BASE | (pml4_indx << 39) | (pdpt_indx << 30) | (pd_indx << 21) | (pt_indx << 12);
}