#include <stdio.h>
#include <stdlib.h>

#include <CLR/ReferenceTypeObject.h>
#include <CLR/GC.h>

void ReferenceTypeObject_AddReference(ReferenceTypeObject* pReference, ReferenceTypeObject* pDependancy)
{
    if (!pReference || !pDependancy) return;

    ++pDependancy->ReferenceCount;

    if (!pReference->DependancyPool)
    {
        pReference->DependancyPoolSize = 1;
        pReference->DependancyPoolCount = 1;
        pReference->DependancyPool = (ReferenceTypeObject**)calloc(pReference->DependancyPoolSize, sizeof(ReferenceTypeObject*));
        pReference->DependancyPool[0] = pDependancy;
    }
    else if (pReference->DependancyPoolCount < pReference->DependancyPoolSize)
    {
        for (uint32_t index = 0; index < pReference->DependancyPoolSize; ++index)
        {
            if (!pReference->DependancyPool[index])
            {
                ++pReference->DependancyPoolCount;
                pReference->DependancyPool[index] = pDependancy;
                break;
            }
        }
    }
    else
    {
        pReference->DependancyPoolSize <<= 1;
        pReference->DependancyPool = (ReferenceTypeObject**)realloc(pReference->DependancyPool, sizeof(ReferenceTypeObject*) * pReference->DependancyPoolSize);
        for (uint32_t objectIndex = pReference->DependancyPoolSize >> 1; objectIndex < pReference->DependancyPoolSize; ++objectIndex)
            pReference->DependancyPool[objectIndex] = NULL;
        ++pReference->DependancyPoolCount;
        pReference->DependancyPool[pReference->DependancyPoolSize >> 1] = pDependancy;
    }
}

void ReferenceTypeObject_RemoveReference(ReferenceTypeObject* pReference, ReferenceTypeObject* pDependancy)
{
    if (!pReference->DependancyPool || !pDependancy) return;

    --pDependancy->ReferenceCount;

    for (uint32_t index = 0; index < pReference->DependancyPoolSize; ++index)
    {
        if (pReference->DependancyPool[index] == pDependancy)
        {
            --pReference->DependancyPoolCount;
            pReference->DependancyPool[index] = NULL;
            break;
        }
    }

    if (pDependancy->ReferenceCount == 0)
        pDependancy->Flags |= ReferenceTypeObject_Flags_Disposing;
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
    pObject->ReferenceCount = 0;
    pObject->DependancyPoolSize = 0;
    pObject->DependancyPoolCount = 0;
    if (pObject->DependancyPool)
        free(pObject->DependancyPool);
    pObject->DependancyPool = NULL;
}
