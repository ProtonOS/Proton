#pragma once

void SerialLogger_Startup();
void SerialLogger_Shutdown();

bool_t SerialLogger_IsTransmitEmpty();
void SerialLogger_WriteByte(uint8_t pByte);
