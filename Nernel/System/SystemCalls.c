#include "Common.h"
#include "Console.h"
#include "Multiboot.h"
#include "SystemClock.h"

#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

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

int gettimeofday(struct timeval* pTime, void* pTimeZone)
{
	if (pTime)
	{
        pTime->tv_sec = 0;
        pTime->tv_usec = 0;
        if (SystemClock_IsReady())
        {
	        pTime->tv_sec = (time_t)SystemClock_GetSecondsSinceEpoch();
	        pTime->tv_usec = (suseconds_t)SystemClock_GetMilliseconds();
        }
	}
	if (pTimeZone)
	{
		struct timezone* tz = (struct timezone*)pTimeZone;
		tz->tz_minuteswest = 0;
		tz->tz_dsttime = 0;
	}
	return 0;
}
