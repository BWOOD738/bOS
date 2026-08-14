#pragma once

#include <stdint.h>

typedef struct __attribute__((packed))
{
    uint16_t isr_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t resv;
} idt_t;

typedef struct __attribute__((packed))
{
    uint16_t limit;
    uint64_t base;
} idtr_t;

void idtInit();
void idtSetGate(uint8_t vector, void* isr, uint8_t ist, uint8_t flags);