#pragma once

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

    void(*init)(device_t* device);
} device_t;
