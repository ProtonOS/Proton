#include "Common.h"
#include "Log.h"

#include <stdarg.h>

LogLevel gLog_Level = LOGLEVEL_INFORMATION | LOGLEVEL_MEMORY;

void Log_Startup()
{
}

void Log_Shutdown()
{
}

void Log_WriteLine(LogLevel pLogLevel, const char* pFormat, ...)
{
	if ((gLog_Level & pLogLevel) != 0)
	{
		va_list args;
		va_start( args, pFormat );
		vprintf( pFormat, args );
		va_end( args );
		printf("\n");
	}
}
