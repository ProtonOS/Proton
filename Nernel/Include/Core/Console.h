#pragma once

#include <types.h>

#define Console_BaseMemory          0x000B8000
#define Console_DefaultColumns      80
#define Console_DefaultRows         25
#define Console_DefaultAttributes   0x0F

#define Console_DarkBlack           0x00
#define Console_DarkBlue            0x01
#define Console_DarkGreen           0x02
#define Console_DarkCyan            0x03
#define Console_DarkRed             0x04
#define Console_DarkMagenta         0x05
#define Console_DarkYellow          0x06
#define Console_DarkWhite           0x07
#define Console_LightBlack          0x08
#define Console_LightBlue           0x09
#define Console_LightGreen          0x0A
#define Console_LightCyan           0x0B
#define Console_LightRed            0x0C
#define Console_LightMagenta        0x0D
#define Console_LightYellow         0x0E
#define Console_LightWhite          0x0F

void Console_Startup();
void Console_Shutdown();

uint8_t Console_CreateAttributes(uint8_t pForeground, uint8_t pBackground);
uint8_t* Console_GetCursor();
void Console_Advance();
void Console_MoveTo(uint8_t pColumn, uint8_t pRow);
void Console_MoveToTopLeft();
void Console_MoveToNextLine();
void Console_Clear(uint8_t pAttributes);
void Console_WriteCharacter(char pCharacter);
void Console_WriteString(const char* pString, uint32_t pLength);
void Console_WriteLine(const char* pLine);

extern uint8_t* gConsole_BaseMemory;
extern uint8_t gConsole_Columns;
extern uint8_t gConsole_Rows;
extern uint8_t gConsole_CursorColumn;
extern uint8_t gConsole_CursorRow;
extern uint8_t gConsole_Attributes;
