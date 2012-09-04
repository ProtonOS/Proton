#include <CLR/InternalCalls/Proton.Core.Interop.h>

void Proton_Core_Interop_AddressAsObject(AppDomain* pAppDomain, void* pAddress, void** pReturnObject)
{
	*pReturnObject = pAddress;
}

size_t Proton_Core_Interop_ObjectAsAddress(AppDomain* pAppDomain, void* pObject)
{
	return (size_t)pObject;
}
