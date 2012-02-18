#include <CLR/InternalCalls/Proton.IO.MemoryIO.h>
#include <CLR/InternalCalls/Helpers.h>

void Proton_IO_MemoryIO_InByte(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	uint32_t address = INTERNAL_CALL_PARAM(0, uint32_t);
	*(uint8_t*)pReturn = *((uint8_t*)address);
}

void Proton_IO_MemoryIO_InUShort(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	uint32_t address = INTERNAL_CALL_PARAM(0, uint32_t);
	*(uint16_t*)pReturn = *((uint16_t*)address);
}

void Proton_IO_MemoryIO_InUInt(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	uint32_t address = INTERNAL_CALL_PARAM(0, uint32_t);
	*(uint32_t*)pReturn = *((uint32_t*)address);
}


void Proton_IO_MemoryIO_OutByte(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	uint32_t address = INTERNAL_CALL_PARAM(0, uint32_t);
	uint8_t value = INTERNAL_CALL_PARAM(1, uint8_t);
    *((uint8_t*)address) = value;
}

void Proton_IO_MemoryIO_OutUShort(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	uint32_t address = INTERNAL_CALL_PARAM(0, uint32_t);
	uint16_t value = INTERNAL_CALL_PARAM(1, uint16_t);
	*((uint16_t*)address) = value;
}

void Proton_IO_MemoryIO_OutUInt(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	uint32_t address = INTERNAL_CALL_PARAM(0, uint32_t);
	uint32_t value = INTERNAL_CALL_PARAM(1, uint32_t);
	*((uint32_t*)address) = value;
}

