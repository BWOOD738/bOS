#include "mm.h"
#include "memory.h"
#include "kernel/kprintf.h"
#include "limine.h"

#define PAGES_PER_BYTE 0x8
#define PAGE_ALIGN PAGE_SIZE

static uint64_t pmm_mem_size;
static uint64_t pmm_used_pages;
static uint64_t pmm_max_pages;

static uint32_t* pmm_mem_map;


__attribute__((used, section(".limine_requests")))
struct limine_memmap_request mmap_req = 
{
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
struct limine_hhdm_request hhdm_req = 
{
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};

size_t pmmGetMemorySize()
{
    return pmm_mem_size;
}

uint64_t pmmGetPageCount()
{
    return pmm_max_pages;
}

uint64_t pmmGetUsedPageCount()
{
    return pmm_used_pages;
}

uint64_t pmmGetFreePageCount()
{
    return pmm_max_pages - pmm_used_pages;
}

void pmmInit()
{
    uint64_t hhdm = 0; /* This should probably be global */
    uint64_t total_memory = 0;

    struct limine_memmap_entry* mme = NULL;

    if(hhdm_req.response == NULL || mmap_req.response == NULL)
    {
        kprintf("pmm.c: Cannot get memory requests.");
        return;
    }
    
    hhdm = hhdm_req.response->offset;
    
    for(size_t i = 0; i < mmap_req.response->entry_count; i++ )
    {
        struct limine_memmap_entry* entry = mmap_req.response->entries[i];

        if(entry->type == LIMINE_MEMMAP_USABLE)
            total_memory += entry->length;
    }


    pmm_mem_size = total_memory / 1024;
    pmm_max_pages = total_memory / PAGE_SIZE;
    pmm_used_pages = pmm_max_pages;
   
    /* Calculate size of bitmap */
    size_t bitmap_size = (pmm_max_pages + 7) / 8; /* Stores size of bitmap in bytes  */
    bitmap_size = ALIGN_UP(bitmap_size, sizeof(uint32_t));

    for(size_t i = 0; i < mmap_req.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = mmap_req.response->entries[i];
        
        if(entry->type != LIMINE_MEMMAP_USABLE || entry->length < bitmap_size)
            continue;
            
        /* Check if a region can hold the bitmap */
        if(entry->length >= bitmap_size + PAGE_SIZE)
        {
            mme = entry;
            break;  
        }
    }

    if(!mme)
    {
        kprintf("pmm.c: Not enough memory");
        return;
    }

    uint64_t bitmap_phys = mme->base;
    pmm_mem_map = (uint32_t*)(bitmap_phys + hhdm); /* could set hhdm to hhdm_req.response->offset initially instead of 0*/

    bitmap_phys = ALIGN_UP(bitmap_phys, PAGE_SIZE);

    /* Should look into skipping the first 1-2 MB since Limine puts some important stuff there */
    /* All memory marked as used initially */
    memset(pmm_mem_map, 0xff, bitmap_size);

    for(size_t i = 0; i < mmap_req.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = mmap_req.response->entries[i];
        
        if(entry->type == LIMINE_MEMMAP_USABLE)
        {
            if(entry->base == bitmap_phys)
            {
                /* Mark memory after bitmap as free */
                uint64_t free_region = bitmap_phys + bitmap_size;
                size_t free_length = entry->length - bitmap_size;
                
                if(free_length > 0)
                {
                    pmmInitRegion(free_region, free_length);
                }
            }
            else
            {
                pmmInitRegion(entry->base, entry->length);
            }
        }
    }
    pmmMapSet(0);

    kprintf("Bitmap initialized at address 0x%lx, with size %lu MB memory", bitmap_phys, total_memory / (1024*1024));
}

inline void pmmMapSet(uint32_t bit)
{
    pmm_mem_map[bit / 32] |= (1U << (bit % 32));
}

inline void pmmMapUnset(uint32_t bit)
{
    pmm_mem_map[bit / 32] &= ~ (1U << (bit % 32));
}

inline bool pmmMapTest(uint32_t bit)
{
    return pmm_mem_map[bit / 32] &  (1U << (bit % 32));
} 

uint64_t pmmFirstMapFree()
{
    for(uint32_t i = 0; i < pmmGetPageCount() / 32; i++)
    {
        if(pmm_mem_map[i] != 0xffffffff)
        {
            for (uint32_t j = 0; j < 32; j++)
            {
                uint32_t bit = 1U << j;
                if(!(pmm_mem_map[i] & bit))
                {
                    return i * 32 + j;
                }
            }
        }
    }
    return -1;
}
void* pmmAllocPage()
{
    if(pmmGetFreePageCount() <= 0)
        return 0; /* No memory */
    
    uint32_t frame = pmmFirstMapFree();
    if(frame == -1)
        return 0;

    pmmMapSet(frame);
    physaddr_t addr = frame * PAGE_SIZE;
    pmm_used_pages++;

    return (void*)addr;
}

void pmmDeallocPage(uintptr_t physical_addr)
{
    physaddr_t addr = (physaddr_t)physical_addr;

    if (addr & (PAGE_SIZE - 1)) return; /* Not aligned */

    uint32_t frame = addr / PAGE_SIZE;
   
    if (frame >= pmm_max_pages || !pmmMapTest(frame))
        return;

    pmmMapUnset(frame);
    pmm_used_pages--;
}

/* Initialize a zone of memory for use */
void pmmInitRegion(uintptr_t base, size_t size)
{
    uint32_t align = (uint32_t)base / PAGE_SIZE;
    uint32_t pages = size / PAGE_SIZE;

    for(; pages > 0; pages--)
    {
        pmmMapUnset(align++);
        pmm_used_pages--;
    }

    if (base & (PAGE_SIZE - 1) || size & (PAGE_SIZE - 1)) 
        kprintf("Misaligned Pages Rejected. "); /* TODO: Add a fucntion to realign pages. */

    pmmMapSet(0); /* First page is always set so that allocs cannot be 0 */

}
/* Memory region will not be in use */
void pmmDeinitRegion(uintptr_t base, size_t size)
{
    uint32_t align = (uint32_t)base / PAGE_SIZE;
    uint32_t pages = (uint32_t)size / PAGE_SIZE;

    for(; pages > 0; pages--)
    {
        pmmMapSet(align++);
        pmm_used_pages++;
    }

    if (base & (PAGE_SIZE - 1) || size & (PAGE_SIZE - 1)) return;
}



static bool pmmAlignPage(uint32_t align)
{
    
}