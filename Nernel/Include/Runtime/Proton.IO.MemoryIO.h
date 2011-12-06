#ifndef _PROTON_IO_MEMORYIO_H
#define _PROTON_IO_MEMORYIO_H

#include "Types.h"

tAsyncCall* Proton_IO_MemoryIO_InByte(PTR pThis_, PTR pParams, PTR pReturnValue);
tAsyncCall* Proton_IO_MemoryIO_InUShort(PTR pThis_, PTR pParams, PTR pReturnValue);
tAsyncCall* Proton_IO_MemoryIO_InUInt(PTR pThis_, PTR pParams, PTR pReturnValue);

tAsyncCall* Proton_IO_MemoryIO_OutByte(PTR pThis_, PTR pParams, PTR pReturnValue);
tAsyncCall* Proton_IO_MemoryIO_OutUShort(PTR pThis_, PTR pParams, PTR pReturnValue);
tAsyncCall* Proton_IO_MemoryIO_OutUInt(PTR pThis_, PTR pParams, PTR pReturnValue);

#endif