#include <stddef.h>

#include "terminal.h"


void initTerminal(terminal_t* terminal, framebuffer_t* framebuffer, psf1_font_t* psf1_font)
{
    if (terminal == NULL || framebuffer == NULL || psf1_font == NULL) 
    {
        asm volatile("hlt");
    }

    terminal->text_color =  0xffffffff;
    terminal->cursor_pos.x = 0;
    terminal->cursor_pos.y = 0;

    terminal->fb = framebuffer;
    terminal->psf1_font = psf1_font;


    volatile uint32_t* fb_ptr = (volatile uint32_t*)framebuffer->base_addr;
    
    // Write to multiple positions to ensure we're hitting visible area
    fb_ptr[0] = 0xFFFFFFFF;           // White pixel at top-left
    fb_ptr[100] = 0xFFFF0000;         // Red pixel
    fb_ptr[200] = 0xFF00FF00;         // Green pixel
    fb_ptr[300] = 0xFF0000FF;         // Blue pixel
    
    // Also try writing to center of screen
    uint32_t center = (framebuffer->height/2 * framebuffer->pixels_per_scan_line) + (framebuffer->width/2);
    fb_ptr[center] = 0xFFFF0000;

    return;
}

void print(terminal_t* terminal, const char* string)
{
    char *chr = (char *)string;
    while (*chr != 0)
    {
        switch (*chr)
        {
            case '\n':
                terminal->cursor_pos.x = 0;
                terminal->cursor_pos.y += 16;
                break;

            case '\t':
                terminal->cursor_pos.x += 8;
                break;
            
            default:
                putcTerminal(terminal, *chr, terminal->cursor_pos.x, terminal->cursor_pos.y);
                terminal->cursor_pos.x += 8;
                break;
        }

        if (terminal->cursor_pos.x + 8 > terminal->fb->width)
        {
            terminal->cursor_pos.x = 0;
            terminal->cursor_pos.y += 16;
        }

        chr++;
    }
}

void putcTerminal(terminal_t* terminal, char chr, unsigned int xoff, unsigned int yoff)
{
    uint64_t fb_base = (uint64_t)terminal->fb->base_addr;
    uint64_t pixels_per_scanline = terminal->fb->pixels_per_scan_line;
    
    char* fontPtr = (char*)terminal->psf1_font->glyph_buffer + (chr * terminal->psf1_font->psf1_header->charsize);

    for (uint64_t y = yoff; y < yoff + 16; y++)
    {
        for (uint64_t x = xoff; x < xoff + 8; x++)
        {
            if ((*fontPtr & (0b10000000 >> (x - xoff))) > 0)
            {
                
                uint64_t pixel_offset = x + (y * pixels_per_scanline);
                uint64_t byte_offset = pixel_offset * 4; 
                
                *(uint32_t*)(fb_base + byte_offset) = terminal->text_color;
            }
        }
        fontPtr++;
    }
}


void clear(terminal_t* terminal, uint32_t color, bool reset)
{
    uint64_t fb_base = (uint64_t)terminal->fb->base_addr;
    uint64_t pxlsPerScanline = terminal->fb->pixels_per_scan_line;
    uint64_t fb_height = terminal->fb->height;

    for (int64_t y = 0; y < terminal->fb->height; y++)
    {
        for (int64_t x = 0; x < terminal->fb->width; x++)
        {
            *((uint32_t *)(fb_base + 4 * (x + pxlsPerScanline * y))) = color;
        }
    }

    if (reset)
    {
        terminal->cursor_pos.x = 0;
        terminal->cursor_pos.y = 0;
    }

    return;
}