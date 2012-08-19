#include <CLR/InternalCalls/System.Threading.Thread.h>
#include <CLR/JIT/Layout.h>

void System_Threading_Thread_CurrentInternalThread_internal(AppDomain* pAppDomain, void** pReturnObjectDestination)
{
	IRType* internalThreadType = pAppDomain->IRAssemblies[0]->Types[pAppDomain->CachedType___System_Threading_InternalThread->TableIndex - 1];
	if (!internalThreadType->SizeCalculated) JIT_GetSizeOfType(internalThreadType);

	GC_AllocateObject(pAppDomain, internalThreadType, internalThreadType->Size, pReturnObjectDestination);

	// TODO: Lots.
}
