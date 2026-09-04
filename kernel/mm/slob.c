#include "mm.h"
#include "kernel/lib/memory.h"
#include "kernel/lib/kprintf.h"


typedef struct slob_header
{
    uint32_t size;
    struct slob_header* next;
    bool free;
} slob_header_t;

typedef struct 
{
    slob_header_t* free_list;
    void* start;
    void* end; 
    size_t total_size;
    size_t allocated_size;
    size_t alloc_count;
    bool slob_initialized;
} slob_t;

static slob_t slob_allocator;

#define SLOB_MINIMUM_BLOCK_SIZE (sizeof(slob_header_t) + 8)
#define SLOB_ALIGNMENT 8

static inline size_t align_size(size_t size) 
{
    return (size + SLOB_ALIGNMENT - 1) & ~(SLOB_ALIGNMENT - 1);
}

void slobInit(void* start, size_t size)
{
    /* Align the starting address */
    uintptr_t align_start = ((uintptr_t)start + SLOB_ALIGNMENT - 1) & ~(SLOB_ALIGNMENT - 1);

    size_t aligned_size = size & ~(SLOB_ALIGNMENT - 1);
    
    slob_allocator.start = (void*)align_start;
    slob_allocator.end = (void*)(align_start + aligned_size);
    slob_allocator.total_size = aligned_size;
    slob_allocator.allocated_size = 0;
    slob_allocator.alloc_count = 0;
    
    /* Make an initial free block we can use  */
    slob_header_t* initial_block = (slob_header_t*)align_start;
    initial_block->size = aligned_size;
    initial_block->free = true;
    initial_block->next = NULL;
    
    slob_allocator.free_list = initial_block;
    slob_allocator.slob_initialized = true;

}

void* slobAlloc(size_t size)
{
    if (size == 0) 
    {
        return NULL;
    }
    
    size = align_size(size);
    size_t total_needed = size + sizeof(slob_header_t);
    
    
    if (total_needed < SLOB_MINIMUM_BLOCK_SIZE) 
        total_needed = SLOB_MINIMUM_BLOCK_SIZE;
    
    /* Find the first fit */
    slob_header_t* current = slob_allocator.free_list;
    slob_header_t* previous = NULL;
    
    while (current != NULL) 
    {
        if (current->free && current->size >= total_needed)    
            break;
        
        previous = current;
        current = current->next;
    }
    
    if (current == NULL) 
        return NULL;  /* Could not find block. Should implement something for the kernel to do if a block isn't found */
    
    
    /* see if block could be split */
    size_t remaining_size = current->size - total_needed;
    
    if (remaining_size >= SLOB_MINIMUM_BLOCK_SIZE) 
    {
        
        slob_header_t* new_block = (slob_header_t*)((uintptr_t)current + total_needed);
        new_block->size = remaining_size;
        new_block->free = true;
        new_block->next = current->next;
        
        if (previous != NULL) 
        {
            previous->next = new_block;
        } 
        else 
        {
            slob_allocator.free_list = new_block;
        }
        
        current->size = total_needed;
    } 
    else 
    {
        /* Use entire block */
        if (previous != NULL) 
        {
            previous->next = current->next;
        } 
        else 
        {
            slob_allocator.free_list = current->next;
        }
    }
    
   /* mark block as allocated */
    current->free = false;
    current->next = NULL;
    
    slob_allocator.allocated_size += current->size;
    slob_allocator.alloc_count++;
    
    return (void*)((uintptr_t)current + sizeof(slob_header_t));
}

void slobFree(void* p)
{
    if ( p == NULL) 
    {
        kprintf("slob.c: in function slobFree(void* p): Function argument p is NULL.");
    }
    
    slob_header_t* block = (slob_header_t*)((uintptr_t)p - sizeof(slob_header_t));
    

    if ((uintptr_t)block < (uintptr_t)slob_allocator.start ||
        (uintptr_t)block >= (uintptr_t)slob_allocator.end) 
    {
        return;  
    }
    
    if (block->free) 
    {
        return; /* already free */
    }
    
    slob_allocator.allocated_size -= block->size;
    slob_allocator.alloc_count--;
    

    block->free = true;
    
    
    slob_header_t* current = slob_allocator.free_list;
    slob_header_t* previous = NULL;
    
    while (current != NULL) 
    {
        /* Check if current block is adjacent to freed block */
        if ((uintptr_t)current + current->size == (uintptr_t)block) 
        {
            /* merge current with block with previous block */
            current->size += block->size;
            block = current;  
            continue;
        }
        
        if ((uintptr_t)block + block->size == (uintptr_t)current) 
        {
           
            block->size += current->size;
        
            if (previous != NULL) 
            {
                previous->next = current->next;
            } 
            else 
            {
                slob_allocator.free_list = current->next;
            }
            continue;
        }
        
        previous = current;
        current = current->next;
    }
    
    /* Add block to free list (if not already there from merge)*/
    if (block->free && block != slob_allocator.free_list) 
    {
        block->next = slob_allocator.free_list;
        slob_allocator.free_list = block;
    }
}

void* slobRealloc(void *p, size_t new_size)
{
    if (p == NULL) 
    {
        return slobAlloc(new_size);
    }
    
    if (new_size == 0) 
    {
        slobFree(p);
        return NULL;
    }
    
    
    slob_header_t* block = (slob_header_t*)((uintptr_t)p - sizeof(slob_header_t));
    size_t old_size = block->size - sizeof(slob_header_t);

    if (new_size <= old_size) 
        return p;
    
    void* new_ptr = slobAlloc(new_size);
    if (new_ptr == NULL) 
        return NULL;
    
    
    memcpy(new_ptr, p, old_size);
    
    slobFree(p);
    
    return new_ptr;
}

void* slobCalloc(size_t num, size_t size )
{
    size_t total = num * size;
    void* p = slobAlloc(total);
    
    if (p != NULL) 
    {
        memset(p, 0, total);
    }
    
    return p;
}

void printHeap()
{
    size_t total_mem = slob_allocator.total_size;
    size_t total_allocated = slob_allocator.allocated_size;
    size_t free = slob_allocator.total_size - slob_allocator.allocated_size;

    /* print fragmentation stats */
    size_t free_blocks;
    slob_header_t* current = slob_allocator.free_list;

    while(current != NULL)
    {
        free_blocks++;
        current = current->next;
    }

    kprintf("Total heap memory: %lu bytes, Total allocated: %lu bytes, Free: %lu bytes\n", total_mem, total_allocated, free);
}