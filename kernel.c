#include "arch/x86_64/idt.h"
//#include "arch/x86/gdt.h"
#include "limine.h"
//#include "kernel/mm/memory.h"
//#include "kernel/vga.h"
#include "kernel/kprintf.h"
#include "kernel/timer.h"
#include "kernel/devices/keyboard_ps2.h"
/* Maybe move pmmInit to mm.h?*/
//#include "kernel/mm/mm.h"
//#include "kernel/mm/pmm.h"
#include "types.h"
#include "kernel/framebuffer.h"
#include "kernel/terminal.h"
#include <stddef.h>
#include <stdint.h>



// Set the base revision to 6, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};


__attribute__((used, section(".limine_requests")))
static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST_ID,
    .revision = 0
};

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

static void hcf() {
    for (;;) {
        asm ("hlt");
    }
}



bool checkStringEndsWith(const char* str, const char* end)
{
    const char* _str = str;
    const char* _end = end;

    while(*str != 0)
        str++;
    str--;

    while(*end != 0)
        end++;
    end--;

    while (true)
    {
        if (*str != *end)
            return false;

        str--;
        end--;

        if (end == _end || (str == _str && end == _end))
            return true;

        if (str == _str)
            return false;
    }

    return true;
}

struct limine_file* getFile(const char* name)
{
        if (module_request.response == NULL || 
        module_request.response->module_count == 0) {
        return NULL;
    }
    
    // Search through modules by name
    for (uint64_t i = 0; i < module_request.response->module_count; i++) {
        struct limine_file *file = module_request.response->modules[i];
        
        // Compare filename (file->path might include path, not just filename)
        const char *filename = file->path;
        // Skip path separators to get just the filename
        const char *basename = filename;
        for (const char *p = filename; *p != '\0'; p++) {
            if (*p == '/') basename = p + 1;
        }
        
        // Compare strings
        const char *a = name;
        const char *b = basename;
        while (*a && *b && *a == *b) {
            a++;
            b++;
        }
        if (*a == '\0' && *b == '\0') {
            return file;
        }
    }
    
    return NULL;
}

static framebuffer_t fb;
static psf1_font_t font;
static terminal_t t;
terminal_t* global_terminal;

void kmain(void);

void kmain(void)
{

    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
       hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    
    {
        fb.base_addr = framebuffer->address;
        fb.width = framebuffer->width;
        fb.height = framebuffer->height;
        fb.pixels_per_scan_line = framebuffer->pitch / 4;
        fb.buffer_size = framebuffer->height * framebuffer->pitch;
    }

  
    {
        const char *fName = "zap-light16.psf";
        struct limine_file *file = getFile(fName);
        if (file == NULL)
        {
            hcf();
        }

        font.psf1_header = (psf1_header_t*)file->address;
        if (font.psf1_header->magic[0] != 0x36 || font.psf1_header->magic[1] != 0x04)
        {
            hcf();
        }

        font.glyph_buffer = (void *)((uint64_t)file->address + sizeof(psf1_header_t));
    }

   
    initTerminal(&t, &fb, &font);
    global_terminal = &t;
    
    clear(global_terminal, 0xff000000, true);
    kprintf("Testing\n");

    kprintf("Enabling interrupts...");
    idtInit();
    kprintf("Interrupts enabled...\n");

    kprintf("Initializing PIT...");
    pitInit();
    kprintf("PIT initialized...\n");

    kprintf("Initializing keyboard...");
    initKeyboardPS2();
    kprintf("Keyboard initialized...");

    hcf();
}