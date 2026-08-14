#include "gdt.h"
#include "util.h"
#include "kernel/mm/memory.h"

extern void gdtFlush(uint32_t);
extern void tssFlush(void);

gdt_t gdt_entries[7];
gdt_ptr_t gdt_ptr;
tss_t tss_entry;

void gdtInit()
{
    gdt_ptr.limit = (sizeof(gdt_t) * 7) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;
    
    /* null segment */
    setGdtGate(0,0,0,0,0);
    /* kernel code segment. Sets up 4kb paging */
    setGdtGate(1, 0, 0xFFFFFFFF, 0x9A, 0x20);
    /* Kernel data segment */
    setGdtGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    /* Segment for user code */
    setGdtGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    /* User data segment */
    setGdtGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    tssInit(5, 0x10, 0x0);

    gdtFlush((uint32_t)&gdt_ptr);

    tssFlush();
}

void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_mid = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;
    gdt_entries[num].base_upper =( base >> 32) & 0xFFFFFFFF;
    gdt_entries[num].limit = (limit & 0xFFFF);
    gdt_entries[num].flags = (limit >> 16) & 0x0F;
    gdt_entries[num].flags |= (gran & 0xF0);
    gdt_entries[num].access = access;
    gdt_entries[num].resv = 0;
}

void tssInit(uint32_t num, uint16_t ss0, uint32_t esp0)
{
    uint32_t base = (uint32_t)&tss_entry; /* store address of tss */
    uint32_t limit = sizeof(tss_entry) - 1; // Subtracting 1 could fix the issue. Also not adding base 

    /* Init all fields in tss to zero using memset */
    memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.ss0 = ss0; 
    tss_entry.esp0 = esp0;
    tss_entry.cs = 0x08 | 0x03; 
    tss_entry.ss = 0x10 | 0x03;  
    tss_entry.ds = 0x10 | 0x03;
    tss_entry.es = 0x10 | 0x03;
    tss_entry.fs = 0x10 | 0x03;
    tss_entry.gs = 0x10 | 0x03;

    setGdtGate(num, base, limit, 0xE9, 0x00);

}

