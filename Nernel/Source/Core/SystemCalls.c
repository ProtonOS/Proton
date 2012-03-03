#include <signal.h>
#include <errno.h>
#include <types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <Core/Console.h>
#include <Core/MultiBoot.h>
#include <Core/SystemClock.h>

#undef errno
extern int errno;

int open(const char* pathname, int flags, mode_t mode);


void Halt() { __asm("hlt"); }

void Panic(const char* pMessage)
{
    __asm("cli");
	Console_Clear(Console_CreateAttributes(Console_DarkBlack, Console_LightRed));
	Console_WriteLine(pMessage);
	while (TRUE) Halt();
}

void _fini() { }

char* getcwd(char* buf, size_t bufLen)
{
	static const char* curDir = (const char*)"/";
	static size_t curDirLen = 1;
	if (curDirLen > bufLen)
		Panic("Buffer not big enough for current directory!");
	strcpy(buf, curDir);
	return (char*)curDir;
}

long sysconf(int pSetting)
{
    if (pSetting == _SC_PAGE_SIZE) return 1;
    errno = EINVAL;
    return -1;
}

void abort()
{
    Panic("Aborted");
    while(TRUE);
}

void* sbrk(ptrdiff_t pAdjustment)
{
	static uint8_t memoryBlockIndex;
	MultiBoot_MemoryBlock* memoryBlock = &gMultiBoot_MemoryBlocks[memoryBlockIndex];
	if (pAdjustment >= 0)
	{
        uint32_t adjustment = (uint32_t)pAdjustment;
		uint8_t originalIndex = memoryBlockIndex;
		while ((memoryBlock->Length - memoryBlock->Used) < adjustment &&
               memoryBlockIndex < gMultiBoot_MemoryBlockCount)
		{
			++memoryBlockIndex;
		    if (memoryBlockIndex >= gMultiBoot_MemoryBlockCount)
		    {
			    memoryBlockIndex = originalIndex;
			    Panic("PANIC: Insufficient memory");
		    }
			memoryBlock = &gMultiBoot_MemoryBlocks[memoryBlockIndex];
		}
		memoryBlock->Used += adjustment;
		//printf("sbrk adjusting by %d, returning @ 0x%x\n", (int)pAdjustment, (unsigned int)(memoryBlock->Address + (memoryBlock->Used - adjustment)));
		return (void*)(memoryBlock->Address + (memoryBlock->Used - adjustment));
	}
	memoryBlock->Used += pAdjustment;
	if (memoryBlock->Used == 0 && memoryBlockIndex > 0) --memoryBlockIndex;

    return NULL;
}

int gettimeofday(struct timeval* pTime,
                 void* pTimeZone)
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

int open(const char* pPath,
         int pFlags,
         mode_t pMode)
{
    Panic("OPEN");
    errno = ENFILE;
    return -1;
}

int close(int pDescriptorIndex)
{
    //Panic("CLOSE");
    errno = EBADF;
    return -1;
}

int fstat(int pDescriptorIndex,
          struct stat* pStats)
{
    Panic("FSTAT");
    errno = EBADF;
    return -1;
}

int stat(const char* pPath,
         struct stat* pStats)
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

int write(int pDescriptorIndex,
          const void* pData,
          size_t pLength)
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

off_t lseek(int pDescriptorIndex,
            off_t pOffset,
            int pWhence)
{
    Panic("LSEEK");
    errno = EBADF;
    return -1;
}

int read(int pDescriptorIndex,
         void* pData,
         size_t pLength)
{
    Panic("READ");
    errno = EBADF;
    return -1;
}
