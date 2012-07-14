#pragma once

namespace Console
{
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
	void Startup();
	void Clear(Color pForeground, Color pBackground);
	void SetColors(Color pForeground, Color pBackground);
	Color GetForegroundColor();
	Color GetBackgroundColor();
	void WriteString(const char* pString, size_t pLength);
	void WriteLine(const char* pLine);
};
