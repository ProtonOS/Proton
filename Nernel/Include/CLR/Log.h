#pragma once


typedef enum LogFlags
{
    LogFlags_NoLog =            0x00,
    LogFlags_ILReading =        1 << 0,
    LogFlags_IREmitting =       1 << 1,
    LogFlags_MetaDataLoading =  1 << 2,

} LogFlags;

void Log_Initialize(LogFlags logFlags);

void Log_WriteLine(LogFlags logCondition, char* fmt, ...);