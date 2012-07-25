#pragma once

#include "Metadata.h"

namespace CLR
{
	class AppDomain;
	class CLIFile;
	class IRMethod;
	class IRField;
	class IRType;
	class IRArrayType;
	class IRPointerType;
	class IRGenericType;
	class IRGenericMethod;

	class Assembly
	{
	public:
		uint32_t AssemblyIndex;

		AppDomain* ParentDomain;
		CLIFile* ParentFile;

		IRMethod* EntryPoint;

		uint32_t MethodCount;
		IRMethod** Methods;
		uint32_t FieldCount;
		IRField** Fields;
		uint32_t TypeCount;
		IRType** Types;
	
		uint32_t StaticFieldIndex;
		uint32_t StaticFieldCount;
		IRField** StaticFields;

		IRArrayType* ArrayTypesHashTable;
		IRPointerType* PointerTypesHashTable;
		IRGenericType* GenericTypesHashTable;
		IRGenericMethod* GenericMethodsHashTable;

		Assembly(AppDomain* pAppDomain, CLIFile* pFile);
		~Assembly();

		IRType* CreateType(TypeDefinitionData* pTypeDefinition);
		void LinkType(IRType* pType);
		IRType* GetIRTypeFromSignatureType(SignatureType* pType);
		IRMethod* CreateMethod(MethodDefinitionData* pMethodDefinition);
		void ResolveGenericTypeParameters(IRType* pType);
		IRType* CreateGenericParameter(bool pIsFromParentType, uint32_t pIndex);
		void ResolveGenericMethodParameters(IRType* pType, IRMethod* pMethod);
		void ResolveGenericMethodParametersInternal(IRType* pType, IRMethod* pMethod, IRType** pResolvingType);
	};
};
