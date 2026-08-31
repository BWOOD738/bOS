#pragma once

#include <types.h>
#include <stddef.h> /* size_t */
#include <stdint.h>

/* This file will be used as an interface for the rest of the main files in the mm directory. Important functions for memory
management will be declared and defined elsewhere. 
*/

#define PAGE_SIZE 0x1000
#define NUM_PAGES(num)          (((num) + PAGE_SIZE - 1) / PAGE_SIZE)

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

/* vmm.c */

typedef uint64_t pml4e_t;
typedef uint64_t pdpte_t;
typedef uint64_t pde_t;
typedef uint64_t pte_t;

__attribute__((packed, aligned(PAGE_SIZE)))
typedef struct 
{
    pml4e_t entries[512];
} pml4_t;

__attribute__((packed, aligned(PAGE_SIZE)))
typedef struct 
{
    pdpte_t entries[512];
} pdpt_t;

__attribute__((packed, aligned(PAGE_SIZE)))
typedef struct 
{
    pde_t entries[512];
} pagedir_t;

__attribute__((packed, aligned(PAGE_SIZE)))
typedef struct 
{
    pte_t entries[512];
} pagetbl_t;


void vmmInit();
uint64_t vmmPhysicalToVirtual(physaddr_t physical);
void vmmMapPage(physaddr_t physical, virtaddr_t virtual, uint64_t flags);
void vmmUnmapPage();

/* tlb.c */