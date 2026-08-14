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
    uint32_t base_upper;
    uint32_t resv; 
} gdt_t;

typedef struct __attribute__((packed))
{
    uint16_t limit;
    uint64_t base;
} gdt_ptr_t;

typedef struct __attribute__((packed))
{
    uint32_t resv0;     
    uint64_t rsp0;          /*Ring 0 stack pointer*/
    uint64_t rsp1;          
    uint64_t rsp2;        
    uint64_t resv1;     
    uint64_t ist1;          /*Interrupt Stack Table entries*/
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t resv2;     
    uint16_t resv3;     
    uint16_t iomap_base; 
} tss_t;


void gdtInit();
void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void tssInit(uint32_t num, uint16_t ss0, uint32_t esp0);
