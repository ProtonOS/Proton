#pragma once
#include <stdio.h>

//#define LOG__DISABLE

typedef enum LogLevel
{
    LOGLEVEL__None =								0 << 0,
    LOGLEVEL__Information =							1 << 0,
    LOGLEVEL__Memory =								1 << 1,
	LOGLEVEL__Processor =							1 << 2,
	LOGLEVEL__SyntheticStack =						1 << 3,
	LOGLEVEL__MethodLayout =						1 << 4,
	LOGLEVEL__FieldLayout =							1 << 5,
	LOGLEVEL__Exceptions =							1 << 6,
	LOGLEVEL__ILReader =							1 << 7,
	LOGLEVEL__IREmitter =							1 << 8,
	LOGLEVEL__Link_Internals =						1 << 9,
	LOGLEVEL__Link_Branches =						1 << 10,
	LOGLEVEL__Optimize =							1 << 11,
	LOGLEVEL__Optimize_SSA =						1 << 12,
} LogLevel;

#ifdef LOG__DISABLE
#define LOGLEVEL__Current					(LOGLEVEL__None)
#define Log_WriteLine(logCondition, ...)
#elif _WIN32
#define LOGLEVEL__Current					(LOGLEVEL__None)
void Log_WriteLine(LogLevel pLevel, const char* pFormatString, ...);
#else
#define LOGLEVEL__Current ((LogLevel)( \
		LOGLEVEL__Information \
		/* \
		| LOGLEVEL__Memory \
		| LOGLEVEL__Processor \
		| LOGLEVEL__SyntheticStack \
		| LOGLEVEL__Decomposition \
		| LOGLEVEL__MethodLayout \
		| LOGLEVEL__FieldLayout \
		| LOGLEVEL__Exceptions \
		| LOGLEVEL__ILReader \
		| LOGLEVEL__IREmitter \
		| LOGLEVEL__Link_Internals \
		| LOGLEVEL__Link_Branches \
		*/ \
		| LOGLEVEL__Optimize \
		| LOGLEVEL__Optimize_SSA \
        ))

#define Log_WriteLine(ll, ...) \
	if (LOGLEVEL__Current & ll) \
    { \
        printf(__VA_ARGS__); \
        printf("\n"); \
    }
#endif

void Log_Startup();
void Log_Shutdown();

