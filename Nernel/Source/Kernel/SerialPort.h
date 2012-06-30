#pragma once

class SerialPort
{
public:
	virtual bool IsReady() = 0;
	virtual void WriteByte(uint8_t pByte) = 0;
};