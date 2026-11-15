#include "terminal.h"

/* TODO: Scroll function */

static uint32_t glyphCount(const psf1_font_t* font)
{
    /* psf1 mode bit 0: 1 => 512 glyphs, 0 => 256 glyphs. */
    return (font->psf1_header->mode & 0x01) ? 512u : 256u;
}

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
    
}

static void putcTerminal(terminal_t* terminal, char chr, unsigned int xoff, unsigned int yoff)
{
    uint64_t char_height = terminal->psf1_font->psf1_header->charsize;

    if (chr >= glyphCount(terminal->psf1_font))
        return;
    /* Dont read past glyph buffer width or height */
    if (xoff + 8 > terminal->fb->width ||
        yoff + char_height > terminal->fb->height)
        return;

    uint64_t fb_base = (uint64_t)terminal->fb->base_addr;
    uint64_t pixels_per_scanline = terminal->fb->pixels_per_scan_line;
    
    const unsigned char* glyph = (const unsigned char*)terminal->psf1_font->glyph_buffer + (chr * char_height); 

    for (uint64_t y = 0; y < char_height; y++)
    {
        byte row = glyph[y];
        for (uint64_t x = 0; x < 8; x++)
        {
            if (row & (0x80u >> x))
            {
                uint64_t pixel = (xoff + x) + ((yoff + y) * pixels_per_scanline);
                *(uint32_t*)(fb_base + pixel * 4) = terminal->text_color;
            }
        }
    }
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


void clear(terminal_t* terminal, uint32_t color, bool reset)
{
    uint64_t fb_base = (uint64_t)terminal->fb->base_addr;
    uint64_t pxls_per_scan_line = terminal->fb->pixels_per_scan_line;
    uint64_t fb_height = terminal->fb->height;

    for (int64_t y = 0; y < fb_height; y++)
    {
        for (int64_t x = 0; x < terminal->fb->width; x++)
        {
            *((uint32_t *)(fb_base + 4 * (x + pxls_per_scan_line * y))) = color;
        }
    }

    if (reset)
    {
        terminal->cursor_pos.x = 0;
        terminal->cursor_pos.y = 0;
    }

    return;
}