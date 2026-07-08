#include "timer.h"
#include "arch/x86/isr.h"
#include  "io.h"
#include "util.h"

static uint64_t ticks;
static const uint32_t freq = 100;

/* Tell the PIT to increase ticks when IRQ0 is called */
void pitIrq(iregs_t* regs)
{
    ticks += 1;
}

void pitInit()
{
    ticks = 0;
    irqInstall(TIMER_IRQ, &pitIrq);

    unsigned int divisor = 1193188/freq;

    outb(0x43, 0x36);
    outb(0x40, (byte)(divisor & 0xFF));
    outb(0x40, (byte)((divisor >> 8) & 0xFF));
}

unsigned int readPitCount()
{
    uint32_t count = 0;

    asm volatile("cli");

    outb(0x43, 0b0000000);

    count = inb(0x40); /* Low byte */
    count |= inb(0x40) << 8; /* High byte */

    return count;
}