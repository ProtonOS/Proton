#pragma once

#include <Core/Device.h>

namespace Core
{
    class Console : public Device
    {
    public:
        enum Color : uint8_t
        {
            DarkBlack = 0x00,
            DarkBlue = 0x01,
            DarkGreen = 0x02,
            DarkCyan = 0x03,
            DarkRed = 0x04,
            DarkMagenta = 0x05,
            DarkYellow = 0x06,
            DarkWhite = 0x07,
            LightBlack = 0x08,
            LightBlue = 0x09,
            LightGreen = 0x0A,
            LightCyan = 0x0B,
            LightRed = 0x0C,
            LightMagenta = 0x0D,
            LightYellow = 0x0E,
            LightWhite = 0x0F
        };

        Console();
        bool Initialize();
        void Cleanup();

        static uint8_t CreateAttributes(Color pForeground, Color pBackground);
        void MoveTo(uint8_t pColumn, uint8_t pRow);
        void MoveToTopLeft();
        void MoveToNextLine();
        void Clear(uint8_t pAttributes);
        void WriteCharacter(char pCharacter);
        void WriteString(const char* pString, uint32_t pLength);
        void WriteLine(const char* pLine);

    private:
        static const uint32_t BaseMemory = 0x000B8000;
        static const uint8_t DefaultColumns = 80;
        static const uint8_t DefaultRows = 25;
        static const uint8_t DefaultAttributes = 0x0F;

        uint8_t* mBaseMemory;
        uint8_t mColumns;
        uint8_t mRows;
        uint8_t mCursorColumn;
        uint8_t mCursorRow;
        uint8_t mAttributes;

        Console(const Console&);
        uint8_t* GetCursor();
        void Advance();
    };
}
