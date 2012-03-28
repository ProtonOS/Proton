#pragma once

#define CONSOLE__DarkBlack           							0x00
#define CONSOLE__DarkBlue            							0x01
#define CONSOLE__DarkGreen           							0x02
#define CONSOLE__DarkCyan            							0x03
#define CONSOLE__DarkRed             							0x04
#define CONSOLE__DarkMagenta         							0x05
#define CONSOLE__DarkYellow          							0x06
#define CONSOLE__DarkWhite           							0x07
#define CONSOLE__LightBlack          							0x08
#define CONSOLE__LightBlue           							0x09
#define CONSOLE__LightGreen          							0x0A
#define CONSOLE__LightCyan           							0x0B
#define CONSOLE__LightRed            							0x0C
#define CONSOLE__LightMagenta        							0x0D
#define CONSOLE__LightYellow									0x0E
#define CONSOLE__LightWhite										0x0F

#define Console_CreateAttributes(pForeground, pBackground)		(pForeground | (pBackground << 4))

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
