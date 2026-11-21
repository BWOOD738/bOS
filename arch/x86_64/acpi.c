#include "acpi.h"

static bool use_xsdt = false; /* Should we use revision 2 or not */
static acpi_sdt_t* sdt;

void acpiInit()
{
    rsdp_t* rsdp = (rsdp_t*)g_rsdp_request.response->address;

    uint64_t sdt_phys; 

    if (rsdp->revision == 2) 
    {
        sdt_phys = rsdp->xsdt_address;
        use_xsdt = true;
    } 
    else 
    {
        sdt_phys = rsdp->rsdt_address;
        use_xsdt = false;
    }

    /* TODO: Implement madtInit() and put it here so that it can read the ACPI tables */
}

acpi_sdt_t* getSdt(const char* signature)
{
}
