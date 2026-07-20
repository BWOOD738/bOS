#pragma once

#include <stddef.h>
#include "mm.h"

#define PAGES_PER_BYTE 0x8
#define PAGE_SIZE 0x1000
#define PAGE_ALIGN PAGE_SIZE


size_t pmmGetMemorySize();
uint32_t pmmGetPageCount();
uint32_t pmmGetUsedPageCount();
uint32_t pmmGetFreePageCount();
