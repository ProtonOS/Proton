#pragma once

#include <Inline.h>
#include <CLR/IRStructures.h>

ALWAYS_INLINE inline uint32_t JIT_StackAlign(uint32_t pSize)
{
	return pSize + (gSizeOfPointerInBytes - (pSize & (gSizeOfPointerInBytes - 1)));
}

uint32_t JIT_GetStackSizeOfType(IRType* pType);
