#pragma once

#include "x86/PortIO.h"

uint8_t Atomic_CompareExchange(volatile void* pSource, uint8_t pComparand, uint8_t pNewValue);
uint32_t Atomic_Increment(volatile uint32_t* pSource);
uint32_t Atomic_Add(volatile uint32_t* pSource, int32_t pAdded);

#define Atomic_AquireLock(sourcePtr)		{ while (Atomic_CompareExchange(sourcePtr, 0, 1)) IOWAIT(); }
#define Atomic_ReleaseLock(sourcePtr)		{ *sourcePtr = 0; }


