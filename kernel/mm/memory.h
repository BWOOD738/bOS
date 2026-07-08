#pragma once 

#include "types.h"
#include "../multiboot.h"

typedef struct 
{
    byte status;
    uint32_t size;
} alloc_t;

void malloc();
void free();

void* memset(void* destination, char val, unsigned int count);

void mmInit(multiboot_info_t* bootInfo);
