#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <CLR/GC.h>

GCHeapStack* GCHeapStack_Create(uint32_t pSize)
{
    GCHeapStack* stack = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));
    stack->ObjectPoolSize = 1;
    stack->ObjectPool = (ReferenceTypeObject**)calloc(stack->ObjectPoolSize, sizeof(ReferenceTypeObject*));
    stack->ObjectPool[0] = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
    stack->Size = pSize;
    stack->Available = stack->Size;
    stack->Bottom = (uint8_t*)calloc(1, stack->Size);
    stack->Top = stack->Bottom;
    return stack;
}

void GCHeapStack_Destroy(GCHeapStack* pGCHeapStack)
{
    for (uint32_t index = 0; index < pGCHeapStack->ObjectPoolSize; ++index)
    {
        free (pGCHeapStack->Bottom);
        if (pGCHeapStack->ObjectPool[index]->DependancyPool)
            free(pGCHeapStack->ObjectPool[index]->DependancyPool);
        free (pGCHeapStack->ObjectPool[index]);
    }
    free (pGCHeapStack->ObjectPool);
    free(pGCHeapStack);
}

GC* GC_Create()
{
    GC* gc = (GC*)calloc(1, sizeof(GC));
    gc->SmallGeneration0Heap.StackCount = 1;
    gc->SmallGeneration1Heap.StackCount = 1;
    gc->SmallGeneration2Heap.StackCount = 1;
    gc->LargeHeap.StackCount = 1;

    gc->SmallGeneration0Heap.Stacks = (GCHeapStack**)calloc(gc->SmallGeneration0Heap.StackCount, sizeof(GCHeapStack*));
    gc->SmallGeneration1Heap.Stacks = (GCHeapStack**)calloc(gc->SmallGeneration1Heap.StackCount, sizeof(GCHeapStack*));
    gc->SmallGeneration2Heap.Stacks = (GCHeapStack**)calloc(gc->SmallGeneration2Heap.StackCount, sizeof(GCHeapStack*));
    gc->LargeHeap.Stacks = (GCHeapStack**)calloc(gc->LargeHeap.StackCount, sizeof(GCHeapStack*));

    gc->SmallGeneration0Heap.Stacks[0] = GCHeapStack_Create(GCHeapStack_SmallHeap_Size);
    gc->SmallGeneration1Heap.Stacks[0] = GCHeapStack_Create(GCHeapStack_SmallHeap_Size);
    gc->SmallGeneration2Heap.Stacks[0] = GCHeapStack_Create(GCHeapStack_SmallHeap_Size);
    gc->LargeHeap.Stacks[0] = GCHeapStack_Create(GCHeapStack_LargeHeap_Size);

    return gc;
}

void GC_Destroy(GC* pGC)
{
    for (uint32_t index = 0; index < pGC->LargeHeap.StackCount; ++index)
        GCHeapStack_Destroy(pGC->LargeHeap.Stacks[index]);
    for (uint32_t index = 0; index < pGC->SmallGeneration2Heap.StackCount; ++index)
        GCHeapStack_Destroy(pGC->SmallGeneration2Heap.Stacks[index]);
    for (uint32_t index = 0; index < pGC->SmallGeneration1Heap.StackCount; ++index)
        GCHeapStack_Destroy(pGC->SmallGeneration1Heap.Stacks[index]);
    for (uint32_t index = 0; index < pGC->SmallGeneration0Heap.StackCount; ++index)
        GCHeapStack_Destroy(pGC->SmallGeneration0Heap.Stacks[index]);
    free (pGC->LargeHeap.Stacks);
    free (pGC->SmallGeneration2Heap.Stacks);
    free (pGC->SmallGeneration1Heap.Stacks);
    free (pGC->SmallGeneration0Heap.Stacks);
    free(pGC);
}

