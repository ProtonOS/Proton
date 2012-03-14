#include <stdio.h>
#include <stdlib.h>

#include <CLR/ReferenceTypeObject.h>
#include <CLR/GC.h>

void ReferenceTypeObject_AddReference(ReferenceTypeObject* pParent, ReferenceTypeObject* pReferenced)
{
    if (!pParent || !pReferenced) return;

    ++pReferenced->ReferenceCount;

    if (!pParent->DependancyPool)
    {
        pParent->DependancyPoolSize = 1;
        pParent->DependancyPoolCount = 1;
        pParent->DependancyPool = (ReferenceTypeObject**)calloc(pParent->DependancyPoolSize, sizeof(ReferenceTypeObject*));
        pParent->DependancyPool[0] = pReferenced;
    }
    else if (pParent->DependancyPoolCount < pParent->DependancyPoolSize)
    {
        for (uint32_t index = 0; index < pParent->DependancyPoolSize; ++index)
        {
            if (!pParent->DependancyPool[index])
            {
                ++pParent->DependancyPoolCount;
                pParent->DependancyPool[index] = pReferenced;
                break;
            }
        }
    }
    else
    {
        pParent->DependancyPoolSize <<= 1;
        pParent->DependancyPool = (ReferenceTypeObject**)realloc(pParent->DependancyPool, sizeof(ReferenceTypeObject*) * pParent->DependancyPoolSize);
        for (uint32_t objectIndex = pParent->DependancyPoolSize >> 1; objectIndex < pParent->DependancyPoolSize; ++objectIndex)
            pParent->DependancyPool[objectIndex] = NULL;
        ++pParent->DependancyPoolCount;
        pParent->DependancyPool[pParent->DependancyPoolSize >> 1] = pReferenced;
    }
}

void ReferenceTypeObject_RemoveReference(ReferenceTypeObject* pParent, ReferenceTypeObject* pReferenced)
{
    if (!pParent->DependancyPool || !pReferenced) return;
	bool_t found = FALSE;
    for (uint32_t index = 0; !found && index < pParent->DependancyPoolSize; ++index)
    {
        if (pParent->DependancyPool[index] == pReferenced)
        {
		    --pReferenced->ReferenceCount;
            --pParent->DependancyPoolCount;
            pParent->DependancyPool[index] = NULL;
			found = TRUE;
        }
    }
	if (!found) Panic("Attempting to remove reference to object which isn't in dependancy pool!");
    if (pReferenced->ReferenceCount == 0)
	{
        pReferenced->Flags |= ReferenceTypeObject_Flags_Disposing;
		if (pReferenced->Stack->DisposingTop) pReferenced->DisposingNext = pReferenced->Stack->DisposingTop;
		pReferenced->Stack->DisposingTop = pReferenced;
	}
}

void ReferenceTypeObject_Dispose(ReferenceTypeObject* pObject)
{
    // TODO: Call object finalizer callback here
    pObject->Flags = (pObject->Flags & (~ReferenceTypeObject_Flags_Disposing)) | ReferenceTypeObject_Flags_Disposed;
    pObject->Stack->Allocated -= pObject->Size;
    pObject->Stack->Disposed += pObject->Size;

    ReferenceTypeObject* dependancy = NULL;
    for (uint32_t dependancyIndex = 0; pObject->DependancyPoolCount > 0 && dependancyIndex < pObject->DependancyPoolSize; ++dependancyIndex)
    {
        dependancy = pObject->DependancyPool[dependancyIndex];
        ReferenceTypeObject_RemoveReference(pObject, dependancy);
        if ((dependancy->Flags & ReferenceTypeObject_Flags_Disposing) != 0)
            ReferenceTypeObject_Dispose(dependancy);
    }
}

void ReferenceTypeObject_Reset(ReferenceTypeObject* pObject)
{
    pObject->Flags = 0;
    pObject->Size = 0;
    pObject->Object = NULL;
    pObject->Stack = NULL;
    pObject->Age = 0;
    pObject->ReferenceCount = 0;
    pObject->DependancyPoolSize = 0;
    pObject->DependancyPoolCount = 0;
    if (pObject->DependancyPool)
        free(pObject->DependancyPool);
    pObject->DependancyPool = NULL;
	pObject->DisposingNext = NULL;
}
