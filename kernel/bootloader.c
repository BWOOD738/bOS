#include "bootloader.h"

__attribute__((used, section(".limine_requests")))
volatile struct limine_hhdm_request g_hhdm_req = 
{
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};