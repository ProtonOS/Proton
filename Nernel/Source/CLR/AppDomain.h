#pragma once

typedef struct _AppDomain AppDomain;

#include <CLR/IRStructures.h>
#include <CLR/MetadataStructures.h>

struct _AppDomain
{
	uint32_t DomainIndex;

	uint32_t IRAssemblyCount;
	IRAssembly** IRAssemblies;

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
	TypeDefinition* CachedType___System_RuntimeType;
	TypeDefinition* CachedType___System_SByte;
	TypeDefinition* CachedType___System_Single;
	TypeDefinition* CachedType___System_String;
	TypeDefinition* CachedType___System_Type;
	TypeDefinition* CachedType___System_TypedReference;
	TypeDefinition* CachedType___System_UInt16;
	TypeDefinition* CachedType___System_UInt32;
	TypeDefinition* CachedType___System_UInt64;
	TypeDefinition* CachedType___System_UIntPtr;
	TypeDefinition* CachedType___System_ValueType;
	TypeDefinition* CachedType___System_Void;
};

AppDomain* AppDomain_Create();
void AppDomain_Destroy(AppDomain* pDomain);
void AppDomain_AddAssembly(AppDomain* pDomain, IRAssembly* pAssembly);
void AppDomain_LinkCorlib(CLIFile* pCorlibFile, AppDomain* pDomain);
