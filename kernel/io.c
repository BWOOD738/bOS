#include "io.h"

void outb(short unsigned int port, unsigned char value)
{
    asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}

unsigned char inb(unsigned short int port)
{
    char rv;
    asm volatile("inb %1, %0": "=a"(rv):"dN"(port));
    return rv;
}

void ioWait()
{
    outb(0x80, 0);
}