#pragma once
#include <stdio.h>

#define LOG__DISABLE

typedef enum LogLevel
{
    LOGLEVEL__None =							0x00,
    LOGLEVEL__Information =						1 << 0,
    LOGLEVEL__Memory =							1 << 1,
	LOGLEVEL__Processor =						1 << 2,
	LOGLEVEL__ILDecomposition =					1 << 3,
	LOGLEVEL__ILDecomposition_MethodLayout =	1 << 4,
	LOGLEVEL__ILDecomposition_FieldLayout =		1 << 5,

} LogLevel;

#ifdef LOG__DISABLE
#define LOGLEVEL__Current					(LOGLEVEL__None)
#define Log_WriteLine(logCondition, ...)
#else
#define LOGLEVEL__Current ((LogLevel)( \
		LOGLEVEL__Information \
		| LOGLEVEL__Memory \
		| LOGLEVEL__Processor \
		| LOGLEVEL__ILDecomposition \
		| LOGLEVEL__ILDecomposition_MethodLayout \
		| LOGLEVEL__ILDecomposition_FieldLayout \
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

