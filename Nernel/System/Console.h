#pragma once

#define CONSOLE_DARKBLACK           0x00
#define CONSOLE_DARKBLUE            0x01
#define CONSOLE_DARKGREEN           0x02
#define CONSOLE_DARKCYAN            0x03
#define CONSOLE_DARKRED             0x04
#define CONSOLE_DARKMAGENTA         0x05
#define CONSOLE_DARKYELLOW          0x06
#define CONSOLE_DARKWHITE           0x07
#define CONSOLE_LIGHTBLACK          0x08
#define CONSOLE_LIGHTBLUE           0x09
#define CONSOLE_LIGHTGREEN          0x0A
#define CONSOLE_LIGHTCYAN           0x0B
#define CONSOLE_LIGHTRED            0x0C
#define CONSOLE_LIGHTMAGENTA        0x0D
#define CONSOLE_LIGHTYELLOW         0x0E
#define CONSOLE_LIGHTWHITE          0x0F

#define CONSOLE_CREATEATTRIBUTES(pForeground, pBackground)		(pForeground | (pBackground << 4))

void Console_Startup();
void Console_Shutdown();

uint8_t* Console_GetCursor();
void Console_Advance();
void Console_MoveTo(uint8_t pColumn, uint8_t pRow);
void Console_MoveToTopLeft();
void Console_MoveToNextLine();
void Console_Clear(uint8_t pAttributes);
void Console_WriteCharacter(char pCharacter);
void Console_WriteString(const char* pString, uint32_t pLength);
void Console_WriteLine(const char* pLine);
