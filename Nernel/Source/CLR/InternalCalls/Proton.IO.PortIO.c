#include <CLR/InternalCalls/Proton.IO.PortIO.h>
#include <CLR/InternalCalls/Helpers.h>
#include <PortIO.h>

void Proton_IO_PortIO_InByte(void* pThis_, void* pParams, void* pReturnValue)
{
	uint16_t port = INTERNAL_CALL_PARAM(0, uint16_t);
	*(uint8_t*)pReturnValue = inb(port);
}

void Proton_IO_PortIO_InUShort(void* pThis_, void* pParams, void* pReturnValue)
{
	uint16_t port = INTERNAL_CALL_PARAM(0, uint16_t);
	*(uint16_t*)pReturnValue = inw(port);
}

void Proton_IO_PortIO_InUInt(void* pThis_, void* pParams, void* pReturnValue)
{
	uint16_t port = INTERNAL_CALL_PARAM(0, uint16_t);
	*(uint32_t*)pReturnValue = inl(port);
}


void Proton_IO_PortIO_OutByte(void* pThis_, void* pParams, void* pReturnValue)
{
	uint16_t port = INTERNAL_CALL_PARAM(0, uint16_t);
	uint8_t value = INTERNAL_CALL_PARAM(4, uint8_t);
	outb(port, value);
}

void Proton_IO_PortIO_OutUShort(void* pThis_, void* pParams, void* pReturnValue)
{
	uint16_t port = INTERNAL_CALL_PARAM(0, uint16_t);
	uint16_t value = INTERNAL_CALL_PARAM(4, uint16_t);
	outw(port, value);
}

void Proton_IO_PortIO_OutUInt(void* pThis_, void* pParams, void* pReturnValue)
{
	uint16_t port = INTERNAL_CALL_PARAM(0, uint16_t);
	uint32_t value = INTERNAL_CALL_PARAM(4, uint32_t);
	outl(port, value);
}

