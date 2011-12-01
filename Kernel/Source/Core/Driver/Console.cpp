extern "C" {
#include <stdio.h>
}
#include <Core/DeviceManager.h>

Core::Driver::Console::Console()
    : mBaseMemory(reinterpret_cast<uint8_t*>(BaseMemory)),
      mColumns(DefaultColumns),
      mRows(DefaultRows),
      mCursorColumn(0),
      mCursorRow(0),
      mAttributes(DefaultAttributes)
{
}

bool Core::Driver::Console::OnRegister()
{
    uint32_t memoryRequired = mColumns * mRows * 2;
    if (!IsMemoryAvailable(BaseMemory, memoryRequired)) return false;

    ClaimMemory(BaseMemory, memoryRequired);

    return true;
}

void Core::Driver::Console::OnUnregister()
{
    ReleaseMemory(reinterpret_cast<uint32_t>(mBaseMemory));
}

uint8_t Core::Driver::Console::CreateAttributes(ConsoleColor pForeground, ConsoleColor pBackground) { return pForeground | (pBackground << 4); }

void Core::Driver::Console::MoveTo(uint8_t pColumn, uint8_t pRow)
{
    mCursorColumn = pColumn % mColumns;
    mCursorRow = pRow % mRows;
}

void Core::Driver::Console::MoveToTopLeft() { MoveTo(0, 0); }

void Core::Driver::Console::MoveToNextLine() { MoveTo(0, mCursorRow + 1); }

void Core::Driver::Console::Clear(uint8_t pAttributes)
{
	mAttributes = pAttributes;
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

void Core::Driver::Console::WriteCharacter(char pCharacter)
{
    if (pCharacter == '\n')
    {
        MoveToNextLine();
    	Core::DeviceManager::GetCOMPortLogger().WriteByte('\r');
    	Core::DeviceManager::GetCOMPortLogger().WriteByte('\n');
    }
    else
    {
	    uint8_t* cursor = GetCursor();
	    *cursor = pCharacter;
	    *(cursor + 1) = mAttributes;
	    Advance();
    	Core::DeviceManager::GetCOMPortLogger().WriteByte(pCharacter);
    }
}

void Core::Driver::Console::WriteString(const char* pString,
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

void Core::Driver::Console::WriteLine(const char* pString)
{
	WriteString(pString, 0);
	if (mCursorColumn > 0) MoveToNextLine();
    Core::DeviceManager::GetCOMPortLogger().WriteByte('\r');
    Core::DeviceManager::GetCOMPortLogger().WriteByte('\n');
}

uint8_t* Core::Driver::Console::GetCursor() { return mBaseMemory + (((mCursorRow * mColumns) + mCursorColumn) * 2); }

void Core::Driver::Console::Advance()
{
	++mCursorColumn;
	if (mCursorColumn >= mColumns)
	{
		MoveToNextLine();
		if (mCursorRow >= mRows) mCursorRow = 0;
	}
}
