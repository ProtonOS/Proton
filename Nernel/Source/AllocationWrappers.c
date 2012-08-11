
#ifndef _WIN32
void* calloc(size_t n, size_t bytes)
{
	void* t = _calloc_r(__getreent(), n, bytes);
	if (!t)
		Panic("There's an issue with calloc!");
	return t;
}
void* malloc(size_t bytes)
{
	void* t = _malloc_r(__getreent(), bytes);
	if (!t)
		Panic("There's an issue with malloc!");
	return t;
}
void free(void* objPtr)
{
	if (!objPtr)
		Panic("Tried to free a null pointer!");
	_free_r(__getreent(), objPtr);
}
#endif