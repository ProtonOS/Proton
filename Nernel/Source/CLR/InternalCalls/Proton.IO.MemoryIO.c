#include <CLR/InternalCalls/Proton.IO.MemoryIO.h>
#include <CLR/InternalCalls/Helpers.h>

void Proton_IO_MemoryIO_InByte(void* pThis_, void* pParams, void* pReturnValue)
{
	uint32_t address = INTERNAL_CALL_PARAM(0, uint32_t);
	*(uint8_t*)pReturnValue = *((uint8_t*)address);
}

void Proton_IO_MemoryIO_InUShort(void* pThis_, void* pParams, void* pReturnValue)
{
	uint32_t address = INTERNAL_CALL_PARAM(0, uint32_t);
	*(uint16_t*)pReturnValue = *((uint16_t*)address);
}

void Proton_IO_MemoryIO_InUInt(void* pThis_, void* pParams, void* pReturnValue)
{
	uint32_t address = INTERNAL_CALL_PARAM(0, uint32_t);
	*(uint32_t*)pReturnValue = *((uint32_t*)address);
}


void Proton_IO_MemoryIO_OutByte(void* pThis_, void* pParams, void* pReturnValue)
{
	uint32_t address = INTERNAL_CALL_PARAM(0, uint32_t);
	uint8_t value = INTERNAL_CALL_PARAM(4, uint8_t);
    *((uint8_t*)address) = value;
}

void Proton_IO_MemoryIO_OutUShort(void* pThis_, void* pParams, void* pReturnValue)
{
	uint32_t address = INTERNAL_CALL_PARAM(0, uint32_t);
	uint16_t value = INTERNAL_CALL_PARAM(4, uint16_t);
	*((uint16_t*)address) = value;
}

void Proton_IO_MemoryIO_OutUInt(void* pThis_, void* pParams, void* pReturnValue)
{
	uint32_t address = INTERNAL_CALL_PARAM(0, uint32_t);
	uint32_t value = INTERNAL_CALL_PARAM(4, uint32_t);
	*((uint32_t*)address) = value;
}

