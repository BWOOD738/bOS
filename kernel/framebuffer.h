#pragma once

#include <stddef.h>

typedef struct 
{
    void *base_addr;
    size_t buffer_size;
    unsigned int width;
    unsigned int height;
    unsigned int pixels_per_scan_line;
} framebuffer_t;
