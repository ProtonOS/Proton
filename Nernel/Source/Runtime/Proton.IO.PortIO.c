
#include "Sys.h"

#define NULL 0

#include "Proton.IO.PortIO.h"
#include "PortIO.h"

tAsyncCall* Proton_IO_PortIO_InByte(PTR pThis_, PTR pParams, PTR pReturnValue)
{
	unsigned short port = INTERNALCALL_PARAM(0, unsigned short);
	*(unsigned char*)pReturnValue = inb(port);
	
	return NULL;
}

tAsyncCall* Proton_IO_PortIO_InUShort(PTR pThis_, PTR pParams, PTR pReturnValue)
{
	unsigned short port = INTERNALCALL_PARAM(0, unsigned short);
	*(unsigned short*)pReturnValue = inw(port);

	return NULL;
}

tAsyncCall* Proton_IO_PortIO_InUInt(PTR pThis_, PTR pParams, PTR pReturnValue)
{
	unsigned short port = INTERNALCALL_PARAM(0, unsigned short);
	*(unsigned int*)pReturnValue = inl(port);

	return NULL;
}


tAsyncCall* Proton_IO_PortIO_OutByte(PTR pThis_, PTR pParams, PTR pReturnValue)
{
	unsigned short port = INTERNALCALL_PARAM(0, unsigned short);
	unsigned char value = INTERNALCALL_PARAM(4, unsigned char);
	outb(port, value);

	return NULL;
}

tAsyncCall* Proton_IO_PortIO_OutUShort(PTR pThis_, PTR pParams, PTR pReturnValue)
{
	unsigned short port = INTERNALCALL_PARAM(0, unsigned short);
	unsigned short value = INTERNALCALL_PARAM(4, unsigned short);
	outw(port, value);

	return NULL;
}

tAsyncCall* Proton_IO_PortIO_OutUInt(PTR pThis_, PTR pParams, PTR pReturnValue)
{
	unsigned short port = INTERNALCALL_PARAM(0, unsigned short);
	unsigned int value = INTERNALCALL_PARAM(4, unsigned int);
	outl(port, value);

	return NULL;
}

