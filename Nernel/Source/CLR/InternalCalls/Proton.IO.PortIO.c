#include <CLR/InternalCalls/Proton.IO.PortIO.h>
#include <CLR/InternalCalls/Helpers.h>
#include <PortIO.h>

void Proton_IO_PortIO_InByte(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	uint16_t port = INTERNAL_CALL_PARAM(0, uint16_t);
	*(uint8_t*)pReturn = inb(port);
}

void Proton_IO_PortIO_InUShort(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	uint16_t port = INTERNAL_CALL_PARAM(0, uint16_t);
	*(uint16_t*)pReturn = inw(port);
}

void Proton_IO_PortIO_InUInt(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	uint16_t port = INTERNAL_CALL_PARAM(0, uint16_t);
	*(uint32_t*)pReturn = inl(port);
}


void Proton_IO_PortIO_OutByte(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	uint16_t port = INTERNAL_CALL_PARAM(0, uint16_t);
	uint8_t value = INTERNAL_CALL_PARAM(1, uint8_t);
	outb(port, value);
}

void Proton_IO_PortIO_OutUShort(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	uint16_t port = INTERNAL_CALL_PARAM(0, uint16_t);
	uint16_t value = INTERNAL_CALL_PARAM(1, uint16_t);
	outw(port, value);
}

void Proton_IO_PortIO_OutUInt(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	uint16_t port = INTERNAL_CALL_PARAM(0, uint16_t);
	uint32_t value = INTERNAL_CALL_PARAM(1, uint32_t);
	outl(port, value);
}