ReferenceTypeObject* GCHeap_Allocate(GCHeap* pGCHeap, uint32_t pStackSize, uint32_t pSize)
{
    ReferenceTypeObject* object = NULL;
    GCHeapStack* stack = NULL;
    for (uint32_t index = 0; !object && index < pGCHeap->StackCount; ++index)
    {
        stack = pGCHeap->Stacks[index];
        if (stack->Available >= pSize)
        {
            for (uint32_t objectIndex = 0; !object && objectIndex < stack->ObjectPoolSize; ++objectIndex)
                if (!stack->ObjectPool[objectIndex]->Object)
                    object = stack->ObjectPool[objectIndex];
            if (!object)
            {
                stack->ObjectPoolSize <<= 1;
                stack->ObjectPool = (ReferenceTypeObject**)realloc(stack->ObjectPool, sizeof(ReferenceTypeObject*) * stack->ObjectPoolSize);
                for (uint32_t objectIndex = stack->ObjectPoolSize >> 1; objectIndex < stack->ObjectPoolSize; ++objectIndex)
                    stack->ObjectPool[objectIndex] = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
                object = stack->ObjectPool[stack->ObjectPoolSize >> 1];
            }
            object->Flags = ReferenceTypeObject_Flags_Allocated;
            object->Size = pSize;
            object->Object = stack->Top;
            object->Stack = stack;
            stack->Available -= pSize;
            stack->Allocated += pSize;
            stack->Top += pSize;
        }
    }
    if (!object)
    {
        ++pGCHeap->StackCount;
        pGCHeap->Stacks = (GCHeapStack**)realloc(pGCHeap->Stacks, sizeof(GCHeapStack*) * pGCHeap->StackCount);
        pGCHeap->Stacks[pGCHeap->StackCount - 1] = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));
        stack = pGCHeap->Stacks[pGCHeap->StackCount - 1];
        stack->ObjectPoolSize = 1;
        stack->ObjectPool = (ReferenceTypeObject**)calloc(stack->ObjectPoolSize, sizeof(ReferenceTypeObject*));
        stack->ObjectPool[0] = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
        stack->Size = pStackSize;
        stack->Available = stack->Size;
        stack->Bottom = (uint8_t*)calloc(1, stack->Size);
        stack->Top = stack->Bottom;

        object = stack->ObjectPool[0];
        object->Flags = ReferenceTypeObject_Flags_Allocated;
        object->Size = pSize;
        object->Object = stack->Top;
        object->Stack = stack;
        stack->Available -= pSize;
        stack->Allocated += pSize;
        stack->Top += pSize;
    }
    return object;
}

ReferenceTypeObject* GC_AllocateObject(GC* pGC, ReferenceTypeObject* pInitialReference, uint32_t pSize)
{
    if (!pInitialReference) Panic("GC_AllocateObject pInitialReference == NULL");
    if (pSize == 0 || pSize >= 0x7FFFFFFF) Panic("GC_AllocateObject pSize == 0 || pSize >= 0x7FFFFFFF");
    ReferenceTypeObject* object = NULL;
    if (pSize <= GCHeapStack_SmallHeap_Size)
        object = GCHeap_Allocate(&pGC->SmallGeneration0Heap, GCHeapStack_SmallHeap_Size, pSize);
    else if (pSize <= GCHeapStack_LargeHeap_Size)
        object = GCHeap_Allocate(&pGC->LargeHeap, GCHeapStack_LargeHeap_Size, pSize);
    else object = GCHeap_Allocate(&pGC->LargeHeap, pSize, pSize);
    ReferenceTypeObject_AddReference(pInitialReference, object);
    return object;
}

ReferenceTypeObject* GC_AllocateString(GC* pGC, ReferenceTypeObject* pInitialReference, uint8_t* pData, uint32_t pSize)
{
    if (!pInitialReference) Panic("GC_AllocateString pInitialReference == NULL");
    if (pSize == 0 || pSize >= 0x7FFFFFFF) Panic("GC_AllocateString pSize == 0 || pSize >= 0x7FFFFFFF");
    ReferenceTypeObject* object = NULL;
	uint32_t sizeOfGCString = sizeof(GCString) + pSize;
	GCString* header = NULL;
	HASH_FIND(HashHandle, pGC->StringHashTable, pData, pSize, header);
	if (!header)
	{
		if (sizeOfGCString <= GCHeapStack_SmallHeap_Size)
			object = GCHeap_Allocate(&pGC->SmallGeneration0Heap, GCHeapStack_SmallHeap_Size, sizeOfGCString);
		else if (sizeOfGCString <= GCHeapStack_LargeHeap_Size)
			object = GCHeap_Allocate(&pGC->LargeHeap, GCHeapStack_LargeHeap_Size, sizeOfGCString);
		else object = GCHeap_Allocate(&pGC->LargeHeap, sizeOfGCString, sizeOfGCString);
		object->Flags |= ReferenceTypeObject_Flags_String;
		header = (GCString*)object->Object;
		header->Data = (object->Object + sizeof(GCString));
		header->Size = pSize;
		header->Object = object;
		memcpy(header->Data, pData, pSize);
		HASH_ADD(HashHandle, pGC->StringHashTable, Data, pSize, header);
	}
	else object = header->Object;
	ReferenceTypeObject_AddReference(pInitialReference, object);
    return object;
}

