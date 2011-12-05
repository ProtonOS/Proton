#pragma once

#include <types.h>

#define PIC_MasterIOPort            0x0020
#define PIC_MasterCommandIOPort     PIC_MasterIOPort + 0x00
#define PIC_MasterDataIOPort        PIC_MasterIOPort + 0x01

#define PIC_SlaveIOPort             0x00A0
#define PIC_SlaveCommandIOPort      PIC_SlaveIOPort + 0x00
#define PIC_SlaveDataIOPort         PIC_SlaveIOPort + 0x01

#define PIC_ConfigModeCommand       0x01
#define PIC_SinglePICCommand        0x02
#define PIC_Interval4Command        0x04
#define PIC_LevelModeCommand        0x08
#define PIC_InitializeCommand       0x10
#define PIC_ResetCommand            0x20

#define PIC_MasterToSlaveLine       0x04
#define PIC_SlaveToMasterLine       0x02

#define PIC_Microprocessor8086Mode  0x01
#define PIC_AutoEndOfInterruptMode  0x02
#define PIC_MasterMode              0x04
#define PIC_BufferedMode            0x08
#define PIC_SpecialFullyNestedMode  0x10

void PIC_Startup();
void PIC_Shutdown();

void PIC_StartInterrupts();
void PIC_StopInterrupts();
void PIC_ResetInterrupts(bool_t pSlave);
