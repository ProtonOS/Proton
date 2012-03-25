#include "Common.h"
#include "Multiboot.h"

void* sbrk(ptrdiff_t pAdjustment)
{
	static uint8_t currentMemoryBlockIndex;
	MemoryBlock* memoryBlock = &gMemoryBlocks[currentMemoryBlockIndex];
	if (pAdjustment >= 0)
	{
        uint32_t adjustment = (uint32_t)pAdjustment;
		uint8_t originalIndex = currentMemoryBlockIndex;
		while ((memoryBlock->Size - memoryBlock->Used) < adjustment &&
               currentMemoryBlockIndex < gMemoryBlockCount)
		{
			++currentMemoryBlockIndex;
		    if (currentMemoryBlockIndex >= gMemoryBlockCount)
		    {
			    currentMemoryBlockIndex = originalIndex;
			    Panic("Insufficient memory for allocator break");
		    }
			memoryBlock = &gMemoryBlocks[currentMemoryBlockIndex];
		}
		memoryBlock->Used += adjustment;
		return (void*)(memoryBlock->Address + (memoryBlock->Used - adjustment));
	}
	memoryBlock->Used += pAdjustment;
	if (memoryBlock->Used == 0 && currentMemoryBlockIndex > 0) --currentMemoryBlockIndex;
    return NULL;
}