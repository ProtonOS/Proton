#ifndef _PROTON_IO_PORTIO_H
#define _PROTON_IO_PORTIO_H

#include "Types.h"

tAsyncCall* Proton_IO_PortIO_InByte(PTR pThis_, PTR pParams, PTR pReturnValue);
tAsyncCall* Proton_IO_PortIO_InUShort(PTR pThis_, PTR pParams, PTR pReturnValue);
tAsyncCall* Proton_IO_PortIO_InUInt(PTR pThis_, PTR pParams, PTR pReturnValue);

tAsyncCall* Proton_IO_PortIO_OutByte(PTR pThis_, PTR pParams, PTR pReturnValue);
tAsyncCall* Proton_IO_PortIO_OutUShort(PTR pThis_, PTR pParams, PTR pReturnValue);
tAsyncCall* Proton_IO_PortIO_OutUInt(PTR pThis_, PTR pParams, PTR pReturnValue);

#endif