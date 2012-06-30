#pragma once

#include "../../SerialPort.h"

class SerialPortLogger : public SerialPort
{
public:
	static SerialPortLogger* Instance;

	SerialPortLogger();

	virtual bool IsReady();
	virtual void WriteByte(uint8_t pByte);
};
