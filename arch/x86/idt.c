#include "idt.h"
#include <types.h>
#include "kernel/pic.h"
#include <stdint.h>

#define MAX_DESCRIPTORS 256
#define IRQ_OFFSET 32

static idtr_t idtr;

__attribute__((aligned(0x10))) static idt32_t idt[MAX_DESCRIPTORS];

extern void* isr_stub_table[]; /* In interrupts.s */
extern void* irq_stub_table[];
static bool vectors[MAX_DESCRIPTORS];

void idtInit()
{
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt32_t) * MAX_DESCRIPTORS - 1;
    /* Map ISRs */
    for (byte vector = 0; vector < 32; vector++)
    {
        idtSetGate(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }
    /* Remap descriptors */
    initPIC();

    /* Map IRQs */
    for (byte vector = 0; vector < 16; vector++)
    {
        idtSetGate(IRQ_OFFSET + vector, irq_stub_table[vector], 0x8E);
        vectors[vector + IRQ_OFFSET] = true;
    }

    asm volatile("lidt %0" : : "m"(idtr)); // load idt 
    asm volatile("sti"); // enable interrupts 
}

void idtSetGate(uint8_t vector, void* isr, uint8_t flags)
{
    idt32_t* descriptor = &idt[vector];

    descriptor->low = (uint32_t)isr & 0xFFFF;
    descriptor->selector = 0x08;
    descriptor->zero = 0;
    descriptor->attributes = flags;
    descriptor->high = (uint32_t)isr >> 16 & 0xFFFF;
}
