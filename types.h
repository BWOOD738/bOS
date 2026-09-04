#pragma once

#if defined(__i386__)
typedef unsigned char byte;

typedef byte uint8_t;
typedef signed char int8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

typedef signed long int int32_t;
typedef unsigned long int uint32_t;


typedef signed long long int int64_t;
typedef unsigned long long int uint64_t;
#endif

#if defined (__x86_64__)
    #include <stdint.h>
    #include <stddef.h>
    
    typedef unsigned char byte;

    typedef byte uint8_t;

#endif

#define bool int
#define true 1
#define false 0
