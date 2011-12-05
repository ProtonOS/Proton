#pragma once

#include <types.h>

#include <Core/InterruptRegisters.h>

#define IDT_MaxDescriptors                  256
#define IDT_RemappedIRQBase                 0x20

#define IDT_PriviledgeLevelRing0Selector    0x0000
#define IDT_PriviledgeLevelRing1Selector    0x0001
#define IDT_PriviledgeLevelRing2Selector    0x0002
#define IDT_PriviledgeLevelRing3Selector    0x0003
#define IDT_TableIndexSelector              0x0004
#define IDT_DescriptorIndexSelector         0x0008

#define IDT_Task386Gate32BitType            0x05
#define IDT_Interrupt286Gate16BitType       0x06
#define IDT_Trap286Gate16BitType            0x07
#define IDT_Interrupt386Gate32BitType       0x0E
#define IDT_Trap386Gate32BitType            0x0F
#define IDT_PresentType                     0x80

typedef struct
{
    uint16_t Limit;
    uint32_t Address;
} IDT_Register;

typedef struct
{
    uint16_t AddressLow;
    uint16_t Selector;
    uint8_t Zero;
    uint8_t TypeAndFlags;
    uint16_t AddressHigh;
} IDT_Descriptor;

typedef void (*IDT_Handler)(InterruptRegisters pRegisters);

void IDT_Startup();
void IDT_Shutdown();

void IDT_SetInterrupt(uint8_t pIndex, uint32_t pAddress, uint16_t pSelector, uint8_t pTypeAndFlags);
void IDT_Schedule(uint8_t pInterrupt);
void IDT_Unschedule(uint8_t pInterrupt);
void IDT_WaitFor(uint8_t pInterrupt);
void IDT_RegisterHandler(uint8_t pInterrupt, IDT_Handler pHandler);
IDT_Handler IDT_GetHandler(uint8_t pInterrupt);

extern IDT_Register gIDT_Register;
extern IDT_Descriptor gIDT_Descriptors[];
extern bool_t gIDT_Scheduled[];
extern IDT_Handler gIDT_Handlers[];
