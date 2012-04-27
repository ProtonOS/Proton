#pragma once

#include <CLR/AppDomain.h>

uint32_t Proton_IO_PortIO_InByte(AppDomain* pAppDomain, uint32_t port);
uint32_t Proton_IO_PortIO_InUShort(AppDomain* pAppDomain, uint32_t port);
uint32_t Proton_IO_PortIO_InUInt(AppDomain* pAppDomain, uint32_t port);

void Proton_IO_PortIO_OutByte(AppDomain* pAppDomain, uint32_t port, uint32_t value);
void Proton_IO_PortIO_OutUShort(AppDomain* pAppDomain, uint32_t port, uint32_t value);
void Proton_IO_PortIO_OutUInt(AppDomain* pAppDomain, uint32_t port, uint32_t value);
