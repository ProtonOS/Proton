#include <String_Format.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

char* String_Format(char* format, ...)
{
    char* buf = calloc(1024, sizeof(char));
    va_list args;
    va_start( args, format );
    vsnprintf( buf, 1024, format, args );
    va_end( args );
    return buf;
}