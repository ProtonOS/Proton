#pragma once

#define INTERRUPTDESCRIPTORTABLE_REMAPPEDIRQBASE                 0x20

typedef struct _InterruptRegisters InterruptRegisters;
typedef struct _InterruptDescriptorTableRegister InterruptDescriptorTableRegister;
typedef struct _InterruptDescriptorTableDescriptor InterruptDescriptorTableDescriptor;

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

struct _InterruptDescriptorTableRegister
{
    uint16_t Limit;
    uint32_t Address;
};

struct _InterruptDescriptorTableDescriptor
{
    uint16_t AddressLow;
    uint16_t Selector;
    uint8_t Zero;
    uint8_t TypeAndFlags;
    uint16_t AddressHigh;
};

typedef void (*InterruptDescriptorTableHandler)(InterruptRegisters pRegisters);

void InterruptDescriptorTable_Startup();
void InterruptDescriptorTable_Shutdown();

void InterruptDescriptorTable_SetInterrupt(uint8_t pIndex, uint32_t pAddress, uint16_t pSelector, uint8_t pTypeAndFlags);
void InterruptDescriptorTable_Schedule(uint8_t pInterrupt);
void InterruptDescriptorTable_Unschedule(uint8_t pInterrupt);
void InterruptDescriptorTable_WaitFor(uint8_t pInterrupt);
void InterruptDescriptorTable_RegisterHandler(uint8_t pInterrupt, InterruptDescriptorTableHandler pHandler);
InterruptDescriptorTableHandler InterruptDescriptorTable_GetHandler(uint8_t pInterrupt);
