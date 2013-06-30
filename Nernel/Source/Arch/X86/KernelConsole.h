#pragma once

#include "../../Core/Core.hpp"
#include "PortIO.h"

namespace KernelConsole
{
	constexpr UInt8 DarkBlack = 0x00;
	constexpr UInt8 DarkBlue = 0x01;
	constexpr UInt8 DarkGreen = 0x02;
	constexpr UInt8 DarkCyan = 0x03;
	constexpr UInt8 DarkRed = 0x04;
	constexpr UInt8 DarkMagenta = 0x05;
	constexpr UInt8 DarkYellow = 0x06;
	constexpr UInt8 DarkWhite = 0x07;
	constexpr UInt8 LightBlack = 0x08;
	constexpr UInt8 LightBlue = 0x09;
	constexpr UInt8 LightGreen = 0x0A;
	constexpr UInt8 LightCyan = 0x0B;
	constexpr UInt8 LightRed = 0x0C;
	constexpr UInt8 LightMagenta = 0x0D;
	constexpr UInt8 LightYellow = 0x0E;
	constexpr UInt8 LightWhite = 0x0F;

	constexpr UInt BaseMemoryAddress = 0x000B8000;
	constexpr UInt16 DefaultColumns = 80;
	constexpr UInt16 DefaultRows = 25;
	constexpr UInt8 DefaultAttributes = 0x0F;

    UInt8 CreateAttributes(UInt8 pForeground, UInt8 pBackground);

	void Load();

	void Clear(UInt8 pAttributes);

    void MoveCursorTo(UInt8 pColumn, UInt8 pRow);
    void MoveCursorToTopLeft();
	void MoveCursorToNextLine();
	void Advance();

	void WriteCharacter(char pCharacter);
	void WriteString(const char* pString, UInt32 pLength);
	void WriteLine(const char* pString);


	
};