ReferenceTypeObject* GC_ConcatenateStrings(GC* pGC, ReferenceTypeObject* pInitialReference, ReferenceTypeObject* pString1, ReferenceTypeObject* pString2)
{
	GCString* string1 = (GCString*)pString1->Object;
	GCString* string2 = (GCString*)pString2->Object;
	ReferenceTypeObject* object = NULL;
	uint32_t sizeOfGCString = sizeof(GCString) + string1->Size + string2->Size;
	if (sizeOfGCString <= GCHeapStack_SmallHeap_Size)
		object = GCHeap_Allocate(&pGC->SmallGeneration0Heap, GCHeapStack_SmallHeap_Size, sizeOfGCString);
	else if (sizeOfGCString <= GCHeapStack_LargeHeap_Size)
		object = GCHeap_Allocate(&pGC->LargeHeap, GCHeapStack_LargeHeap_Size, sizeOfGCString);
	else object = GCHeap_Allocate(&pGC->LargeHeap, sizeOfGCString, sizeOfGCString);

	object->Flags |= ReferenceTypeObject_Flags_String;
	GCString* header = (GCString*)object->Object;
	header->Data = (object->Object + sizeof(GCString));
	header->Size = string1->Size + string2->Size;
	header->Object = object;
	memcpy(header->Data, string1->Data, string1->Size);
	memcpy(header->Data + string1->Size, string2->Data, string2->Size);

	GCString* searchHeader = NULL;
	HASH_FIND(HashHandle, pGC->StringHashTable, header->Data, string1->Size + string2->Size, searchHeader);
	if (!searchHeader)
	{
		HASH_ADD(HashHandle, pGC->StringHashTable, Data, string1->Size + string2->Size, header);
	}
	else
	{
		object->Flags ^= ReferenceTypeObject_Flags_String;
		object->Flags &= ReferenceTypeObject_Flags_Disposing;
		object = searchHeader->Object;
	}
	ReferenceTypeObject_AddReference(pInitialReference, object);
    return object;
}

