#pragma once 

#include "types.h"
#include <stddef.h>

typedef struct 
{
    byte status;
    uint32_t size;
} alloc_t;

void malloc();
void free();

void *memcpy(void *restrict dest, const void *restrict, size_t n);
void* memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
