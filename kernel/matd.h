#pragma once

#include "types.h"
#include "arch/x86_64/acpi.h"

/* The Multiple APIC Description Table (MADT) is an ACPI table which provides informations necessary for operation on systems with APIC.
   This is going to be necessary to set up once multiple cores are being used by the kernel.
*/

/* https://wiki.osdev.org/MADT */

typedef struct
{
    

} madt_header_t;
