#pragma once

#define IDT__IRQ__RemappedBase				0x20

#define IDT__Selector__PriviledgeLevel__Ring_0		0x0000
#define IDT__Selector__PriviledgeLevel__Ring_1		0x0001
#define IDT__Selector__PriviledgeLevel__Ring_2		0x0002
#define IDT__Selector__PriviledgeLevel__Ring_3		0x0003
#define IDT__Selector__TableIndex					0x0004
#define IDT__Selector__DescriptorIndex				0x0008

#define IDT__Type__Task386_Gate32Bit				0x05
#define IDT__Type__Interrupt286_Gate16Bit			0x06
#define IDT__Type__Trap286_Gate16Bit				0x07
#define IDT__Type__Interrupt386_Gate32Bit			0x0E
#define IDT__Type__Trap386_Gate32Bit				0x0F
#define IDT__Type__Present							0x80


typedef struct _InterruptRegisters InterruptRegisters;
typedef struct _IDTRegister IDTRegister;
typedef struct _IDTDescriptor IDTDescriptor;

struct _InterruptRegisters
{
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t int_no;
    uint32_t err_code;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
};

struct _IDTRegister
{
    uint16_t Limit;
    uint32_t Address;
};

struct _IDTDescriptor
{
    uint16_t AddressLow;
    uint16_t Selector;
    uint8_t Zero;
    uint8_t TypeAndFlags;
    uint16_t AddressHigh;
};

typedef void (*IDTHandler)(InterruptRegisters pRegisters);

void IDT_Startup();
void IDT_Shutdown();

void IDT_SetInterrupt(uint8_t pIndex, uint32_t pAddress, uint16_t pSelector, uint8_t pTypeAndFlags);
uint32_t IDT_GetInterrupt(uint8_t pIndex);
void IDT_Schedule(uint8_t pInterrupt);
void IDT_Unschedule(uint8_t pInterrupt);
void IDT_WaitFor(uint8_t pInterrupt);
void IDT_RegisterHandler(uint8_t pInterrupt, IDTHandler pHandler);
IDTHandler IDT_GetHandler(uint8_t pInterrupt);

extern void IDT_Update(IDTRegister* pRegister);

extern void IDT_ThrowInterrupt80();

extern IDTRegister gIDT_Register;
