#pragma once

namespace SerialPortLogger
{
	bool IsReady();
	void WriteByte(uint8_t pByte);
}