#pragma once

typedef  enum _LogLevel LogLevel;

enum _LogLevel
{
	LOGLEVEL_INFORMATION = (1 << 0),
	LOGLEVEL_MEMORY = (1 << 1),
};

void Log_Startup();
void Log_Shutdown();

void Log_WriteLine(LogLevel pLogLevel, const char* pFormat, ...);
