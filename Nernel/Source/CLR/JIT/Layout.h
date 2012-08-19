#pragma once

#include <Inline.h>
#include <CLR/IRStructures.h>

extern const uint32_t gSizeOfPointerInBytes;

ALWAYS_INLINE inline uint32_t JIT_StackAlign(uint32_t pSize)
{
	if (pSize & (gSizeOfPointerInBytes - 1)) pSize = pSize + (gSizeOfPointerInBytes - (pSize & (gSizeOfPointerInBytes - 1)));
	return pSize;
}

uint32_t JIT_GetStackSizeOfType(IRType* pType);
uint32_t JIT_GetSizeOfType(IRType* pType);
