#include "spinlock.h"

void acquire(spinlock_t* lock)
{
    while(atomic_flag_test_and_set_explicit(lock, memory_order_acquire))
    {
        asm volatile("pause" ::: "memory");
    }
}

void release(spinlock_t* lock)
{
    atomic_flag_clear_explicit(lock, memory_order_release);
}
