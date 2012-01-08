#include <CLR/Log.h>
#include <stdio.h>
#include <stdarg.h>


void Log_WriteLine(LogFlags logCondition, char* fmt, ...)
{
    if (ActiveLogFlags & logCondition)
    {
        va_list args;
        va_start( args, fmt );
        vprintf( fmt, args );
        va_end( args );
        printf("\n");
    }
}