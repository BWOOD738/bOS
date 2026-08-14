#pragma once

#include "arch/regs.h"


__attribute__((noreturn))
void isrExceptionHandler(iregs_t* regs);
void irqHandler(iregs_t* regs);

void irqInstall(int irq, void(*handler)(iregs_t* regs));
void irqUninstall(int irq);
