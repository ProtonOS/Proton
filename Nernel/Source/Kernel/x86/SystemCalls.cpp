#include "../Atomics.h"
#include "../Multiboot.h"
#include "../Processor.h"
#include "../Thread.h"
#include "TSS.h"

#include <reent.h>
#include <sys/time.h>
#include <unistd.h>

extern "C" {
struct _reent* __getreent();
void __malloc_lock(struct _reent* pReent);
void __malloc_unlock(struct _reent* pReent);
long sysconf(int pSetting);
void* sbrk(ptrdiff_t pAdjustment);
int open(const char* pPath, int pFlags, mode_t pMode);
int close(int pDescriptorIndex);
int fstat(int pDescriptorIndex, struct stat* pStats);
int stat(const char* pPath, struct stat* pStats);
int isatty(int pDescriptorIndex);
off_t lseek(int pDescriptorIndex, off_t pOffset, int pWhence);
int read(int pDescriptorIndex, void* pData, size_t pLength);
int write(int pDescriptorIndex, const void* pData, size_t pLength);
pid_t getpid();
int gettimeofday(struct timeval* pTime, void* pTimeZone);
}

struct _reent gDefaultReent;
bool gMultitasking = false;	
uint8_t gMallocBusy = 0;
void *__dso_handle = nullptr;

struct _reent* __getreent()
{
	if (!gMultitasking) return &gDefaultReent;
	uint32_t taskRegister = TSSGetRegister();
	uint16_t processorIndex = (taskRegister - 0x2B) >> 3;
	Processor* processor = Processor::GetProcessor(processorIndex);
	return processor->GetCurrentThread()->GetReentrant();
}

void __malloc_lock(struct _reent* pReent)
{
	AtomicCompareExchange(&gMallocBusy, 0, 1);
	Thread* thread = (Thread*)pReent->_new._reent._unused_rand;
	thread->IncrementMallocLockDepth();
}

void __malloc_unlock(struct _reent* pReent)
{
	Thread* thread = (Thread*)pReent->_new._reent._unused_rand;
	if (thread->DecrementMallocLockDepth()) AtomicReleaseLock(&gMallocBusy);
}

long sysconf(int pSetting)
{
    if (pSetting == _SC_PAGE_SIZE) return 1;
    errno = EINVAL;
    return -1;
}

void* sbrk(ptrdiff_t pAdjustment)
{
	return Multiboot::AdjustMemory(pAdjustment);
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

int write(int pDescriptorIndex, const void* pData, size_t pLength)
{
    if (pDescriptorIndex == STDOUT_FILENO ||
        pDescriptorIndex == STDERR_FILENO)
    {
//        Console_WriteString((const char*)pData, pLength);
        return (int)pLength;
    }
    Panic("WRITE");
    errno = EBADF;
    return -1;
}

pid_t getpid()
{
	return 0;
}

int gettimeofday(struct timeval* pTime, void* pTimeZone)
{
	if (pTime)
	{
        pTime->tv_sec = 0;
        pTime->tv_usec = 0;
        //if (SystemClock_IsReady())
        //{
	       // pTime->tv_sec = (time_t)SystemClock_GetSecondsSinceEpoch();
	       // pTime->tv_usec = (suseconds_t)SystemClock_GetMilliseconds();
        //}
	}
	if (pTimeZone)
	{
		struct timezone* tz = (struct timezone*)pTimeZone;
		tz->tz_minuteswest = 0;
		tz->tz_dsttime = 0;
	}
	return 0;
}