ReferenceTypeObject* GC_SubstituteString(GC* pGC, ReferenceTypeObject* pInitialReference, ReferenceTypeObject* pSource, ReferenceTypeObject* pPattern, ReferenceTypeObject* pSubstitute)
{
	GCString* source = (GCString*)pSource->Object;
	GCString* pattern = (GCString*)pPattern->Object;
	GCString* substitute = (GCString*)pSubstitute->Object;
	ReferenceTypeObject* object = NULL;
	GCString* header = NULL;
	if (pattern->Size == substitute->Size)
	{
		uint32_t sizeOfGCString = sizeof(GCString) + source->Size;
		if (sizeOfGCString <= GCHeapStack_SmallHeap_Size)
			object = GCHeap_Allocate(&pGC->SmallGeneration0Heap, GCHeapStack_SmallHeap_Size, sizeOfGCString);
		else if (sizeOfGCString <= GCHeapStack_LargeHeap_Size)
			object = GCHeap_Allocate(&pGC->LargeHeap, GCHeapStack_LargeHeap_Size, sizeOfGCString);
		else object = GCHeap_Allocate(&pGC->LargeHeap, sizeOfGCString, sizeOfGCString);

		object->Flags |= ReferenceTypeObject_Flags_String;
		header = (GCString*)object->Object;
		header->Data = (object->Object + sizeof(GCString));
		header->Size = source->Size;
		header->Object = object;
		memcpy(header->Data, source->Data, source->Size);

		uint32_t eos = 0;
		if (object->Size > pattern->Size) eos = object->Size - pattern->Size;
		for (uint32_t index = 0; index < eos; ++index)
		{
			if (!memcmp(header->Data + index, pattern->Data, pattern->Size))
			{
				memcpy(header->Data + index, substitute->Data, substitute->Size);
			}
		}
	}
	else
	{
		// Pattern & Substitute sizes are different
		uint32_t sizeOfWorkspace = source->Size;
		uint8_t* workspace = (uint8_t*)calloc(1, sizeOfWorkspace);
		uint8_t* tempspace = (uint8_t*)calloc(1, sizeOfWorkspace);
		memcpy(workspace, source->Data, sizeOfWorkspace);
		for (uint32_t index = 0; index < sizeOfWorkspace; ++index)
		{
			if (!memcmp(workspace + index, pattern->Data, pattern->Size))
			{
				uint32_t remainder = sizeOfWorkspace - index;
				if (substitute->Size > pattern->Size)
				{
					uint32_t difference = substitute->Size - pattern->Size;
					sizeOfWorkspace += difference;
					workspace = (uint8_t*)realloc(workspace, sizeOfWorkspace);
					tempspace = (uint8_t*)realloc(tempspace, sizeOfWorkspace);
					memcpy(tempspace + index + substitute->Size, workspace + index, remainder);
					memcpy(workspace + index, substitute->Data, substitute->Size);
					memcpy(workspace + index + substitute->Size, tempspace + index + substitute->Size, remainder);
				}
				else
				{
					uint32_t difference = pattern->Size - substitute->Size;
					sizeOfWorkspace -= difference;
					workspace = (uint8_t*)realloc(workspace, sizeOfWorkspace);
					tempspace = (uint8_t*)realloc(tempspace, sizeOfWorkspace);
					memcpy(workspace + index, substitute->Data, substitute->Size);
					memcpy(workspace + index + substitute->Size, workspace + index + pattern->Size, remainder);
				}
			}
		}
		free(tempspace);

		uint32_t sizeOfGCString = sizeof(GCString) + sizeOfWorkspace;
		if (sizeOfGCString <= GCHeapStack_SmallHeap_Size)
			object = GCHeap_Allocate(&pGC->SmallGeneration0Heap, GCHeapStack_SmallHeap_Size, sizeOfGCString);
		else if (sizeOfGCString <= GCHeapStack_LargeHeap_Size)
			object = GCHeap_Allocate(&pGC->LargeHeap, GCHeapStack_LargeHeap_Size, sizeOfGCString);
		else object = GCHeap_Allocate(&pGC->LargeHeap, sizeOfGCString, sizeOfGCString);

		object->Flags |= ReferenceTypeObject_Flags_String;
		header = (GCString*)object->Object;
		header->Data = (object->Object + sizeof(GCString));
		header->Size = sizeOfWorkspace;
		header->Object = object;
		memcpy(header->Data, workspace, sizeOfWorkspace);
	}
	GCString* searchHeader = NULL;
	HASH_FIND(HashHandle, pGC->StringHashTable, header->Data, header->Size, searchHeader);
	if (!searchHeader)
	{
		HASH_ADD(HashHandle, pGC->StringHashTable, Data, header->Size, header);
	}
	else
	{
		object->Flags ^= ReferenceTypeObject_Flags_String;
		object->Flags &= ReferenceTypeObject_Flags_Disposing;
		object = searchHeader->Object;
	}
	ReferenceTypeObject_AddReference(pInitialReference, object);
    return object;
}


void GCHeap_Collect(GCHeap* pGCHeap, bool_t pAgeObjects)
{
    ReferenceTypeObject* object = NULL;
    GCHeapStack* stack = NULL;
    for (uint32_t index = 0; index < pGCHeap->StackCount; ++index)
    {
        stack = pGCHeap->Stacks[index];
        for (uint32_t objectIndex = 0; objectIndex < stack->ObjectPoolSize; ++objectIndex)
        {
            if ((stack->ObjectPool[objectIndex]->Flags & ReferenceTypeObject_Flags_Allocated) != 0)
            {
                if (pAgeObjects) ++stack->ObjectPool[objectIndex]->Age;
                if ((stack->ObjectPool[objectIndex]->Flags & ReferenceTypeObject_Flags_Disposing) != 0)
                {
                    object = stack->ObjectPool[objectIndex];
                    ReferenceTypeObject_Dispose(object);
                }
            }
        }
    }
}

