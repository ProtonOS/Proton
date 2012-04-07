#pragma once
#include <stdio.h>

typedef enum LogLevel
{
    LOGLEVEL__None =						0x00,
    LOGLEVEL__Information =					1 << 0,
    LOGLEVEL__Memory =						1 << 1,
	LOGLEVEL__Processor =					1 << 2,
	LOGLEVEL__ILDecomposition =				1 << 3,
	LOGLEVEL__ILDecompositionLayouts =		1 << 4,
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
		| LOGLEVEL__ILDecompositionLayouts \
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

