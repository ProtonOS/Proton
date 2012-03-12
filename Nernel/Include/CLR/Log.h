#pragma once

// To disable logging, all
// you have to do is uncomment
// this line.
//#define NO_LOG


#ifdef NO_LOG
#define ActiveLogFlags (LogFlags_NoLog)
#undef NO_LOG
#else
#define ActiveLogFlags ((LogFlags)( \
		LogFlags_JIT \
        | LogFlags_ILReading \
        | LogFlags_IREmitting \
        /* | LogFlags_SyntheticStack */ \
        /* | LogFlags_MetaData_Loading */ \
        /* | LogFlags_AppDomain_Loading  */ \
        /* | LogFlags_ILReading_BranchLinker */ \
        /* | LogFlags_ILReading_ElementTypes */ \
		/* | LogFlags_ILReading_MethodLayout */ \
		/* | LogFlags_ILReading_ExceptionBlocks */ \
		/* | LogFlags_ILReading_FieldLayout */ \
        ))
#endif


typedef enum LogFlags
{
    // Please note, these are actual
    // tabs after the equals. Please
    // keep them that way.
    LogFlags_NoLog =						0x00,
    LogFlags_ILReading =					1 << 0,
    LogFlags_IREmitting =					1 << 1,
    LogFlags_SyntheticStack =				1 << 2,
    LogFlags_MetaData_Loading =				1 << 3,
    LogFlags_AppDomain_Loading =			1 << 4,
    LogFlags_ILReading_BranchLinker =		1 << 5,
    LogFlags_ILReading_ElementTypes =		1 << 6,
    LogFlags_ILReading_MethodLayout =		1 << 7,
    LogFlags_ILReading_ExceptionBlocks =	1 << 8,
    LogFlags_JIT =							1 << 9,
    LogFlags_ILReading_FieldLayout =		1 << 10,

} LogFlags;

void Log_WriteLine(LogFlags logCondition, char* fmt, ...);