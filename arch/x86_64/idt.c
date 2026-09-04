#include "idt.h"
#include "kernel/pic.h"


#define MAX_DESCRIPTORS 256
#define IRQ_OFFSET 32

static idtr_t idtr;

__attribute__((aligned(0x10))) static idt_t idt[MAX_DESCRIPTORS];

extern void* isr_stub_table[]; /* In interrupts.s */
extern void* irq_stub_table[];
static bool vectors[MAX_DESCRIPTORS];

void idtInit()
{
    idtr.base = (uint64_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_t) * MAX_DESCRIPTORS - 1;
    /* Map ISRs */
    for (byte vector = 0; vector < 32; vector++)
    {
        idtSetGate(vector, isr_stub_table[vector], 0, 0x8E);
        vectors[vector] = true;
    }
    /* Remap descriptors */
    initPIC();

    /* Map IRQs */
    for (byte vector = 0; vector < 16; vector++)
    {
        idtSetGate(IRQ_OFFSET + vector, irq_stub_table[vector], 0, 0x8E);
        vectors[vector + IRQ_OFFSET] = true;
    }

    asm volatile("lidt %0" : : "m"(idtr)); /* load idt*/ 
    asm volatile("sti"); /* enable interrupts */
}

void idtSetGate(uint8_t vector, void* isr, uint8_t ist, uint8_t flags)
{
    idt_t* descriptor = &idt[vector];

    descriptor->isr_low = (uint64_t)isr & 0xFFFF;
    descriptor->selector = 0x28;
    descriptor->ist = ist & 0x7;
    descriptor->attributes = flags;
    descriptor->isr_mid = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high = (uint64_t)isr >> 32 & 0xFFFFFFFF;
    descriptor->resv = 0; /* Always zero */
}
