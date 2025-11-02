// stdint.h - Standard integer típusok
#ifndef STDINT_H
#define STDINT_H

// Előjel nélküli típusok
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

// Előjeles típusok
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

// Legnagyobb értékeket tartalmazó típusok
typedef long intptr_t;
typedef unsigned long uintptr_t;
typedef unsigned long size_t;
typedef long ssize_t;

#endif // STDINT_H