void GCHeap_Compact(GC* pGC, GCHeap* pGCHeap)
{
    GCHeapStack* stack = NULL;
	// Let's check all the stacks to see if they should be compacted
    for (uint32_t index = 0; index < pGCHeap->StackCount; ++index)
    {
        stack = pGCHeap->Stacks[index];
		// Let's compact after at least 25% of the stack space is disposed
        if (stack->Disposed >= (stack->Size / 4))
        {
			// First we need to go through all the objects in the pool, and swap spots when there is gaps in the
			// allocated objects, and count the total allocated objects in the process, unallocated objects are
			// only preallocated placeholders, they do not represent any form of a valid object yet, and have no
			// internal stack pointers, so they are completely ignored and shifted to the end of the pool, and
			// disposed objects are still considered allocated and valid until they are taken care of here
            uint32_t allocatedCount = 0;
            for (uint32_t objectIndex = 0, startIndex = 0; objectIndex < stack->ObjectPoolSize; ++objectIndex)
            {
				// Let's check for allocated objects, that's what we're interested in
                if ((stack->ObjectPool[objectIndex]->Flags & ReferenceTypeObject_Flags_Allocated) != 0)
                {
					// We found one, so we'll increment our total allocated object count
                    ++allocatedCount;
					// Now let's look for the first unallocated object before this object, we use the startIndex
					// to cache the objects we've already checked that are allocated
                    for (uint32_t emptyIndex = startIndex; emptyIndex < objectIndex; ++emptyIndex)
                    {
						// If we find an unallocated object before the current object, then we need to swap places,
						// unallocated objects have no value, and thus no need to worry about internal object data,
						// this simply means somehow there was internal stack corruption and an object has somehow
						// been disposed, and released, without getting shifted to the end of the pool, it may be
						// a valid situation that can occur, so we handle it by simply swapping them to the end
                        if ((stack->ObjectPool[emptyIndex]->Flags & ReferenceTypeObject_Flags_Allocated) == 0)
                        {
							// Found an unallocated object before the current allocated object, so let's swap the
							// main object pointers around in the object pool, and then our current object index
							// reflects the unallocated object
                            ReferenceTypeObject* tempObject = stack->ObjectPool[objectIndex];
                            stack->ObjectPool[objectIndex] = stack->ObjectPool[emptyIndex];
                            stack->ObjectPool[emptyIndex] = tempObject;
							// We set startIndex to avoid checking for unallocated objects on objects we already
							// checked, this time through, and if this occurs, then every allocated object after 
							// this will get moved down through the same process, so we don't need to handle that
                            startIndex = emptyIndex + 1;
                        }
                    }
                }
            }
			// At this point all the allocated objects in the pool, are sequential without unallocated gaps, but
			// they are not in order of their internal stack pointers, so we sort them to make things easier, we
			// do have a count as to how many total allocated objects there are to speed things up now
            for (uint32_t objectIndex = 0; objectIndex < allocatedCount; ++objectIndex)
            {
				// While the internal object pointer on the stack is less than the object before, swap the main
				// object pointers of the 2 allocated objects so they are in order based on the internal stack,
				// then decrement index and continue repeating until sorted to correct order, this ensures objects
				// are always sorted based on their order on the stack
				uint32_t tempIndex = objectIndex;
				while (tempIndex > 0 && stack->ObjectPool[objectIndex]->Object < stack->ObjectPool[objectIndex - 1]->Object)
                {
					// Simply swap the main object pointers, and we've sorted these 2 objects
                    ReferenceTypeObject* tempObject = stack->ObjectPool[objectIndex];
                    stack->ObjectPool[objectIndex] = stack->ObjectPool[objectIndex - 1];
                    stack->ObjectPool[objectIndex - 1] = tempObject;
					// Now decrement to check previously ordered
					--tempIndex;
                }
            }
			// At this point all the allocated objects in the pool should now be sequential and sorted based on
			// their internal object pointer to the stack, now we need to check any of the disposed objects to see
			// if they are strings, and if they are then remove them from the hash table as they are no longer
			// referenced anywhere
            for (uint32_t objectIndex = 0; objectIndex < allocatedCount; ++objectIndex)
            {
                if ((stack->ObjectPool[objectIndex]->Flags & ReferenceTypeObject_Flags_Disposed) != 0 &&
					(stack->ObjectPool[objectIndex]->Flags & ReferenceTypeObject_Flags_String) != 0)
				{
					// Disposed and a String, so let's remove the string flag to indicate it's no longer a string
					// object, but rather just a plain old object to be handled like the rest by the GC and then
					// remove it from the hash table
					stack->ObjectPool[objectIndex]->Flags &= ~ReferenceTypeObject_Flags_String;
					HASH_DELETE(HashHandle, pGC->StringHashTable, (GCString*)stack->ObjectPool[objectIndex]->Object);
				}
			}
			// At this point all the allocated objects in the pool should now be sequential and ordered based on
			// their internal object pointer to the stack. Now we can deal with compacting the disposed objects,
			// and releasing the memory they are still holding
            for (uint32_t objectIndex = 0; objectIndex < allocatedCount; ++objectIndex)
            {
                if ((stack->ObjectPool[objectIndex]->Flags & ReferenceTypeObject_Flags_Disposed) != 0)
                {
					// Found a disposed object so first, let's see if we can optimize the compacting a bit, by
					// consollidating 2 disposed objects in a row, since they are in order anyway
                    while (objectIndex < (allocatedCount - 1) && (stack->ObjectPool[objectIndex + 1]->Flags & ReferenceTypeObject_Flags_Disposed) != 0)
                    {
						// We loop, until we don't find a disposed object after us to consollidate with, which
						// means there is no more disposed objects in this gap, it may not yet be the end of the
						// stack however, this still allows consollidation of the groups of disposed objects into
						// one larger object to deal with. So all we do is add the size of the next object to this
						// object, and reset the next object.
                        stack->ObjectPool[objectIndex]->Size += stack->ObjectPool[objectIndex + 1]->Size;
                        ReferenceTypeObject_Reset(stack->ObjectPool[objectIndex + 1]);
						
						// Since we reset the next object, the looping would stop there, so we need to shift down
						// the remaining allocated objects, which moves the object we just reset to after any
						// allocated objects that remain
                        for (uint32_t shiftIndex = objectIndex + 2; shiftIndex < allocatedCount; ++shiftIndex)
                        {
							// We started at the objectIndex + 2, the next object after the reset object, and loop upto
							// the allocated count, giving us the remaining objects to shift, so simply shift down the
							// main object pointers, which shifts up the object that was reset to the end
                            ReferenceTypeObject* tempObject = stack->ObjectPool[shiftIndex];
                            stack->ObjectPool[shiftIndex] = stack->ObjectPool[shiftIndex - 1];
                            stack->ObjectPool[shiftIndex - 1] = tempObject;
                        }
						// We reset one object and shifted it to the end, so let's decrement the allocatedCount
						// appropriately for the next loop check
                        --allocatedCount;
                    }

					// At this point all allocated objects should be sorted, and the last allocated object should be
					// one big object to deal with, however we may not be at the end of the pool yet, there may be 
					// allocated in use objects after disposed gaps, so it's possible this is just a gap, and not
					// the last object
                    if (objectIndex == (allocatedCount - 1))
                    {
						// If this is the last allocated object, then it's time to dispose of it, which is quick 
						// and easy, we just adjust the top of the stack down, remove the disposed count, and add to
						// the available count
                        stack->Top -= stack->ObjectPool[objectIndex]->Size;
                        stack->Disposed -= stack->ObjectPool[objectIndex]->Size;
                        stack->Available += stack->ObjectPool[objectIndex]->Size;
						// And then reset the object to ensure the entire state is reset properly
                        ReferenceTypeObject_Reset(stack->ObjectPool[objectIndex]);
                    }
                    else
                    {
						// If we get here, then objectIndex is disposed, but objectIndex + 1 is in use
						// We need to know if the non disposed object is a string, as there is hash table stuff to
						// deal with if so
						bool_t isString = (stack->ObjectPool[objectIndex + 1]->Flags & ReferenceTypeObject_Flags_String) != 0;
						// If the non disposed object is a string, remove it from the hash table, because the string
						// header contains a hash handle that links the address of the string data directly, and that
						// data is about to be moved to the lower disposed object stack space
						if (isString) HASH_DELETE(HashHandle, pGC->StringHashTable, (GCString*)stack->ObjectPool[objectIndex + 1]->Object);

						// Things get a little confusing here, as pointers start to get swapped around, follow this
						// carefully.
						// First, we switch the main object pointers around in the pool, objectIndex will hold the
						// non disposed main object pointer, and objectIndex + 1 will hold the disposed main object
						// pointer
                        ReferenceTypeObject* tempObject = stack->ObjectPool[objectIndex];
                        stack->ObjectPool[objectIndex] = stack->ObjectPool[objectIndex + 1];
                        stack->ObjectPool[objectIndex + 1] = tempObject;

						// The main object pointers have been switched, so let's copy the non disposed internal object
						// data to the lower disposed object data, effectively moving the data down the stack
                        memcpy(stack->ObjectPool[objectIndex + 1]->Object, stack->ObjectPool[objectIndex]->Object, stack->ObjectPool[objectIndex]->Size);

						// Now that the internal data has been copied, we need to adjust the internal object pointers,
						// we do this by simply setting the non disposed internal object pointer, to the lower disposed
						// internal object pointer we just copied to, and then we set the disposed internal object
						// pointer to begin after the non disposed object ends, both objects retain their size, only the
						// pointers need to be fixed
                        stack->ObjectPool[objectIndex]->Object = stack->ObjectPool[objectIndex + 1]->Object;
                        stack->ObjectPool[objectIndex + 1]->Object = stack->ObjectPool[objectIndex]->Object + stack->ObjectPool[objectIndex]->Size;
						// If the object was a string, let's add it back into the hash table
						if (isString) HASH_ADD(HashHandle, pGC->StringHashTable, Data, stack->ObjectPool[objectIndex]->Size, (GCString*)stack->ObjectPool[objectIndex]->Object);
                    }
                }
            }
			// At this point we are done with compacting, the stack should be optimized
        }
    }
}

