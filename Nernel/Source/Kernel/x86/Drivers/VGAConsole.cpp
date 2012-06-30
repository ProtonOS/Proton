#include "SerialPortLogger.h"
#include "VGAConsole.h"
#include "../PortIO.h"

VGAConsole::VGAConsole()
	: Console()
{
	mBaseMemory = (uint8_t*)0x000B8000;
	mColumns = 80;
	mRows = 25;
	mCursorColumn = 0;
	mCursorRow = 0;

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
	Clear(Color::LightWhite, Color::DarkBlack);
}

uint8_t* VGAConsole::GetCursor() { return mBaseMemory + (((mCursorRow * mColumns) + mCursorColumn) * 2); }

void VGAConsole::MoveTo(uint8_t pColumn, uint8_t pRow)
{
    mCursorColumn = pColumn % mColumns;
    mCursorRow = pRow % mRows;
}

void VGAConsole::MoveToTopLeft() { MoveTo(0, 0); }

void VGAConsole::MoveToNextLine()
{
	if (mCursorRow + 1 >= mRows)
	{
		uint32_t lengthToCopyUp = ((mRows - 1) * mColumns) * 2;
		memcpy((char*)mBaseMemory, ((char*)mBaseMemory) + (mColumns * 2), lengthToCopyUp);
		for (uint32_t index = 0; index < mColumns; ++index)
		{
			*(mBaseMemory + (((mRows - 1) * mColumns) * 2) + (index * 2)) = ' ';
		}
		mCursorRow = mRows - 1;
	}
	else ++mCursorRow;
	MoveTo(0, mCursorRow);
}

void VGAConsole::Advance()
{
	++mCursorColumn;
	if (mCursorColumn >= mColumns) MoveToNextLine();
}

void VGAConsole::WriteCharacter(char pCharacter)
{
    if (pCharacter == '\n')
    {
        MoveToNextLine();
    	SerialPortLogger::Instance->WriteByte('\r');
    	SerialPortLogger::Instance->WriteByte('\n');
    }
    else if (pCharacter != '\r')
    {
	    uint8_t* cursor = GetCursor();
	    *cursor = pCharacter;
	    *(cursor + 1) = mAttributes;
	    Advance();
    	SerialPortLogger::Instance->WriteByte(pCharacter);
    }
}

void VGAConsole::WriteString(const char* pString, uint32_t pLength)
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


void VGAConsole::Clear(Color pForeground, Color pBackground)
{
	mAttributes = CreateAttributes(pForeground, pBackground);
	MoveToTopLeft();

	int32_t index = 0;
	int32_t count = mColumns * mRows;
	uint8_t* cursor = GetCursor();
	while (index < count)
	{
		*cursor = ' ';
		*(cursor + 1) = mAttributes;
		cursor += 2;
		++index;
	}
}

void VGAConsole::SetColors(Color pForeground, Color pBackground)
{
	mAttributes = CreateAttributes(pForeground, pBackground);
}

Console::Color VGAConsole::GetForegroundColor()
{
	return (Color)(mAttributes & 0x0F);
}

Console::Color VGAConsole::GetBackgroundColor()
{
	return (Color)((mAttributes >> 4) & 0x0F);
}

void VGAConsole::WriteLine(const char* pLine)
{
	WriteString(pLine, 0);
	WriteString("\n", 0);
}
