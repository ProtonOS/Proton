#include <CLR/InternalCalls/System.Runtime.CompilerServices.RuntimeHelpers.h>
#include <CLR/JIT/Layout.h>

void System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray(AppDomain* pAppDomain, void* pArray, size_t pFieldHandle)
{
	GCObject* arrayObject = *(GCObject**)((size_t)pArray - sizeof(GCObject*));
	IRField* field = (IRField*)pFieldHandle;
	memcpy(pArray, field->FieldDefinition->FieldRVA->InitialValue, arrayObject->ActualSize);
}

int32_t System_Runtime_CompilerServices_RuntimeHelpers_get_OffsetToStringData(AppDomain* pAppDomain)
{
	return 4;
}
