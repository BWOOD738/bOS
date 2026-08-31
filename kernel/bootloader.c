#include "bootloader.h"

__attribute__((used, section(".limine_requests")))
volatile struct limine_hhdm_request g_hhdm_req = 
{
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
volatile struct limine_memmap_request g_mmap_request = 
{
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0
};