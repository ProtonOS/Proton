#include <CLR/Log.h>
#include <stdio.h>
#include <stdarg.h>

// To disable logging, all
// you have to do is uncomment
// this line.
//#define NO_LOG

#ifdef NO_LOG
const LogFlags global_lgFlags = LogFlags_NoLog;
#undef NO_LOG
#else
const LogFlags global_lgFlags = (LogFlags)(
        LogFlags_ILReading
        | LogFlags_IREmitting
        //| LogFlags_SyntheticStack
        //| LogFlags_MetaData_Loading
        //| LogFlags_AppDomain_Loading
        | LogFlags_ILReading_BranchLinker
        );
#endif



void Log_WriteLine(LogFlags logCondition, char* fmt, ...)
{
    if (global_lgFlags & logCondition)
    {
        va_list args;
        va_start( args, fmt );
        vprintf( fmt, args );
        va_end( args );
        printf("\n");
    }
}