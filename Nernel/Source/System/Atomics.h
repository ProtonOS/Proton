#pragma once

#include "x86/PortIO.h"

uint8_t Atomic_CompareExchange(void* pSource, uint8_t pComparand, uint8_t pNewValue);
uint32_t Atomic_Increment(uint32_t* pSource);

#define Atomic_AquireLock(sourcePtr)		{ while (Atomic_CompareExchange(sourcePtr, 0, 1)) IOWAIT(); }
#define Atomic_ReleaseLock(sourcePtr)		{ *sourcePtr = 0; }


