#pragma once

#include "types.h"

#if defined (__x86_64__)

typedef struct
{
    /* Pushed by common stub (in reverse order) */
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    /* Pushed by CPU automatically */
    uint64_t rip, cs, rflags, rsp, ss;
} iregs_t;

#endif