#pragma once

#include <stdint.h>

typedef struct __attribute__((packed))
{
    uint16_t low;
    uint16_t selector;
    uint8_t zero;
    uint8_t attributes;
    uint16_t high;
} idt32_t;

typedef struct __attribute__((packed))
{
    uint16_t limit;
    uint32_t base;
} idtr_t;

void idtInit();
void idtSetGate(uint8_t vector, void* isr, uint8_t flags);