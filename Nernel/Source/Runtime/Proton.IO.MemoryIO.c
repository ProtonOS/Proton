
#include "Sys.h"

#define NULL 0

#include "Proton.IO.PortIO.h"

tAsyncCall* Proton_IO_MemoryIO_InByte(PTR pThis_, PTR pParams, PTR pReturnValue)
{
	unsigned int address = INTERNALCALL_PARAM(0, unsigned int);
	*(unsigned char*)pReturnValue = *((unsigned char*)address);
	
	return NULL;
}

tAsyncCall* Proton_IO_MemoryIO_InUShort(PTR pThis_, PTR pParams, PTR pReturnValue)
{
	unsigned int address = INTERNALCALL_PARAM(0, unsigned int);
	*(unsigned short*)pReturnValue = *((unsigned short*)address);

	return NULL;
}

tAsyncCall* Proton_IO_MemoryIO_InUInt(PTR pThis_, PTR pParams, PTR pReturnValue)
{
	unsigned int address = INTERNALCALL_PARAM(0, unsigned int);
	*(unsigned int*)pReturnValue = *((unsigned int*)address);

	return NULL;
}


tAsyncCall* Proton_IO_MemoryIO_OutByte(PTR pThis_, PTR pParams, PTR pReturnValue)
{
	unsigned int address = INTERNALCALL_PARAM(0, unsigned int);
	unsigned char value = INTERNALCALL_PARAM(4, unsigned char);
    *((unsigned char*)address) = value;

	return NULL;
}

tAsyncCall* Proton_IO_MemoryIO_OutUShort(PTR pThis_, PTR pParams, PTR pReturnValue)
{
	unsigned int address = INTERNALCALL_PARAM(0, unsigned int);
	unsigned short value = INTERNALCALL_PARAM(4, unsigned short);
	*((unsigned short*)address) = value;

	return NULL;
}

tAsyncCall* Proton_IO_MemoryIO_OutUInt(PTR pThis_, PTR pParams, PTR pReturnValue)
{
	unsigned int address = INTERNALCALL_PARAM(0, unsigned int);
	unsigned int value = INTERNALCALL_PARAM(4, unsigned int);
	*((unsigned int*)address) = value;

	return NULL;
}

