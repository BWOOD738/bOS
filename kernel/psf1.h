#pragma once 

#include "types.h"

/* File for the psf1 font header structs. This was moved from terminal.h so that the terminal header file focuses only on 
   terminal logic.
*/

typedef struct
{
    unsigned char magic[2];
    unsigned char mode;
    unsigned char charsize;
} psf1_header_t;

typedef struct
{
    psf1_header_t* psf1_header;
    void* glyph_buffer;
} psf1_font_t;