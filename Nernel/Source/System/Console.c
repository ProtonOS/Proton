// This define causes the console to
// startup in it's own thread and write
// out entire messages rather than being
// interrupted in the middle of doing so.
//#define AsyncConsole

#include <Common.h>
#include <System/Console.h>
#include <System/ConsoleLogger.h>
#include <System/x86/PortIO.h>
#include <System/Atomics.h>

#define CONSOLE__BaseMemory          0x000B8000
#define CONSOLE__DefaultColumns      80
#define CONSOLE__DefaultRows         25
#define CONSOLE__DefaultAttributes   0x0F

uint8_t* gConsole_BaseMemory = (uint8_t*)CONSOLE__BaseMemory;
uint8_t gConsole_Columns = CONSOLE__DefaultColumns;
uint8_t gConsole_Rows = CONSOLE__DefaultRows;
uint8_t gConsole_CursorColumn = 0;
uint8_t gConsole_CursorRow = 0;
uint8_t gConsole_Attributes = CONSOLE__DefaultAttributes;

void Console_Startup()
{
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
	Console_Clear(Console_CreateAttributes(CONSOLE__LightWhite, CONSOLE__DarkBlack));
}

void Console_Shutdown()
{
}

void Console_MoveTo(uint8_t pColumn, uint8_t pRow)
{
    gConsole_CursorColumn = pColumn % gConsole_Columns;
    gConsole_CursorRow = pRow % gConsole_Rows;
}

void Console_MoveToTopLeft() { Console_MoveTo(0, 0); }

void Console_MoveToNextLine()
{
	if (gConsole_CursorRow + 1 >= gConsole_Rows)
	{
		uint32_t lengthToCopyUp = ((gConsole_Rows - 1) * gConsole_Columns) * 2;
		memcpy((char*)gConsole_BaseMemory, ((char*)gConsole_BaseMemory) + (gConsole_Columns * 2), lengthToCopyUp);
		for (uint32_t index = 0; index < gConsole_Columns; ++index)
		{
			*(gConsole_BaseMemory + (((gConsole_Rows - 1) * gConsole_Columns) * 2) + (index * 2)) = ' ';
		}
		gConsole_CursorRow = gConsole_Rows - 1;
	}
	else ++gConsole_CursorRow;
	Console_MoveTo(0, gConsole_CursorRow);
}

void Console_Clear(uint8_t pAttributes)
{
	gConsole_Attributes = pAttributes;
	Console_MoveToTopLeft();

	int32_t index = 0;
	int32_t count = gConsole_Columns * gConsole_Rows;
	uint8_t* cursor = Console_GetCursor();
	while (index < count)
	{
		*cursor = ' ';
		*(cursor + 1) = gConsole_Attributes;
		cursor += 2;
		++index;
	}
}

void Console_WriteCharacter(char pCharacter)
{
    if (pCharacter == '\n')
    {
        Console_MoveToNextLine();
    	ConsoleLogger_WriteByte('\r');
    	ConsoleLogger_WriteByte('\n');
    }
    else if (pCharacter != '\r')
    {
	    uint8_t* cursor = Console_GetCursor();
	    *cursor = pCharacter;
	    *(cursor + 1) = gConsole_Attributes;
	    Console_Advance();
    	ConsoleLogger_WriteByte(pCharacter);
    }
}

#ifdef AsyncConsole
#include <System/ThreadScheduler.h>
uint8_t gConsoleQueueTopLock = 0;
uint8_t gConsoleQueueBottomLock = 0;
uint8_t gConsole_Started = 0;
void Console_AsyncEntryPoint();
typedef struct _ConsoleWriteEntry ConsoleWriteEntry;
struct _ConsoleWriteEntry
{
	char* strToWrite;
	uint32_t strLength;
	ConsoleWriteEntry* NextEntry;
};
ConsoleWriteEntry* TopEntry = NULL;
ConsoleWriteEntry* BottomEntry = NULL;

void Console_WriteString(const char* pString, uint32_t pLength)
{
	if (!Atomic_CompareExchange(&gConsole_Started, 0, 1))
	{
		Process_PriorityCreate((size_t)Console_AsyncEntryPoint, 0x100000, 4);
	}
	if (!pLength)
		pLength = strlen(pString);
	Atomic_AquireLock(&gConsoleQueueTopLock);
	if (!TopEntry)
	{
		Atomic_AquireLock(&gConsoleQueueBottomLock)
		TopEntry = (ConsoleWriteEntry*)calloc(1, sizeof(ConsoleWriteEntry));
		TopEntry->strToWrite = calloc(1, pLength);
		memcpy(TopEntry->strToWrite, pString, pLength);
		TopEntry->strLength = pLength;
		BottomEntry = TopEntry;
		Atomic_ReleaseLock(&gConsoleQueueBottomLock);
		Atomic_ReleaseLock(&gConsoleQueueTopLock);
	}
	else
	{
		Atomic_ReleaseLock(&gConsoleQueueTopLock);
		Atomic_AquireLock(&gConsoleQueueBottomLock);
		ConsoleWriteEntry* nEtry = (ConsoleWriteEntry*)calloc(1, sizeof(ConsoleWriteEntry));
		nEtry->strToWrite = calloc(1, pLength);
		memcpy(nEtry->strToWrite, pString, pLength);
		nEtry->strLength = pLength;
		BottomEntry->NextEntry = nEtry;
		BottomEntry = nEtry;
		Atomic_ReleaseLock(&gConsoleQueueBottomLock);
	}
}

void Console_AsyncEntryPoint()
{
	while(TRUE)
	{
		Atomic_AquireLock(&gConsoleQueueTopLock);
		if (TopEntry)
		{
			Atomic_ReleaseLock(&gConsoleQueueTopLock);

			char* iterator = TopEntry->strToWrite;
			bool_t useLength = TopEntry->strLength > 0;
			while (*iterator)
			{
				Console_WriteCharacter(*iterator);
				++iterator;
				if (useLength)
				{
					--TopEntry->strLength;
					if (TopEntry->strLength == 0) break;
				}
			}
			Atomic_AquireLock(&gConsoleQueueTopLock);
			ConsoleWriteEntry* nEtry = TopEntry;
			TopEntry = TopEntry->NextEntry;
			Atomic_ReleaseLock(&gConsoleQueueTopLock);
			free(nEtry->strToWrite);
			free(nEtry);
		}
		else
		{
			Atomic_ReleaseLock(&gConsoleQueueTopLock);
			IOWAIT();
			IOWAIT();
			IOWAIT();
			IOWAIT();
			IOWAIT();
			IOWAIT();
			IOWAIT();
		}
	}
}
#else
void Console_WriteString(const char* pString,
                                uint32_t pLength)
{
    const char* iterator = pString;
    bool_t useLength = pLength > 0;
    while (*iterator)
	{
        Console_WriteCharacter(*iterator);
        ++iterator;
        if (useLength)
        {
            --pLength;
            if (pLength == 0) break;
        }
    }
}
#endif

void Console_WriteLine(const char* pString)
{
	Console_WriteString(pString, 0);
	Console_WriteString("\n", 0);
}

uint8_t* Console_GetCursor() { return gConsole_BaseMemory + (((gConsole_CursorRow * gConsole_Columns) + gConsole_CursorColumn) * 2); }

void Console_Advance()
{
	++gConsole_CursorColumn;
	if (gConsole_CursorColumn >= gConsole_Columns)
	{
		Console_MoveToNextLine();
	}
}
