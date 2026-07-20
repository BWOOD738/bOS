#pragma once

#include <types.h>

/* TODO: Replace PIT with HPET and just use HPET as default and PIT as fallback */

#define TIMER_IRQ 0

void pitInit();
void sleep();
unsigned int readPitCount();
