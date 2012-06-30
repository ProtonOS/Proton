#pragma once

#include "../../Console.h"

class VGAConsole : public Console
{
private:
	uint8_t* mBaseMemory;
	uint8_t mColumns;
	uint8_t mRows;
	uint8_t mCursorColumn;
	uint8_t mCursorRow;
	uint8_t mAttributes;

public:
	VGAConsole();

	uint8_t CreateAttributes(Color pForeground, Color pBackground) { return (uint8_t)(pForeground | (pBackground << 4)); }

	uint8_t* GetCursor();
	void MoveTo(uint8_t pColumn, uint8_t pRow);
	void MoveToTopLeft();
	void MoveToNextLine();
	void Advance();
	void WriteCharacter(char pCharacter);
	void WriteString(const char* pString, uint32_t pLength);

	virtual void Clear(Color pForeground, Color pBackground);
	virtual void SetColors(Color pForeground, Color pBackground);
	virtual Color GetForegroundColor();
	virtual Color GetBackgroundColor();
	virtual void WriteLine(const char* pLine);
};