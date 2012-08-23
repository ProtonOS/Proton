#pragma once

#ifdef _WIN32

#define MAYBE (TRUE || FALSE)

#undef __asm__
#define __asm__() 

#undef __attribute__
#define __attribute__() 

#undef exit
#define exit(exitCode)

#undef free
#define free(v)

#undef calloc
#define calloc(arraySize, elementSize) 0

#undef malloc
#define malloc(size) 0

#undef realloc
#define realloc(oldPointer, newSize) 0

#undef printf
#define printf(formatString, ...) 

#undef snprintf
#define snprintf(buffer, length, formatString, ...)

#undef time
#define time(value) 0

#undef setbuf
#define setbuf(source, value)

#undef memcpy
#define memcpy(destination, source, length)

#undef memset
#define memset(destination, value, length)

#undef memcmp
#define memcmp(valueA, valueB, length) 0

#undef strlen
#define strlen(string) 0

#undef strdup
#define strdup(string) NULL

#undef strcmp
#define strcmp(stringA, stringB) 0

#undef __extension__
#define __extension__

#undef _EXFNPTR
#define _EXFNPTR(funcName, params) (funcName)params

#undef _EXFNPTR_NOTHROW
#define _EXFNPTR_NOTHROW(funcName, params) (funcName)params

#undef __builtin_offsetof
#define __builtin_offsetof(type, member) 0

#undef offsetof
#define offsetof(type, member)  __builtin_offsetof(type, member)

#endif