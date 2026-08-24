#pragma once 

/* Common requests that are global will be here. Request definitions are defined in a .c file 
since defining it in a header causes linker issues when included in multiple files.
*/
#include "limine.h"

/* symbols from linker script */
extern uint64_t _kernel_start;
extern uint64_t _kernel_end;

extern volatile struct limine_hhdm_request g_hhdm_req;
extern volatile struct limine_memmap_request g_mmap_request;


typedef struct 
{
    /* Put all requests in this */
} bootloader_t;