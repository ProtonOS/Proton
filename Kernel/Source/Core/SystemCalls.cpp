extern "C" {
#include <signal.h>
#include <errno.h>
#include <types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>

#undef errno
extern int errno;

void * __dso_handle = nullptr;

int open(const char * pathname, int flags, mode_t mode);
}

#include <Core/DeviceManager.h>
#include <Core/MultiBoot.h>

using namespace Core;

void Halt() { __asm("hlt"); }

void Panic(const char * pMessage)
{
    __asm("cli");
	DeviceManager::GetConsole().Clear(Console::CreateAttributes(Console::Color::DarkBlack, Console::Color::LightRed));
	DeviceManager::GetConsole().WriteLine(pMessage);
	while (TRUE) Halt();
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
    while(true);
}

void * sbrk(ptrdiff_t pAdjustment)
{
	static uint8_t memoryMapIndex;
	MultiBoot::MemoryBlock * memoryMap = MultiBoot::GetMemoryBlock(memoryMapIndex);

	if (pAdjustment >= 0)
	{
		uint8_t originalIndex = memoryMapIndex;
		while ((memoryMap->Length - memoryMap->Used) < pAdjustment &&
               memoryMapIndex < MultiBoot::GetMemoryBlockCount())
		{
			++memoryMapIndex;
			++memoryMap;
		}
		if (memoryMapIndex >= MultiBoot::GetMemoryBlockCount())
		{
			memoryMapIndex = originalIndex;
			Panic("PANIC: Insufficient memory");
		}
		memoryMap->Used += pAdjustment;
		return reinterpret_cast<void *>(memoryMap->Address + (memoryMap->Used - pAdjustment));
	}
	memoryMap->Used += pAdjustment;
	if (memoryMap->Used == 0 && memoryMapIndex > 0) --memoryMapIndex;

    return nullptr;
}

_PTR _malloc_r(_reent * pReentrant, size_t pSize)
{
    if (pReentrant) { }
    return malloc(pSize);
}

_PTR _calloc_r(_reent * pReentrant, size_t pCount, size_t pSize)
{
    if (pReentrant) { }
    return calloc(pCount, pSize);
}

void _free_r(_reent * pReentrant, _PTR pMemory)
{
    if (pReentrant) { }
    free(pMemory);
}

_PTR _realloc_r(_reent * pReentrant, _PTR pMemory, size_t pSize)
{
    if (pReentrant) { }
    return realloc(pMemory, pSize);
}

int gettimeofday(timeval * pTime,
                 void * pTimeZone)
{
	if (pTime)
	{
		pTime->tv_sec = 0;
		pTime->tv_usec = 0;
	}
	if (pTimeZone)
	{
		timezone * tz = reinterpret_cast<timezone *>(pTimeZone);
		tz->tz_minuteswest = 0;
		tz->tz_dsttime = 0;
	}
	return 0;
}

int open(const char*,
         int,
         mode_t)
{
    errno = ENFILE;
    return -1;
}

int close(int)
{
    errno = EBADF;
    return -1;
}


int fstat(int,
          struct stat*)
{
    errno = EBADF;
    return -1;
}

int stat(const char*,
         struct stat*)
{
    errno = EBADF;
    return -1;
}

int isatty(int)
{
    errno = EBADF;
    return 0;
}

int write(int,
          const void*,
          size_t)
{
    errno = EBADF;
    return -1;
}

off_t lseek(int,
            off_t,
            int)
{
    errno = EBADF;
    return -1;
}

int read(int,
         void*,
         size_t)
{
    errno = EBADF;
    return -1;
}
