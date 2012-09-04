#pragma once

#include <CLR/AppDomain.h>

void Proton_Core_Interop_AddressAsObject(AppDomain* pAppDomain, void* pAddress, void** pReturnObject);
size_t Proton_Core_Interop_ObjectAsAddress(AppDomain* pAppDomain, void* pObject);
