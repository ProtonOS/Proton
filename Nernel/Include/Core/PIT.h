#pragma once

#include <types.h>
#include <Core/InterruptRegisters.h>

#define PIT_MaxHertz                        1193182

#define PIT_BaseIOPort                      0x0040
#define PIT_Counter0IOPort                  PIT_BaseIOPort + 0x00
#define PIT_Counter1IOPort                  PIT_BaseIOPort + 0x01
#define PIT_Counter2IOPort                  PIT_BaseIOPort + 0x02
#define PIT_CommandIOPort                   PIT_BaseIOPort + 0x03
#define PIT_BaseIRQ                         0x00

#define PIT_BinaryCodedDecimalCommand       0x01
#define PIT_Mode0Command                    0x00
#define PIT_Mode1Command                    0x02
#define PIT_Mode2Command                    0x04
#define PIT_Mode3Command                    0x06
#define PIT_Mode4Command                    0x08
#define PIT_Mode5Command                    0x0A
#define PIT_ReadWriteLatchCommand           0x00
#define PIT_ReadWriteLSBOnlyCommand         0x10
#define PIT_ReadWriteMSBOnlyCommand         0x20
#define PIT_ReadWriteLSBThenMSBCommand      0x30
#define PIT_SelectCounter0Command           0x00
#define PIT_SelectCounter1Command           0x40
#define PIT_SelectCounter2Command           0x80
#define PIT_SelectReadBackCommand           0xC0
#define PIT_Counter0InitializeCommand       PIT_Mode3Command | PIT_ReadWriteLSBThenMSBCommand | PIT_SelectCounter0Command

void PIT_Startup(uint16_t pHertz);
void PIT_Shutdown();

void PIT_Interrupt(InterruptRegisters pRegisters);
uint16_t PIT_GetMillisecondsElapsed();
uint32_t PIT_GetSecondsElapsed();

extern uint16_t gPIT_Hertz;
extern uint16_t gPIT_Cycle;
extern uint32_t gPIT_MillisecondsPerCycle;
extern uint32_t gPIT_MillisecondsElapsed;
extern uint32_t gPIT_SecondsElapsed;
