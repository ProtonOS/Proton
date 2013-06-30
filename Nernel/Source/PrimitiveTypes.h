#pragma once

// Namespaced?
typedef	signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed long int32;
typedef unsigned long uint32;

typedef signed long long int64;
typedef unsigned long long uint64;

#ifdef IS64BIT
typedef int64 intptr;
typedef uint64 uintptr;
#else
typedef int32 intptr;
typedef uint32 uintptr;
#endif
