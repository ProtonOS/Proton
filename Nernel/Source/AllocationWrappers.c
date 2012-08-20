
#ifdef _WIN32
#undef calloc
#undef malloc
#undef free
#define __getreent() NULL
#endif

//#define TableOfAllocations

#ifdef TableOfAllocations

typedef struct AllocationEntry
{
	void* AllocationAddress;
	//size_t AllocationSize;
} AllocationEntry;

#define TableSize 0x1000000
AllocationEntry global_TableOfAllocations[TableSize];
uint32_t global_MaxAllocationEntry = 0;
#endif

void* calloc(size_t n, size_t bytes)
{
	void* t = _calloc_r(__getreent(), n, bytes);
#ifdef TableOfAllocations
	bool_t foundOne = FALSE;
	for(uint32_t i = 0; i < global_MaxAllocationEntry; i++)
	{
		if (global_TableOfAllocations[i].AllocationAddress == t)
			Panic("Duplicate memory allocated before it was freed!");
		if (global_TableOfAllocations[i].AllocationAddress == NULL)
		{
			foundOne = TRUE;
			global_TableOfAllocations[i].AllocationAddress = t;
		}
	}
	if (!foundOne)
	{
		global_TableOfAllocations[global_MaxAllocationEntry].AllocationAddress = t;
		global_MaxAllocationEntry++;
		if (global_MaxAllocationEntry >= TableSize)
			Panic("Geese we've made a lot of allocation!");
	}
#endif
	if (!t)
		Panic("There's an issue with calloc!");
	return t;
}
void* malloc(size_t bytes)
{
	void* t = _malloc_r(__getreent(), bytes);
#ifdef TableOfAllocations
	bool_t foundOne = FALSE;
	for(uint32_t i = 0; i < global_MaxAllocationEntry; i++)
	{
		if (global_TableOfAllocations[i].AllocationAddress == t)
			Panic("Duplicate memory allocated before it was freed!");
		if (global_TableOfAllocations[i].AllocationAddress == NULL)
		{
			foundOne = TRUE;
			global_TableOfAllocations[i].AllocationAddress = t;
		}
	}
	if (!foundOne)
	{
		global_TableOfAllocations[global_MaxAllocationEntry].AllocationAddress = t;
		global_MaxAllocationEntry++;
		if (global_MaxAllocationEntry >= TableSize)
			Panic("Geese we've made a lot of allocation!");
	}
#endif
	if (!t)
		Panic("There's an issue with malloc!");
	return t;
}
void free(void* objPtr)
{
	if (!objPtr)
		Panic("Tried to free a null pointer!");
#ifdef TableOfAllocations
	bool_t found = FALSE;
	for(uint32_t i = 0; i < global_MaxAllocationEntry; i++)
	{
		if (global_TableOfAllocations[i].AllocationAddress == objPtr)
		{
			found = TRUE;
			global_TableOfAllocations[i].AllocationAddress = NULL;
		}
	}
	if (!found)
		Panic("Tried to free memory that was never allocated!");
#endif
	_free_r(__getreent(), objPtr);
}
