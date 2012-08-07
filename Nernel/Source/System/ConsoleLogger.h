#pragma once

void ConsoleLogger_Startup();
void ConsoleLogger_Shutdown();

bool_t ConsoleLogger_IsTransmitEmpty();
void ConsoleLogger_WriteByte(uint8_t pByte);
