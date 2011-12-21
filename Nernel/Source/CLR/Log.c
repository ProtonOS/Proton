#include <CLR/Log.h>
#include <stdio.h>
#include <stdarg.h>


static LogFlags lgFlags = LogFlags_NoLog;

void Log_Initialize(LogFlags flags)
{
    lgFlags = flags;
}

void Log_WriteLine(LogFlags logCondition, char* fmt, ...)
{
    if (lgFlags & logCondition)
    {
        char buf[1024];
        va_list args;
        va_start( args, fmt );
        vsnprintf( buf, 1024, fmt, args );
        va_end( args );
        printf(buf);
        printf("\n");
    }
}