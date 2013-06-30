#include "../../Core/Core.hpp"

#include "KernelConsole.hpp"

namespace KernelConsole
{
    constexpr Core::Ptr<UInt8> BaseMemory = reinterpret_cast<UInt8 *>(BaseMemoryAddress);
    UInt8 Columns = DefaultColumns;
    UInt8 Rows = DefaultRows;
    UInt8 CursorColumn = 0;
    UInt8 CursorRow = 0;
    UInt8 Attributes = DefaultAttributes;

    UInt8 CreateAttributes(UInt8 pForeground, UInt8 pBackground)
    {
        return (pForeground | (pBackground << 4));
    }

    void MoveCursorTo(UInt8 pColumn, UInt8 pRow)
    {
        CursorColumn = pColumn % Columns;
        CursorRow = pRow % Rows;
    }

    Core::Ptr<UInt8> GetCursor()
    {
        return BaseMemory + (((CursorRow * Columns) + CursorColumn) * 2);
    }

    void Load()
    {
        outb(0x3D4, 0x0A);
        outb(0x3D5, 0x20);
        Clear(CreateAttributes(LightWhite, DarkBlack));
    }

    void Clear(UInt8 pAttributes)
    {
        Attributes = pAttributes;
        MoveCursorToTopLeft();

        UInt32 index = 0;
        UInt32 count = Columns * Rows;
        Core::Ptr<UInt8> cursor = GetCursor();
        while (index < count) {
            *cursor = ' ';
            *(cursor + 1) = Attributes;
            cursor += 2;
            ++index;
        }
    }

    void MoveCursorToTopLeft()
    {
        MoveCursorTo(0, 0);
    }

    void MoveCursorToNextLine()
    {
        if (CursorRow + 1 >= Rows) {
            UInt32 lengthToCopyUp = ((Rows - 1) * Columns) * 2;
            Core::Memory::ByteCopy(BaseMemory, BaseMemory + (Columns * 2), lengthToCopyUp);
            for (UInt32 index = 0; index < Columns; ++index)
                *(BaseMemory + (((Rows - 1) * Columns) * 2) + (index * 2)) = ' ';
            CursorRow = Rows - 1;
        } else ++CursorRow;
        MoveCursorTo(0, CursorRow);
    }

    void Advance()
    {
        ++CursorColumn;
        if (CursorColumn >= Columns)
            MoveCursorToNextLine();
    }

    void WriteCharacter(char pCharacter)
    {
        if (pCharacter == '\n') {
            MoveCursorToNextLine();
            //ConsoleLogger_WriteByte('\r');
            //ConsoleLogger_WriteByte('\n');
        } else if (pCharacter != '\r') {
            Core::Ptr<UInt8> cursor = GetCursor();
            *cursor = pCharacter;
            *(cursor + 1) = Attributes;
            Advance();
            //ConsoleLogger_WriteByte(pCharacter);
        }
    }

    void WriteString(const char* pString, UInt32 pLength)
    {
        const char* iterator = pString;
        bool useLength = pLength > 0;
        while (*iterator) {
            WriteCharacter(*iterator);
            ++iterator;
            if (useLength) {
                --pLength;
                if (pLength == 0) break;
            }
        }
    }

    void WriteLine(const char* pString)
    {
        WriteString(pString, 0);
        WriteCharacter('\n');
    }
}


