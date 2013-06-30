#pragma once

#include "../../Core/Types.hpp"
#include "PortIO.h"

class KernelConsole
{
public:
	static const uint8 DarkBlack = 0x00;
	static const uint8 DarkBlue = 0x01;
	static const uint8 DarkGreen = 0x02;
	static const uint8 DarkCyan = 0x03;
	static const uint8 DarkRed = 0x04;
	static const uint8 DarkMagenta = 0x05;
	static const uint8 DarkYellow = 0x06;
	static const uint8 DarkWhite = 0x07;
	static const uint8 LightBlack = 0x08;
	static const uint8 LightBlue = 0x09;
	static const uint8 LightGreen = 0x0A;
	static const uint8 LightCyan = 0x0B;
	static const uint8 LightRed = 0x0C;
	static const uint8 LightMagenta = 0x0D;
	static const uint8 LightYellow = 0x0E;
	static const uint8 LightWhite = 0x0F;

	static const uintptr BaseMemoryAddress = 0x000B8000;
	static const uint16 DefaultColumns = 80;
	static const uint16 DefaultRows = 25;
	static const uint8 DefaultAttributes = 0x0F;

	static uint8 CreateAttributes(uint8 pForeground, uint8 pBackground) { return (pForeground | (pBackground << 4)); }

	static void Load();

	static void Clear(uint8 pAttributes);

	static void MoveCursorTo(uint8 pColumn, uint8 pRow)
	{
		CursorColumn = pColumn % Columns;
		CursorRow = pRow % Rows;
	}
	static void MoveCursorToTopLeft() { MoveCursorTo(0, 0); }
	static void MoveCursorToNextLine();
	static void Advance();

	static void WriteCharacter(char pCharacter);
	static void WriteString(const char* pString, uint32 pLength);
	static void WriteLine(const char* pString);

private:
	static uint8* BaseMemory;
	static uint8 Columns;
	static uint8 Rows;
	static uint8 CursorColumn;
	static uint8 CursorRow;
	static uint8 Attributes;

	static uint8* GetCursor() { return BaseMemory + (((CursorRow * Columns) + CursorColumn) * 2); }
};
