#pragma once

/* This file will be used at one point to allow for device regestration and tracking */

typedef enum
{
    DEVICE_UNKNOWN = 0,
    DEVICE_PS2,
    DEVICE_PCI
} device_type_t;

typedef struct 
{
    device_type_t* devtype;
    const char* name;

} device_t;
