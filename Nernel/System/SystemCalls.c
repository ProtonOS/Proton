#include "Common.h"
#include "Console.h"
#include "Multiboot.h"

#include <unistd.h>
#include <sys/stat.h>

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

int open(const char* pPath, int pFlags, mode_t pMode)
{
    Panic("OPEN");
    errno = ENFILE;
    return -1;
}

int close(int pDescriptorIndex)
{
    Panic("CLOSE");
    errno = EBADF;
    return -1;
}

int fstat(int pDescriptorIndex, struct stat* pStats)
{
    Panic("FSTAT");
    errno = EBADF;
    return -1;
}

int stat(const char* pPath, struct stat* pStats)
{
    Panic("STAT");
    errno = EBADF;
    return -1;
}

int isatty(int pDescriptorIndex)
{
    Panic("ISATTY");
    errno = EBADF;
    return 0;
}

int write(int pDescriptorIndex, const void* pData, size_t pLength)
{
    if (pDescriptorIndex == STDOUT_FILENO ||
        pDescriptorIndex == STDERR_FILENO)
    {
        Console_WriteString((const char*)pData, pLength);
        return (int)pLength;
    }
    Panic("WRITE");
    errno = EBADF;
    return -1;
}

off_t lseek(int pDescriptorIndex, off_t pOffset, int pWhence)
{
    Panic("LSEEK");
    errno = EBADF;
    return -1;
}

int read(int pDescriptorIndex, void* pData, size_t pLength)
{
    Panic("READ");
    errno = EBADF;
    return -1;
}
