#pragma once

#define SerialWriter_BaseIOPort         0x02F8
#define SerialWriter_DataIOPort         SerialWriter_BaseIOPort + 0x00
#define SerialWriter_InterruptIOPort    SerialWriter_BaseIOPort + 0x01
#define SerialWriter_FIFOIOPort         SerialWriter_BaseIOPort + 0x02
#define SerialWriter_LineDataIOPort     SerialWriter_BaseIOPort + 0x03
#define SerialWriter_ModemDataIOPort    SerialWriter_BaseIOPort + 0x04
#define SerialWriter_LineStatusIOPort   SerialWriter_BaseIOPort + 0x05
#define SerialWriter_ModemStatusIOPort  SerialWriter_BaseIOPort + 0x06
#define SerialWriter_ScratchIOPort      SerialWriter_BaseIOPort + 0x07

#define SerialWriter_TransmitWait       1000

void SerialWriter_Startup();
void SerialWriter_Shutdown();

bool_t SerialWriter_IsTransmitEmpty();
void SerialWriter_WriteByte(uint8_t pByte);
