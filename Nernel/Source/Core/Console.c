#include <stdio.h>
#include <Core/Console.h>
#include <Core/SerialWriter.h>

uint8_t* gConsole_BaseMemory = (uint8_t*)Console_BaseMemory;
uint8_t gConsole_Columns = Console_DefaultColumns;
uint8_t gConsole_Rows = Console_DefaultRows;
uint8_t gConsole_CursorColumn = 0;
uint8_t gConsole_CursorRow = 0;
uint8_t gConsole_Attributes = Console_DefaultAttributes;

void Console_Startup()
{
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

	Console_Clear(Console_CreateAttributes(Console_LightWhite, Console_DarkBlack));
}

void Console_Shutdown()
{
}

uint8_t Console_CreateAttributes(uint8_t pForeground, uint8_t pBackground) { return pForeground | (pBackground << 4); }

void Console_MoveTo(uint8_t pColumn, uint8_t pRow)
{
    gConsole_CursorColumn = pColumn % gConsole_Columns;
    gConsole_CursorRow = pRow % gConsole_Rows;
}

void Console_MoveToTopLeft() { Console_MoveTo(0, 0); }

void Console_MoveToNextLine() { Console_MoveTo(0, gConsole_CursorRow + 1); }

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
    	SerialWriter_WriteByte('\r');
    	SerialWriter_WriteByte('\n');
    }
    else
    {
	    uint8_t* cursor = Console_GetCursor();
	    *cursor = pCharacter;
	    *(cursor + 1) = gConsole_Attributes;
	    Console_Advance();
    	SerialWriter_WriteByte(pCharacter);
    }
}

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

void Console_WriteLine(const char* pString)
{
	Console_WriteString(pString, 0);
	if (gConsole_CursorColumn > 0) Console_MoveToNextLine();
    SerialWriter_WriteByte('\r');
    SerialWriter_WriteByte('\n');
}

uint8_t* Console_GetCursor() { return gConsole_BaseMemory + (((gConsole_CursorRow * gConsole_Columns) + gConsole_CursorColumn) * 2); }

void Console_Advance()
{
	++gConsole_CursorColumn;
	if (gConsole_CursorColumn >= gConsole_Columns)
	{
		Console_MoveToNextLine();
		if (gConsole_CursorRow >= gConsole_Rows) gConsole_CursorRow = 0;
	}
}