void GCHeap_Migrate(GCHeap* pSourceHeap, GCHeap* pDestinationHeap, uint32_t pStackSize, uint8_t pRequiredAge)
{
    ReferenceTypeObject* object = NULL;
    GCHeapStack* stack = NULL;
    for (uint32_t index = 0; index < pSourceHeap->StackCount; ++index)
    {
        stack = pSourceHeap->Stacks[index];
        for (uint32_t objectIndex = 0; objectIndex < stack->ObjectPoolSize; ++objectIndex)
        {
            if ((stack->ObjectPool[objectIndex]->Flags & (ReferenceTypeObject_Flags_Allocated | ReferenceTypeObject_Flags_Disposed)) == ReferenceTypeObject_Flags_Allocated)
            {
                object = stack->ObjectPool[objectIndex];
                if (object->Age >= pRequiredAge)
                {
                    ReferenceTypeObject* replacement = GCHeap_Allocate(pDestinationHeap, pStackSize, object->Size);
                    memcpy(replacement->Object, object->Object, object->Size);
                    uint8_t* tempObject = replacement->Object;
                    replacement->Object = object->Object;
                    object->Object = tempObject;
                    GCHeapStack* tempStack = replacement->Stack;
                    replacement->Stack = object->Stack;
                    object->Stack = tempStack;
                    stack->ObjectPool[objectIndex] = replacement;
                    for (uint32_t searchIndex = 0; searchIndex < object->Stack->ObjectPoolSize; ++searchIndex)
                    {
                        if (object->Stack->ObjectPool[searchIndex] == replacement)
                        {
                            object->Stack->ObjectPool[searchIndex] = object;
                            break;
                        }
                    }
                    object->Age = 0;
                    ReferenceTypeObject_Dispose(replacement);
                }
            }
        }
    }
}

void GC_Collect(GC* pGC)
{
    GCHeap_Collect(&pGC->SmallGeneration0Heap, TRUE);
    GCHeap_Collect(&pGC->SmallGeneration1Heap, TRUE);
    GCHeap_Collect(&pGC->SmallGeneration2Heap, FALSE);
    GCHeap_Collect(&pGC->LargeHeap, FALSE);

    GCHeap_Migrate(&pGC->SmallGeneration1Heap, &pGC->SmallGeneration2Heap, GCHeapStack_SmallHeap_Size, GC_Generation0ToGeneration1_RequiredAge);
    GCHeap_Migrate(&pGC->SmallGeneration0Heap, &pGC->SmallGeneration1Heap, GCHeapStack_SmallHeap_Size, GC_Generation1ToGeneration2_RequiredAge);

    GCHeap_Compact(pGC, &pGC->SmallGeneration0Heap);
    GCHeap_Compact(pGC, &pGC->SmallGeneration1Heap);
    GCHeap_Compact(pGC, &pGC->SmallGeneration2Heap);
    GCHeap_Compact(pGC, &pGC->LargeHeap);
}

