#include "mm.h"
#include "kernel/bootloader.h"
#include "arch/regs.h"
#include "types.h"
#include "kernel/kprintf.h"
#include "memory.h"

/*
https://cs61.seas.harvard.edu/site/2018/Kernel3Old/
*/

#define L1PAGEINDEX(addr) ((addr >> 39) & 0x1FF)
#define L2PAGEINDEX(addr)  ((addr >> 30) & 0x1FF)
#define L3PAGEINDEX(addr)  ((addr >> 21) & 0x1FF)
#define L4PAGEINDEX(addr)  ((addr >> 12) & 0x1FF)
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

/* initial top level page table */
static pml4_t* pml4;

static pdpt_t* pdpt;
static pagedir_t* pd;
static pagetbl_t* pml1;

static uint32_t max_phys_address;
static uint32_t max_linear_address;

static void pageFault();
static pte_t vmmAllocPage(uint64_t flags);
static uint64_t vmmVirtualToPhysical(virtaddr_t virtual);

/* This function is pretty slow */

void vmmInit()
{
    uint64_t bits = getAddressWidth();

    /* The maximum amount of bits should be read from the cpu directly but since we only use x64 with 4kb pages, this should work as a constant  */
    max_phys_address = bits & 0xFF;
    max_linear_address = bits >> 8 & 0xFF;

    kprintf("vmm.c: Max physical address bits: %d\n", max_phys_address);
    kprintf("vmm.c: Max linear address bits: %d\n", max_linear_address);

    if(g_mmap_request.response == NULL)
    {
        kprintf("vmm.c: Cannot get global memmap response!");
    }

    pml4 = (pml4_t*)pmmAllocPage();
    memset(pml4, 0, PAGE_SIZE);

    if(g_mmap_request.response)
    {
        /* Should write a function to get the number of physical frames and use it for both the pmm and vmm. This way I only need to do this calculation once */
        uint64_t total_pages = 0; /* For getting total number of available pages */
        for(size_t i = 0; i < g_mmap_request.response->entry_count; i++)
        {
            struct limine_memmap_entry* entry = g_mmap_request.response->entries[i];
            
            if(entry->type == LIMINE_MEMMAP_USABLE)
            {
                total_pages += entry->length / PAGE_SIZE;
            }
        }
        /* Where physical pages are mapped to the HHDM virtual pages. This lets us access any physical page through HHDM requests
            0x1000 -> 0xffff800000001000
        */
        for(size_t i = 0; i < total_pages; i++)
        {
            physaddr_t phys = i * PAGE_SIZE;
            virtaddr_t virt  = phys + g_hhdm_req.response->offset;
            vmmMapPage(phys, virt, PAGE_WRITABLE | PAGE_PRESENT);
        }

        /* Map the kernel using it's virtual addresses */

        uint64_t kernel_virtual_start = ALIGN_DOWN((uint64_t)&_kernel_start, PAGE_SIZE);
        uint64_t kernel_virtual_end = ALIGN_UP((uint64_t)&_kernel_end, PAGE_SIZE);
        uint64_t kernel_virtual_size = kernel_virtual_end - kernel_virtual_start;

        kprintf("vmm.c: Kernel virtual start: 0x%llx, Kernel virtual end: 0x%llx, Kernel virtual space size: %xll", kernel_virtual_start, kernel_virtual_end, kernel_virtual_size);

        uint64_t kernel_physical_start = (uint64_t)&_kernel_start - g_hhdm_req.response->offset;
        
        /* Maps kernel physical addresses to virtual so that the kernel/cpu can actually use them */
        for(int i = 0; i < kernel_virtual_size; i += PAGE_SIZE)
        {
            vmmMapPage(kernel_physical_start + i, kernel_virtual_start + i, PAGE_PRESENT | PAGE_WRITABLE);
            
        }
    }

    uint64_t pml4_phys = vmmVirtualToPhysical((virtaddr_t)pml4);
    asm volatile("mov %0, %%cr3" :: "r"(pml4_phys) : "memory"); /* Switch to new page tables */
    kprintf("vmm.c: Kernel vmm initialized.");
}

/*https://www.youtube.com/watch?v=hIsL0rGZzfs*/

void vmmMapPage(physaddr_t physical, virtaddr_t virtual, uint64_t flags)
{
    uint64_t address_mask = (((uint64_t)1 << max_phys_address) - 1) << 12;

    if(!(pml4->entries[L4PAGEINDEX(virtual)] & PAGE_PRESENT))
    {
        pdpte_t *pdpte = (pdpte_t*)pmmAllocPage();
        pml4->entries[L4PAGEINDEX(virtual)] = (uint64_t)pdpte | PAGE_PRESENT | PAGE_WRITABLE;
    }

    
    pdpt_t *pdpt = (pdpt_t*)vmmPhysicalToVirtual(pml4->entries[L4PAGEINDEX(virtual)] & address_mask);


    if(!(pdpt->entries[L3PAGEINDEX(virtual)] & PAGE_PRESENT))
    {
        pde_t *pde = (pde_t*)pmmAllocPage();
        pdpt->entries[L3PAGEINDEX(virtual)] = (uint64_t)pde | PAGE_PRESENT | PAGE_WRITABLE;
    }

    pagedir_t *pd = (pagedir_t*)vmmPhysicalToVirtual(pdpt->entries[L3PAGEINDEX(virtual)] & address_mask);

    if(!(pd->entries[L2PAGEINDEX(virtual)] & PAGE_PRESENT))
    {
        pte_t *pte = (pte_t*)pmmAllocPage();
        pd->entries[L2PAGEINDEX(virtual)] = (uint64_t)pte | PAGE_PRESENT | PAGE_WRITABLE;
    }

    pagetbl_t *pml1 = (pagetbl_t*)vmmPhysicalToVirtual(pd->entries[L2PAGEINDEX(virtual)] & address_mask);
    
    pml1->entries[L1PAGEINDEX(virtual)] = physical | flags | PAGE_PRESENT;

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