#pragma once

#include <Core/Device.h>
#include <Core/Driver/ConsoleColor.h>

namespace Core
{
    namespace Driver
    {
        class Console : public Device
        {
        public:
            Console();
            bool OnRegister();
            void OnUnregister();

            static uint8_t CreateAttributes(ConsoleColor pForeground, ConsoleColor pBackground);
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
}
