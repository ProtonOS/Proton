#pragma once

typedef struct _AppDomain AppDomain;

#include <CLR/IRStructures.h>
#include <CLR/CLIFile.h>



struct _AppDomain
{
	uint32_t IRAssemblyCount;
	IRAssembly** IRAssemblies;

	// Cached Types.
	TypeDefinition* CachedType___System_Array;
	TypeDefinition* CachedType___System_Boolean;
	TypeDefinition* CachedType___System_Byte;
	TypeDefinition* CachedType___System_Char;
	TypeDefinition* CachedType___System_Double;
	TypeDefinition* CachedType___System_Enum;
	TypeDefinition* CachedType___System_Int16;
	TypeDefinition* CachedType___System_Int32;
	TypeDefinition* CachedType___System_Int64;
	TypeDefinition* CachedType___System_IntPtr;
	TypeDefinition* CachedType___System_Object;
	TypeDefinition* CachedType___System_SByte;
	TypeDefinition* CachedType___System_Single;
	TypeDefinition* CachedType___System_UInt16;
	TypeDefinition* CachedType___System_UInt32;
	TypeDefinition* CachedType___System_UInt64;
	TypeDefinition* CachedType___System_UIntPtr;
	TypeDefinition* CachedType___System_ValueType;
	TypeDefinition* CachedType___System_Void;


};


AppDomain* AppDomain_CreateDomain();
void AppDomain_Destroy(AppDomain* domain);


void AppDomain_AddAssembly(AppDomain* domain, IRAssembly* assembly);