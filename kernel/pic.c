#include "pic.h"
#include "io.h"

void sendEoi(uint8_t irq)
{
    if(irq >= 8)
    {
        outb(PIC2_CMD, PIC_EOI);
    }
    outb(PIC1_CMD, PIC_EOI);
}

void initPIC()
{
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    ioWait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    ioWait();
    outb(PIC1_DATA, 0x20);
    ioWait();
    outb(PIC2_DATA, 0x28);
    ioWait();
    outb(PIC2_DATA, 1 << CASECADE_IRQ);
    ioWait();
    outb(PIC2_DATA, 2);
    ioWait();

    outb(PIC1_DATA, ICW4_8086);
    ioWait();
    outb(PIC2_DATA, ICW4_8086);
    ioWait();

    outb(PIC1_DATA, 0);
    ioWait();
    outb(PIC2_DATA, 0);
    ioWait();
}

void irqMask(uint8_t irq)
{
    uint16_t port;
    uint8_t val;

    if(irq < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }
    val = inb(port) | (1 << irq);
    outb(port, val);
}

void irqUnmask(uint8_t irq)
{
    uint16_t port;
    uint8_t val;

    if(irq < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }
    val = inb(port) & ~(1 << irq);
    outb(port, val);
}