#pragma once 

#include <stdatomic.h>

typedef atomic_flag spinlock_t;

void acquire(spinlock_t* lock);
void release(spinlock_t* lock);

