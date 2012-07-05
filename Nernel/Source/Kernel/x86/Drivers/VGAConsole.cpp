#include "SerialPortLogger.h"
#include "../../Console.h"
#include "../PortIO.h"

namespace Console
{
    struct Char {
        uint8_t ch;
        uint8_t at;
    };
	Char* mBaseMemory;
	uint8_t mColumns;
	uint8_t mRows;
	uint8_t mCursorColumn;
	uint8_t mCursorRow;
	uint8_t mAttributes;

	uint8_t CreateAttributes(Color pForeground, Color pBackground);
	Char* GetCursor();
	void MoveTo(uint8_t pColumn, uint8_t pRow);
	void MoveToTopLeft();
	void MoveToNextLine();
	void Advance();
	void WriteCharacter(char pCharacter);
	void WriteString(const char* pString, uint32_t pLength);

    void Startup()
    {
	    mBaseMemory = reinterpret_cast<Char*>(0x000B8000);
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
    uint8_t CreateAttributes(Color pForeground, Color pBackground)
    {
        return static_cast<uint8_t>(pForeground | (pBackground << 4));
    }
    Char* GetCursor()
    {
        return mBaseMemory + (mCursorRow * mColumns + mCursorColumn);
    }
    void MoveTo(uint8_t pColumn, uint8_t pRow)
    {
        mCursorColumn = pColumn % mColumns;
        mCursorRow = pRow % mRows;
    }
    void MoveToTopLeft()
    {
        MoveTo(0, 0);
    }
    void MoveToNextLine()
    {
	    if (mCursorRow + 1 >= mRows)
	    {
            std::memcpy(mBaseMemory, mBaseMemory + mColumns, (mRows - 1) * mColumns);
            //std::copy(mBaseMemory + mColumns, mBaseMemory + mRows * mColumns, mBaseMemory);
		    for (uint32_t index = 0; index < mColumns; ++index)
		    {
			    mBaseMemory[(mRows - 1) * mColumns + index].ch = ' ';
		    }
		    mCursorRow = mRows - 1;
	    }
	    else
        {
            ++mCursorRow;
        }
	    MoveTo(0, mCursorRow);
    }
    void Advance()
    {
	    ++mCursorColumn;
	    if (mCursorColumn >= mColumns)
        {
            MoveToNextLine();
        }
    }

    void WriteCharacter(char pCharacter)
    {
        if (pCharacter == '\n')
        {
            MoveToNextLine();
    	    SerialPortLogger::WriteByte('\r');
    	    SerialPortLogger::WriteByte('\n');
        }
        else if (pCharacter != '\r')
        {
	        Char* cursor = GetCursor();
	        cursor->ch = pCharacter;
	        cursor->at = mAttributes;
	        Advance();
    	    SerialPortLogger::WriteByte(pCharacter);
        }
    }
    void WriteString(const char* pString, uint32_t pLength)
    {
        while (*pString && pLength)
	    {
            WriteCharacter(*pString);
            ++pString;
            --pLength;
        }
    }
    void WriteString(const char* pString)
    {
        while (*pString)
	    {
            WriteCharacter(*pString);
            ++pString;
        }
    }
    void Clear(Color pForeground, Color pBackground)
    {
	    mAttributes = CreateAttributes(pForeground, pBackground);
	    MoveToTopLeft();

	    int32_t index = 0;
	    int32_t count = mColumns * mRows;
	    Char* cursor = GetCursor();
	    while (index < count)
	    {
		    cursor->ch = ' ';
		    cursor->at = mAttributes;
		    ++cursor;
		    ++index;
	    }
    }
    void SetColors(Color pForeground, Color pBackground)
    {
	    mAttributes = CreateAttributes(pForeground, pBackground);
    }
    Console::Color GetForegroundColor()
    {
	    return static_cast<Color>(mAttributes & 0x0F);
    }
    Console::Color GetBackgroundColor()
    {
	    return static_cast<Color>((mAttributes >> 4) & 0x0F);
    }
    void WriteLine(const char* pLine)
    {
	    WriteString(pLine, 0);
	    WriteString("\n", 0);
    }
}
