#include "KernelConsole.h"

uint8* KernelConsole::BaseMemory = (uint8*)KernelConsole::BaseMemoryAddress;
uint8 KernelConsole::Columns = KernelConsole::DefaultColumns;
uint8 KernelConsole::Rows = KernelConsole::DefaultRows;
uint8 KernelConsole::CursorColumn = 0;
uint8 KernelConsole::CursorRow = 0;
uint8 KernelConsole::Attributes = KernelConsole::DefaultAttributes;

void KernelConsole::Load()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
	Clear(CreateAttributes(LightWhite, DarkBlack));
}

void KernelConsole::Clear(uint8 pAttributes)
{
	Attributes = pAttributes;
	MoveCursorToTopLeft();

	uint32 index = 0;
	uint32 count = Columns * Rows;
	uint8* cursor = GetCursor();
	while (index < count)
	{
		*cursor = ' ';
		*(cursor + 1) = Attributes;
		cursor += 2;
		++index;
	}
}

void tmpmemcpy(uint8* dest, uint8* src, uintptr len)
{
	for (uintptr index = 0; index < len; ++index)
		dest[index] = src[index];
}

void KernelConsole::MoveCursorToNextLine()
{
	if (CursorRow + 1 >= Rows)
	{
		uint32 lengthToCopyUp = ((Rows - 1) * Columns) * 2;
		tmpmemcpy(BaseMemory, BaseMemory + (Columns * 2), lengthToCopyUp);
		for (uint32 index = 0; index < Columns; ++index)
			*(BaseMemory + (((Rows - 1) * Columns) * 2) + (index * 2)) = ' ';
		CursorRow = Rows - 1;
	}
	else ++CursorRow;
	MoveCursorTo(0, CursorRow);
}

void KernelConsole::Advance()
{
	++CursorColumn;
	if (CursorColumn >= Columns)
		MoveCursorToNextLine();
}

void KernelConsole::WriteCharacter(char pCharacter)
{
    if (pCharacter == '\n')
    {
        MoveCursorToNextLine();
    	//ConsoleLogger_WriteByte('\r');
    	//ConsoleLogger_WriteByte('\n');
    }
    else if (pCharacter != '\r')
    {
	    uint8* cursor = GetCursor();
	    *cursor = pCharacter;
	    *(cursor + 1) = Attributes;
	    Advance();
    	//ConsoleLogger_WriteByte(pCharacter);
    }
}

void KernelConsole::WriteString(const char* pString, uint32 pLength)
{
    const char* iterator = pString;
    bool useLength = pLength > 0;
    while (*iterator)
	{
        WriteCharacter(*iterator);
        ++iterator;
        if (useLength)
        {
            --pLength;
            if (pLength == 0) break;
        }
    }
}

void KernelConsole::WriteLine(const char* pString)
{
	WriteString(pString, 0);
	WriteCharacter('\n');
}
