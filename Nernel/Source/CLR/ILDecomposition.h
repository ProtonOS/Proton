#pragma once

typedef struct _InternalCall InternalCall;

#include <CLR/IRStructures.h>


#define MAX_INTERNAL_ARGS 8

struct _InternalCall
{
	const char* Namespace;
	const char* TypeName;
	const char* Name;
	const uint8_t ReturnType;
	const uint32_t ParameterCount;
	const uint8_t Args[MAX_INTERNAL_ARGS];
	void* TargetMethod;
};


IRAssembly* ILDecomposition_CreateAssembly(AppDomain* pDomain, CLIFile* pFile);
void ILDecomposition_LinkType(AppDomain* pDomain, IRAssembly* pAssembly, IRType* pType);
IRMethod** ILDecomposition_GetMethodLayout(IRType* pType, TypeDefinition* pTypeDefinition);
IRField** ILDecomposition_GetFieldLayout(IRType* pType, TypeDefinition* pTypeDefinition);
void ILDecomposition_ConvertInstructions(IRMethod* pMethod);
void* ILDecomposition_ResolveInternalCall(MethodDefinition* methodDef, CLIFile* fil);
