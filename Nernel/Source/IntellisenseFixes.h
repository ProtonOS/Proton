#pragma once

#ifdef _WIN32

#undef free
#define free(v)

#undef calloc
#define calloc(arraySize, elementSize) 0

#undef malloc
#define malloc(size) 0

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

#endif