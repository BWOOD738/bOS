#include "pmm.h"
#include "memory.h"
#include "kernel/kprintf.h"

static uint32_t pmm_mem_size;
static uint32_t pmm_used_pages;
static uint32_t pmm_max_pages;

static uint32_t* pmm_mem_map;

void pmmInit(size_t mem_size, phys_addr bitmap)
{
    pmm_mem_size = mem_size;
    pmm_mem_map = (uint32_t*)bitmap;
    pmm_max_pages = (pmmGetMemorySize() * 1024) / PAGE_SIZE;
    pmm_used_pages = pmmGetPageCount();

    /* By default, all memory is in use */

    memset(pmm_mem_map, 0xFF, pmmGetPageCount() / PAGES_PER_BYTE);
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

uint32_t pmmFirstMapFree()
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
    phys_addr addr = frame * PAGE_SIZE;
    pmm_used_pages++;

    return (void*)addr;
}

void pmmDeallocPage(uintptr_t physical_addr)
{
    phys_addr addr = (phys_addr)physical_addr;

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

size_t pmmGetMemorySize()
{
    return pmm_mem_size;
}

uint32_t pmmGetPageCount()
{
    return pmm_max_pages;
}

uint32_t pmmGetUsedPageCount()
{
    return pmm_used_pages;
}

uint32_t pmmGetFreePageCount()
{
    return pmm_max_pages - pmm_used_pages;
}

static bool pmmAlignPage(uint32_t align)
{
    
}