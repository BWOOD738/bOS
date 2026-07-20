/* Taken from Bran's Kernel Dev tutorial */

/* Linker script for flat memory model x86 kernel */

ENTRY(_start)

SECTIONS
{
    /* Kernel starts at 1MB - where GRUB loads it */
    . = 1M;
    
    _kernel_start = .;

    /* Multiboot header must come first */
    .multiboot BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
    }
    
    /* Code section */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.text)
    }
    
    /* Read-only data */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }
    
    /* Initialized data */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }
    
    /* Uninitialized data and stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
    
    /* End of kernel marker */
    _kernel_end = .;
}



