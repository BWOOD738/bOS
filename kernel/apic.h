#pragma once 

/* Basic driver for the Advanced Programmable Interrupt Controller (APIC). This file should be moved to its own directory at some point*/

#include "types.h"
#include "pic.h"

typedef struct 
{
    volatile void* lapic_base; /* Base physical address for the Local APIC */
} lapic_t;


void apicInit();

void writeApic();

void readApic();