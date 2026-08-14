#pragma once

#include <types.h>
#include <stddef.h> /* size_t */
#include <stdint.h>

/* This file will be used as an interface for the rest of the main files in the mm directory. Important functions for memory
management will be declared and defined elsewhere. 
*/

#define PAGE_SIZE 0x1000

#define ALIGN_UP(addr, align) (((addr) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(addr, align) ((addr) & ~(align - 1))

typedef uint64_t physaddr_t;
typedef uint64_t virtaddr_t;

/* pmm.c */
void pmmInit();
inline void pmmMapSet(uint32_t bit);
inline void pmmMapUnset(uint32_t bit);
inline bool pmmMapTest(uint32_t bit); /* See if bit */
uint64_t pmmFirstMapFree(); /* Return first bit free */
void* pmmAllocPage();
void pmmDeallocPage(uintptr_t physical_addr);
void pmmInitRegion(uintptr_t base, size_t size);
void pmmDeinitRegion(uintptr_t base, size_t size);