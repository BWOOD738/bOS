#pragma once

typedef struct 
{
    int tabsize;
    int strsize;
    int addr;
    int reserved;
} multiboot_aout_symbol_table_t;

typedef struct 
{
    int num;
    int size;
    int addr;
    int shndx;
} multiboot_elf_section_header_table_t;

typedef struct 
{
    int flags;
    int mem_lower;
    int mem_upper;
    int boot_device;

    int cmdline;
    int mods_count;
    int mods_addr;

    union
    {
        multiboot_aout_symbol_table_t aout_sym;
        multiboot_elf_section_header_table_t elf_sec;
    } u;

    int mmap_length;
    int mmap_addr;

    int drives_length;
    int drives_addr;

    int config_table;
    int boot_loader_name;

    int apm_table;

    int vbe_control_info;
    int vbe_mode_info;
    short int vbe_mode;
    short int vbe_interface_seg;
    short int vbe_interface_off;
    short int vbe_interface_len;
} multiboot_info_t;

typedef struct __attribute__((packed))
{
    int size;
    /* These two must be split into a low and high address in 32 bit mode. Change for 64 bit*/
    int addr_low;
    int addr_high;

    int len_low;
    int len_high;

#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5

    int type;
} multiboot_mmap_entry_t;