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

inline uint64_t readCR0()
{
    uint64_t value;

    asm volatile("movq %%cr0, %0" : "=r"(value) :: "memory");
    return value;
}

inline uint64_t readCR3()
{
    uint64_t value;

    asm volatile("movq %%cr3, %0" : "=r"(value) :: "memory");
    return value;
}

inline uint64_t writeCR3(uint64_t value)
{
    asm volatile("movq %0, %%cr0" :: "r"(value) : "memory");
    return value;
}

inline uint64_t getAddressWidth()
{
    uint64_t eax = 0x80000008;
    asm volatile (
        "cpuid"
        : "=a"(eax)
        : "a" (eax)
    );

    return eax;
}

#endif