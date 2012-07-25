#pragma once

#include "../Kernel/Multiboot.h"
#include "Assembly.h"
#include "CLIFile.h"
#include "IR.h"

namespace CLR
{
	class AppDomain
	{
	public:
		static AppDomain** sDomains;
		static uint32_t sDomainCount;
		static uint8_t sBusy;

		static void Register(AppDomain* pAppDomain);

		uint32_t DomainIndex;

		uint32_t AssemblyCount;
		Assembly** Assemblies;

		TypeDefinitionData* CachedType___System_Array;
		TypeDefinitionData* CachedType___System_Boolean;
		TypeDefinitionData* CachedType___System_Byte;
		TypeDefinitionData* CachedType___System_Char;
		TypeDefinitionData* CachedType___System_Double;
		TypeDefinitionData* CachedType___System_Enum;
		TypeDefinitionData* CachedType___System_Exception;
		TypeDefinitionData* CachedType___System_Int16;
		TypeDefinitionData* CachedType___System_Int32;
		TypeDefinitionData* CachedType___System_Int64;
		TypeDefinitionData* CachedType___System_IntPtr;
		TypeDefinitionData* CachedType___System_Object;
		TypeDefinitionData* CachedType___System_RuntimeFieldHandle;
		TypeDefinitionData* CachedType___System_RuntimeMethodHandle;
		TypeDefinitionData* CachedType___System_RuntimeTypeHandle;
		TypeDefinitionData* CachedType___System_SByte;
		TypeDefinitionData* CachedType___System_Single;
		TypeDefinitionData* CachedType___System_String;
		TypeDefinitionData* CachedType___System_Type;
		TypeDefinitionData* CachedType___System_TypedReference;
		TypeDefinitionData* CachedType___System_UInt16;
		TypeDefinitionData* CachedType___System_UInt32;
		TypeDefinitionData* CachedType___System_UInt64;
		TypeDefinitionData* CachedType___System_UIntPtr;
		TypeDefinitionData* CachedType___System_ValueType;
		TypeDefinitionData* CachedType___System_Void;

		AppDomain();
		~AppDomain();

		void LinkCorlib(CLIFile* pFile);
		Assembly* CreateAssembly(CLIFile* pFile);
		void AddAssembly(Assembly* pAssembly);
		bool IsStructure(TypeDefinitionData* pTypeDefinition);
		TypeDefinitionData* ResolveTypeReference(TypeReferenceData* pTypeReference);
		void ResolveMemberReference(MemberReferenceData* pMemberReference);
	};
};
