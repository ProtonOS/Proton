#pragma once

// Namespaced?
typedef	signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;//int not long because long is 64bit on some platforms
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;

#ifdef IS64BIT
typedef int64 intptr;
typedef uint64 uintptr; 
#else
typedef int32 intptr;
typedef uint32 uintptr;
#endif
typedef uintptr size_t;
typedef intptr ptrdiff_t;

namespace Types {
    constexpr size_t SizeToShift(size_t n) {
        return n == 1 ? 0 : n == 2 ? 1 : n == 4 ? 2 : n == 8 ? 3 : n == 16 ? 4 : 0;
    }
    constexpr size_t SizeBytes = sizeof(size_t);
    constexpr size_t SizeMask = SizeBytes - 1;
    constexpr size_t SizeShift = SizeToShift(SizeBytes);
}