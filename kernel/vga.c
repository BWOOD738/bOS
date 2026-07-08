#include <stddef.h>

#include "vga.h"
#include "types.h"


/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(unsigned char color) 
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) 
{
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = VGA_HEIGHT - 1;
            scrollUp();
        }
    }
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void newLine()
{
    terminal_column = 0;
    if(terminal_row < VGA_HEIGHT - 1) {
        terminal_row++;
    } else {
        scrollUp();
    }
}

void scrollUp()
{
    for(uint16_t y = 1; y < VGA_HEIGHT; y++)
    {
        for(uint16_t x = 0; x < VGA_WIDTH; x++)
        {
            terminal_buffer[(y-1) * VGA_WIDTH + x ] = terminal_buffer[y * VGA_WIDTH + x];
        }
    }

    for (uint16_t x = 0; x < VGA_WIDTH; x++)
    {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ' ' | terminal_color;
        
    }

}




/*
uint16_t column = 0;
uint16_t line = 0;

uint16_t* const vga = (uint16_t* const) 0xC00D8000;
const uint16_t defaultColor = (COLOR8_LIGHT_GREY << 8)  | (COLOR8_BLACK << 12);
uint16_t currColor = defaultColor;

void reset()
{
    line = 0;
    column = 0;
    currColor = defaultColor;

    for(uint16_t y = 0; y < height; y++)
    {
        for(uint16_t x = 0; x < width; x++)
        {
            vga[y * width + x] = ' ' | defaultColor;
        }
    }
}

void newLine()
{
    if(line < height - 1)
    {
        line++;
        column = 0;
    }
    else
    {
        scrollUp();
        column =0;
    }
}

void scrollUp()
{
    for(uint16_t y = 0; y < height; y++)
    {
        for(uint16_t x = 0; x < width; x++)
        {
            vga[(y-1) * width + x ] = vga[y * width + x];
        }
    }

    for (uint16_t x = 0; x < width; x++)
    {
        vga[(height - 1) * width + x] = ' ' | currColor;
        
    }

}

void print(const char* s)
{
    while(*s)
    {
        switch(*s)
        {
            case '\n':
                newLine();
                break;
            case '\r':
                column = 0;
                break;
            case '\b':
                if (column == 0 && line != 0)
                {
                    line--;
                    column = width;
                }
                vga[line * width + (--column)] = ' ' | currColor;
                break;
            case '\t':
                if(column == width)
                    newLine();
                uint16_t tabLen = 4 - (column % 4);
                while(tabLen != 0)
                {
                    vga[line * width + (column++)] = ' ' | currColor;
                    tabLen--;
                }
                break;
            default:
                if (column == width)
                    newLine();
                
                vga[line * width + (column++)] = *s | currColor;
                break;
            
        }
        s++;
    }
}*/