#pragma once

#define PIC__IOPort__Master					0x0020
#define PIC__IOPort__Master__Command		PIC__IOPort__Master + 0x00
#define PIC__IOPort__Master__Data			PIC__IOPort__Master + 0x01

#define PIC__IOPort__Slave					0x00A0
#define PIC__IOPort__Slave__Command			PIC__IOPort__Slave + 0x00
#define PIC__IOPort__Slave__Data			PIC__IOPort__Slave + 0x01

#define PIC__Command__ConfigMode			0x01
#define PIC__Command__SinglePIC				0x02
#define PIC__Command__Interval4				0x04
#define PIC__Command__LevelMode				0x08
#define PIC__Command__Initialize			0x10
#define PIC__Command__Reset					0x20

#define PIC__Line__MasterToSlave			0x04
#define PIC__Line__SlaveToMaster			0x02

#define PIC__Mode__Microprocessor8086		0x01
#define PIC__Mode__AutoEndOfInterrupt		0x02
#define PIC__Mode__Master					0x04
#define PIC__Mode__Buffered					0x08
#define PIC__Mode__SpecialFullyNested		0x10

void PIC_Startup();
void PIC_Shutdown();

void PIC_ResetInterrupts(bool_t pSlave);

extern void PIC_StartInterrupts();
extern void PIC_StopInterrupts();

