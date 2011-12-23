#pragma once


typedef enum LogFlags
{
    // Please note, these are actual
    // tabs after the equals. Please
    // keep them that way.
    LogFlags_NoLog =					0x00,
    LogFlags_ILReading =				1 << 0,
    LogFlags_IREmitting =				1 << 1,
    LogFlags_SyntheticStack =			1 << 2,
    LogFlags_MetaData_Loading =			1 << 3,
    LogFlags_AppDomain_Loading =		1 << 4,
    LogFlags_ILReading_BranchLinker =	1 << 5,

} LogFlags;

void Log_WriteLine(LogFlags logCondition, char* fmt, ...);