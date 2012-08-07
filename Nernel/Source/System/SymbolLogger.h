#pragma once

void SymbolLogger_Startup();
void SymbolLogger_Shutdown();

bool_t SymbolLogger_IsTransmitEmpty();
void SymbolLogger_WriteByte(uint8_t pByte);
void SymbolLogger_WriteLine(const char* pLine);
