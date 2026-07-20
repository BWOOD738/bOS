#pragma once

#include <types.h>
#include <stddef.h> /* size_t */
#include <stdint.h>

/* This file will be used as an interface for the rest of the main files in the mm directory. Important functions for memory
management will be declared and defined elsewhere. 
*/

typedef uint32_t phys_addr;

/* pmm.c */
void pmmInit(size_t mem_size, phys_addr bitmap);
inline void pmmMapSet(uint32_t bit);
inline void pmmMapUnset(uint32_t bit);
inline bool pmmMapTest(uint32_t bit); /* See if bit */
uint32_t pmmFirstMapFree(); /* Return first bit free */
void* pmmAllocPage();
void pmmDeallocPage(uintptr_t physical_addr);
void pmmInitRegion(uintptr_t base, size_t size);
void pmmDeinitRegion(uintptr_t base, size_t size);