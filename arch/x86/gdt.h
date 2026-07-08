#pragma once

#include "types.h" 

typedef struct __attribute__((packed))
{
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t flags;
    uint8_t base_high;
} gdt_t;

typedef struct __attribute__((packed))
{
    uint16_t limit;
    uint32_t base;
} gdt_ptr_t;

typedef struct __attribute__((packed))
{
    uint32_t prev_task_seg;
    uint32_t esp0, esp1, esp2; /* Stack pointer to load when switching task/contexts */
    uint32_t ss0, ss1, ss2; /* kernel stack segments */
    uint32_t cr3;
    uint32_t eax, ebx, ecx, edx, esp, ebp, esi, edi; /* pushed by pusha */
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t eflags;
    uint32_t ldt; 
    uint32_t trap;
    uint32_t iomap;
} tss_t;


void gdtInit();
void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void tssInit(uint32_t num, uint16_t ss0, uint32_t esp0);
