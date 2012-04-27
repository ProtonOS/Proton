#include <CLR/InternalCalls/Proton.IO.PortIO.h>
#include <System/x86/PortIO.h>

uint32_t Proton_IO_PortIO_InByte(AppDomain* pAppDomain, uint32_t port)
{
	return (uint32_t)inb(port);
}

uint32_t Proton_IO_PortIO_InUShort(AppDomain* pAppDomain, uint32_t port)
{
	return (uint32_t)inw(port);
}

uint32_t Proton_IO_PortIO_InUInt(AppDomain* pAppDomain, uint32_t port)
{
	return (uint32_t)inl(port);
}


void Proton_IO_PortIO_OutByte(AppDomain* pAppDomain, uint32_t port, uint32_t value)
{
	outb(port, (uint8_t)value);
}

void Proton_IO_PortIO_OutUShort(AppDomain* pAppDomain, uint32_t port, uint32_t value)
{
	outw(port, (uint16_t)value);
}

void Proton_IO_PortIO_OutUInt(AppDomain* pAppDomain, uint32_t port, uint32_t value)
{
	outl(port, value);
}

