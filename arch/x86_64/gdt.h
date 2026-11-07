#pragma once

#include "types.h" 

/*
    This GDT will replace Limine's initial GDT which only lets the kernel run in ring 0.
    This GDT will create the separation between Ring 3 and Ring 0
*/

typedef struct 
{
  uint16_t limit;
  uint16_t base_low;
  uint8_t  base_mid;
  uint8_t  access;
  uint8_t  granularity;
  uint8_t  base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct 
{
  uint16_t length;
  uint16_t base_low;
  uint8_t  base_mid;
  uint8_t  flags1;
  uint8_t  flags2;
  uint8_t  base_high;
  uint32_t base_upper32;
  uint32_t reserved;
} __attribute__((packed)) tss_entry_t;

typedef struct 
{
  uint32_t unused0;
  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;
  uint64_t unused1;
  uint64_t ist1;
  uint64_t ist2;
  uint64_t ist3;
  uint64_t ist4;
  uint64_t ist5;
  uint64_t ist6;
  uint64_t ist7;
  uint64_t unused2;
  uint32_t iopb;
} __attribute__((packed)) tss_ptr_t;

typedef struct 
{
  gdt_entry_t descriptors[11];
  tss_entry_t tss;
} __attribute__((packed)) gdt_entries_t;

typedef struct  
{
  uint16_t limit;
  uint64_t base;
} __attribute__((packed)) gdt_ptr_t;

#define NUM_GDT_ENTRIES 7
#define GDT_KERNEL_CODE 40
#define GDT_KERNEL_DATA 48
#define GDT_USER_CODE 80
#define GDT_USER_DATA 72
#define GDT_TSS 80

void gdtInit();