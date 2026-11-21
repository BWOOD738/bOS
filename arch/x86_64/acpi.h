#pragma once

#include "types.h"
#include "kernel/bootloader.h"
#include "kernel/mm/mm.h"
#include "kernel/lib/memory.h"

/* https://wiki.osdev.org/RSDP 
   https://github.com/dreamportdev/Osdev-Notes/blob/1998f44257f7bc870eb496b4afb7c34f78445c76/02_Architecture/06_ACPITables.md
*/

/*
Signature

An 8 byte magic number used for locating the RSDP, containing RSD PTR.

Checksum

    A byte used to verify the first 20 bytes of the RSDP.

OEMID

    An OEM-supplied string that identified the OEM.

Revision

    The RSDP revision, used for determining which fields are available.

RSDT Address

    A 32-bit physical address pointing to the RSDT.

Length

    The size of the XSDP (RSDP size if fixed, XSDP size might change in later revisions).

XSDT Address

    A 64-bit physical address pointing to the XSDT. If the revision is at least 2, the XSDT should be used regardless of architecture, as the RSDT was deprecated.

Extended Checksum

    A checksum used for the entire table. 

*/

typedef struct
{
    char signature[8];
    byte checksum;
    char oemid[6];
    byte revision;
    uint32_t rsdt_address;

    /* Revision 2.0+ */
    uint32_t length;
    uint64_t xsdt_address;
    byte checksum_ext;
    byte reserved[3];

} __attribute__((packed)) rsdp_t;

typedef struct 
{
    char signature[4];
    uint32_t length;
    byte revision;
    byte checksum;
    char oemid[6];
    char oem_tableid[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed)) acpi_sdt_t;

typedef struct 
{

} ;

void acpiInit();

acpi_sdt_t* getSdt(const char* signature);