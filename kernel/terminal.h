#pragma once

#include "framebuffer.h"
#include "types.h"

#define WHITE 0xffffffff,
#define SILVER 0xffc0c0c0,
#define GRAY 0xff808080,
#define BGRAY 0xffC0C0C0,
#define DGRAY 0xff404040,
#define BLACK 0xff000000,
#define PINK 0xffFF1493,
#define GREEN 0xff008000,
#define RED 0xff800000,
#define PURPLE 0xff800080,
#define ORANGE 0xffFF4500,
#define CYAN 0xff008080,
#define YELLOW 0xffFFD700,
#define BROWN 0xffA52A2A,
#define BLUE 0xff000080,
#define DBLUE 0xff000030,
#define BRED 0xffFF0000,
#define BBLUE 0xff0000FF,
#define BGREEN 0xff00FF00,
#define TBLACK 0x00000000;

typedef struct 
{
    unsigned int x;
    unsigned int y;
} point_t;

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

typedef struct 
{
    point_t cursor_pos;
    framebuffer_t* fb;
    psf1_font_t* psf1_font;

    uint32_t text_color;
    bool overwrite;
} terminal_t;

void initTerminal(terminal_t* terminal, framebuffer_t* framebuffer, psf1_font_t* psf1_font);
void print(terminal_t* terminal, const char* string);
void putcTerminal(terminal_t* terminal, char chr, unsigned int xoff, unsigned int yoff);
void clear(terminal_t* terminal, uint32_t color, bool reset);
void newLine();

extern terminal_t* global_terminal;