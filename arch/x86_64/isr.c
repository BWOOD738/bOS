#include "isr.h"
#include "kernel/kprintf.h"
#include "kernel/io.h"
#include "kernel/pic.h"

char* exceptions[] = 
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Fault",
    "Machine Check", 
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void* irq_routines[16] =
{
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

void isrExceptionHandler(iregs_t* regs)
{   
    kprintf("INT: %d, ERR: 0x%x, EIP: 0x%x\n", 
           regs->int_no, regs->err_code, regs->rip);

    if(regs->int_no < 32)
    {

        kprintf("Exception! %s", exceptions[regs->int_no]);
        asm volatile("cli; hlt");
    }
}

void irqHandler(iregs_t* regs)
{
    void(*handler)(iregs_t* regs);

    handler = irq_routines[regs->int_no - 32];

    if(handler)
        handler(regs);
    
    
    sendEoi(regs->int_no - 32);
}

void irqInstall(int irq, void(*handler)(iregs_t* regs))
{
    irq_routines[irq] = handler;
}

void irqUninstall(int irq)
{
    irq_routines[irq] = 0;
}