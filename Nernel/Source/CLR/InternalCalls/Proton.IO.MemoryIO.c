#include <CLR/InternalCalls/Proton.IO.MemoryIO.h>
#include <CLR/InternalCalls/Helpers.h>

uint32_t Proton_IO_MemoryIO_InByte(AppDomain* pAppDomain, uint32_t address)
{
	return *((uint8_t*)address);
}

uint32_t Proton_IO_MemoryIO_InUShort(AppDomain* pAppDomain, uint32_t address)
{
	return *((uint16_t*)address);
}

uint32_t Proton_IO_MemoryIO_InUInt(AppDomain* pAppDomain, uint32_t address)
{
	return *((uint32_t*)address);
}


void Proton_IO_MemoryIO_OutByte(AppDomain* pAppDomain, uint32_t address, uint32_t value)
{
    *((uint8_t*)address) = (uint8_t)value;
}

void Proton_IO_MemoryIO_OutUShort(AppDomain* pAppDomain, uint32_t address, uint32_t value)
{
	*((uint16_t*)address) = (uint16_t)value;
}

void Proton_IO_MemoryIO_OutUInt(AppDomain* pAppDomain, uint32_t address, uint32_t value)
{
	*((uint32_t*)address) = value;
}

