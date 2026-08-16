#pragma once 

/* Common requests that are global will be here. Request definitions are defined in a .c file 
since defining it in a header causes linker issues when included in multiple files.
*/
#include "limine.h"

extern volatile struct limine_hhdm_request g_hhdm_req;