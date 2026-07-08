#pragma once
/* Useful functions and structs will be found here */
/* Put this somehwere else like in a file named regs.h */
typedef struct
{
    /* Pushed by common stub (in reverse order) */
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    // Pushed by CPU automatically
    unsigned int eip, cs, eflags, useresp, ss;
} iregs_t;


