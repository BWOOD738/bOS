#include "kprintf.h"
#include "terminal.h"

void putc(char c)
{
    if (c == '\n') 
    {
        global_terminal->cursor_pos.x = 0;
        global_terminal->cursor_pos.y += 16;  
    } 
    else if (c == '\t')
    {
        global_terminal->cursor_pos.x += 32;  // 4 spaces * 8 pixels
    }
    else 
    {
        putcTerminal(global_terminal, c, 
                     global_terminal->cursor_pos.x, 
                     global_terminal->cursor_pos.y);
        global_terminal->cursor_pos.x += 8;
    }
    
    /* For line wrap */
    if (global_terminal->cursor_pos.x + 8 > global_terminal->fb->width)
    {
        global_terminal->cursor_pos.x = 0;
        global_terminal->cursor_pos.y += 16;
    }
}

void puts(const char* s)
{
    while (*s)
    {
        putc(*s);
        s++;
    }
}

void kprintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    int state = PRINTF_STATE_START;
    int length = PRINTF_LENGTH_START;
    int radix = 10;
    bool sign = false;

    while (*fmt)
    {
        switch(state)
        {
        case PRINTF_STATE_START:
            if (*fmt == '%')
            {
                state = PRINTF_STATE_LENGTH;
            }
            else
            {
                putc(*fmt);
            }
            break;
            
        case PRINTF_STATE_LENGTH:
            if (*fmt == 'h')
            {
                length = PRINTF_LENGTH_SHORT;
                state = PRINTF_STATE_SHORT;
            }
            else if (*fmt == 'l')
            {
                length = PRINTF_LENGTH_LONG;
                state = PRINTF_STATE_LONG;
            }
            else
            {
                goto PRINTF_STATE_SPEC_;
            }
            break;
            
        case PRINTF_STATE_SHORT:
            if (*fmt == 'h')
            {
                length = PRINTF_LENGTH_SHORT_SHORT;
                state = PRINTF_STATE_SPEC;
            }
            else
            {
                goto PRINTF_STATE_SPEC_;
            }
            break;

        case PRINTF_STATE_LONG:
            if (*fmt == 'l')
            {
                length = PRINTF_LENGTH_LONG_LONG;
                state = PRINTF_STATE_SPEC;
            }
            else
            {
                goto PRINTF_STATE_SPEC_;
            }
            break;

        case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
            switch(*fmt)
            {
                case 'c':
                    putc((char)va_arg(args, int));
                    break;
                    
                case 's':
                {
                    const char* str = va_arg(args, const char*);
                    puts(str);
                    break;
                }
                
                case '%':
                    putc('%');
                    break;
                    
                case 'd':
                case 'i':
                    radix = 10;
                    sign = true;
                    kprintfNumber(args, length, sign, radix);
                    break;
                    
                case 'u':
                    radix = 10;
                    sign = false;
                    kprintfNumber(args, length, sign, radix);
                    break;
                    
                case 'X':
                case 'x':
                case 'p':
                    radix = 16;
                    sign = false;
                    kprintfNumber(args, length, sign, radix);
                    break;
                    
                case 'o':
                    radix = 8;
                    sign = false;
                    kprintfNumber(args, length, sign, radix);
                    break;
                    
                default:
                    break;
            }
            
            state = PRINTF_STATE_START;
            length = PRINTF_LENGTH_START;
            radix = 10;
            sign = false;
            break;
        }
        fmt++;
    }
    
    va_end(args);
}


const char possibleChars[] = "0123456789abcdef";

int* kprintfNumber(va_list args, int length, bool sign, int radix){
    
    unsigned long long num;
    
    if (length == PRINTF_LENGTH_LONG_LONG)
    {
        if (sign)
            num = va_arg(args, long long);
        else
            num = va_arg(args, unsigned long long);
    }
    else if (length == PRINTF_LENGTH_LONG)
    {
        if (sign)
            num = va_arg(args, long);
        else
            num = va_arg(args, unsigned long);
    }
    else
    {
        if (sign)
            num = va_arg(args, int);
        else
            num = va_arg(args, unsigned int);
    }
    
    // Convert and print
    char buf[32];
    char* p = buf + sizeof(buf) - 1;
    *p = '\0';
    
    if (!sign && (long long)num < 0)
    {
        // Unsigned
        unsigned long long unum = num;
        do {
            *--p = "0123456789ABCDEF"[unum % radix];
            unum /= radix;
        } while (unum > 0);
    }
    else if (sign && (long long)num < 0)
    {
        // Signed negative
        long long snum = num;
        do {
            *--p = "0123456789ABCDEF"[-(snum % radix)];
            snum /= radix;
        } while (snum != 0);
        *--p = '-';
    }
    else
    {
        // Positive
        do {
            *--p = "0123456789ABCDEF"[num % radix];
            num /= radix;
        } while (num > 0);
    }
    
    puts(p);
}