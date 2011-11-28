extern "C" {
#include <stdio.h>
}
#include <Core/DeviceManager.h>

using namespace Core;

Console::Console()
    : mBaseMemory(reinterpret_cast<uint8_t*>(BaseMemory)),
      mColumns(DefaultColumns),
      mRows(DefaultRows),
      mCursorColumn(0),
      mCursorRow(0),
      mAttributes(DefaultAttributes)
{
}

bool Console::Initialize()
{
    uint32_t memoryRequired = mColumns * mRows * 2;
    if (!IsMemoryAvailable(BaseMemory, memoryRequired)) return false;

    ClaimMemory(BaseMemory, memoryRequired);

    return true;
}

void Console::Cleanup()
{
    ReleaseMemory(reinterpret_cast<uint32_t>(mBaseMemory));
}

uint8_t Console::CreateAttributes(Color pForeground, Color pBackground) { return pForeground | (pBackground << 4); }

void Console::MoveTo(uint8_t pColumn, uint8_t pRow)
{
    mCursorColumn = pColumn % mColumns;
    mCursorRow = pRow % mRows;
}

void Console::MoveToTopLeft() { MoveTo(0, 0); }

void Console::MoveToNextLine() { MoveTo(0, mCursorRow + 1); }

void Console::Clear(uint8_t pAttributes)
{
	mAttributes = pAttributes;
	MoveToTopLeft();

	int32_t index = 0;
	int32_t count = mColumns * mRows;
	uint8_t * cursor = GetCursor();
	while (index < count)
	{
		*cursor = ' ';
		*(cursor + 1) = mAttributes;
		cursor += 2;
		++index;
	}
}

void Console::WriteCharacter(char pCharacter)
{
    if (pCharacter == '\n')
    {
        MoveToNextLine();
    	DeviceManager::COMPortLoggersWriteByte('\r');
    	DeviceManager::COMPortLoggersWriteByte('\n');
    }
    else
    {
	    uint8_t* cursor = GetCursor();
	    *cursor = pCharacter;
	    *(cursor + 1) = mAttributes;
	    Advance();
    	DeviceManager::COMPortLoggersWriteByte(pCharacter);
    }
}

void Console::WriteString(const char* pString,
                          uint32_t pLength)
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

void Console::WriteLine(const char* pString)
{
	WriteString(pString, 0);
	if (mCursorColumn > 0) MoveToNextLine();
    DeviceManager::COMPortLoggersWriteByte('\r');
    DeviceManager::COMPortLoggersWriteByte('\n');
}

uint8_t* Console::GetCursor() { return mBaseMemory + (((mCursorRow * mColumns) + mCursorColumn) * 2); }

void Console::Advance()
{
	++mCursorColumn;
	if (mCursorColumn >= mColumns)
	{
		MoveToNextLine();
		if (mCursorRow >= mRows) mCursorRow = 0;
	}
}
