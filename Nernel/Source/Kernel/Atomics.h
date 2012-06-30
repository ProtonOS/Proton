#pragma once

extern "C" {
uint8_t AtomicCompareExchange(void* pSource, uint8_t pComparand, uint8_t pNewValue);
uint32_t AtomicIncrement(uint32_t* pSource);
}

#define AtomicAquireLock(sourcePtr)		{ while (AtomicCompareExchange(sourcePtr, 0, 1)); }
#define AtomicReleaseLock(sourcePtr)	{ *sourcePtr = 0; }



