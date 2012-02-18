#pragma once

#include <CLR/AppDomain.h>

void Proton_IO_MemoryIO_InByte(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
void Proton_IO_MemoryIO_InUShort(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
void Proton_IO_MemoryIO_InUInt(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);

void Proton_IO_MemoryIO_OutByte(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
void Proton_IO_MemoryIO_OutUShort(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
void Proton_IO_MemoryIO_OutUInt(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
