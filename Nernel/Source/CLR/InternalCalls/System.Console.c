#include <Core/Console.h>
#include <CLR/GC.h>
#include <CLR/InternalCalls/System.Console.h>
#include <CLR/InternalCalls/Helpers.h>
#include <stdio.h>

void System_Console_Write(AppDomain* pAppDomain, ReferenceTypeObject* pStr)
{
	GCString* stringGCHeader = (GCString*)pStr->Object;
	for (uint32_t index = 0; index < stringGCHeader->Size; index += 2) 
		Console_WriteCharacter(stringGCHeader->Data[index]);
}
