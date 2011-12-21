#include <CLR/Log.h>
#include <stdio.h>


static LogFlags lgFlags = LogFlags_NoLog;

void Log_Initialize(LogFlags flags)
{
    lgFlags = flags;
}

void Log_WriteLine(LogFlags logCondition, const char* msg)
{
    if (lgFlags & logCondition)
    {
        printf(msg);
        printf("\n");
    }
}