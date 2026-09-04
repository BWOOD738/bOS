#include "kernel/mm/mm.h"
#include "mm.h"
#include "kernel/spinlock.h"

/* maybe kmalloc.h should be its own file? */

static spinlock_t lock;

void* kmalloc(size_t size)
{
    acquire(&lock)
    void* ptr = slobAlloc(size);
    release(&lock);
    return ptr;
}


void kfree(void *p)
{
    if(!ptr)
        return;

    acquire(&lock);
    slobFree(p);
    release(&lock);
}