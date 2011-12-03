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

void* __dso_handle = nullptr;

int open(const char* pathname, int flags, mode_t mode);
}

#include <Core/DeviceManager.h>
#include <Core/FileSystemManager.h>
#include <Core/MultiBoot.h>

void Halt() { __asm("hlt"); }

void Panic(const char * pMessage)
{
    __asm("cli");
	Core::DeviceManager::GetConsole().Clear(Core::Driver::Console::CreateAttributes(Core::Driver::ConsoleColor::DarkBlack, Core::Driver::ConsoleColor::LightRed));
	Core::DeviceManager::GetConsole().WriteLine(pMessage);
	while (true) Halt();
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

void* sbrk(ptrdiff_t pAdjustment)
{
	static uint8_t memoryMapIndex;
	Core::MultiBoot::MemoryBlock& memoryMap = Core::MultiBoot::GetMemoryBlock(memoryMapIndex);

	if (pAdjustment >= 0)
	{
		uint8_t originalIndex = memoryMapIndex;
		while ((memoryMap.Length - memoryMap.Used) < pAdjustment &&
               memoryMapIndex < Core::MultiBoot::GetMemoryBlockCount())
		{
			++memoryMapIndex;
		    if (memoryMapIndex >= Core::MultiBoot::GetMemoryBlockCount())
		    {
			    memoryMapIndex = originalIndex;
			    Panic("PANIC: Insufficient memory");
		    }
			memoryMap = Core::MultiBoot::GetMemoryBlock(memoryMapIndex);
		}
		memoryMap.Used += pAdjustment;
		return reinterpret_cast<void *>(memoryMap.Address + (memoryMap.Used - pAdjustment));
	}
	memoryMap.Used += pAdjustment;
	if (memoryMap.Used == 0 && memoryMapIndex > 0) --memoryMapIndex;

    return nullptr;
}

_PTR _malloc_r(_reent* pReentrant,
               size_t pSize)
{
    if (pReentrant) { }
    return malloc(pSize);
}

_PTR _calloc_r(_reent* pReentrant,
               size_t pCount,
               size_t pSize)
{
    if (pReentrant) { }
    return calloc(pCount, pSize);
}

void _free_r(_reent* pReentrant,
             _PTR pMemory)
{
    if (pReentrant) { }
    free(pMemory);
}

_PTR _realloc_r(_reent* pReentrant,
                _PTR pMemory,
                size_t pSize)
{
    if (pReentrant) { }
    return realloc(pMemory, pSize);
}

int gettimeofday(timeval* pTime,
                 void* pTimeZone)
{
	if (pTime)
	{
        pTime->tv_sec = 0;
        pTime->tv_usec = 0;
        if (Core::SystemClock::IsReady())
        {
	        pTime->tv_sec = static_cast<time_t>(Core::SystemClock::GetSecondsSinceEpoch());
	        pTime->tv_usec = static_cast<suseconds_t>(Core::SystemClock::GetMilliseconds());
        }
	}
	if (pTimeZone)
	{
		timezone* tz = reinterpret_cast<timezone*>(pTimeZone);
		tz->tz_minuteswest = 0;
		tz->tz_dsttime = 0;
	}
	return 0;
}

int open(const char*,
         int,
         mode_t)
{
    Panic("OPEN");
    errno = ENFILE;
    return -1;
}

int close(int)
{
    Panic("CLOSE");
    errno = EBADF;
    return -1;
}

int fstat(int,
          struct stat*)
{
    Panic("FSTAT");
    errno = EBADF;
    return -1;
}

int stat(const char*,
         struct stat*)
{
    Panic("STAT");
    errno = EBADF;
    return -1;
}

int isatty(int)
{
    Panic("ISATTY");
    errno = EBADF;
    return 0;
}

int write(int pDescriptorIndex,
          const void* pData,
          size_t pLength)
{
    Core::FileDescriptor* descriptor = nullptr;
    if (pDescriptorIndex < 0 ||
        pDescriptorIndex >= Core::FileSystemManager::MaxDescriptors ||
        !(descriptor = Core::FileSystemManager::GetDescriptor(pDescriptorIndex)) ||
        !descriptor->Active)
    {
        errno = EBADF;
        return -1;
    }
    if (!descriptor->Write)
    {
        errno = EINVAL;
        return -1;
    }
    return descriptor->Write(descriptor, pData, pLength);
}

off_t lseek(int,
            off_t,
            int)
{
    Panic("LSEEK");
    errno = EBADF;
    return -1;
}

int read(int,
         void*,
         size_t)
{
    Panic("READ");
    errno = EBADF;
    return -1;
}
