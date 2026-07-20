#include "memory.h"

void mmInit(multiboot_info_t* bootInfo)
{
    for (int i = 0; i < bootInfo->mmap_length; i += sizeof(multiboot_mmap_entry_t))
    {
        multiboot_mmap_entry_t* mmmt = (multiboot_mmap_entry_t*)(bootInfo->mmap_addr + i);
    }
}

void* memset(void* destination, char val, unsigned int count)
{
    char* temp = (char*)destination;
    
    for(; count != 0; count--)
    {
        *temp++ = val;
    }

}