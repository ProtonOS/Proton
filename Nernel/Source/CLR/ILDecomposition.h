#pragma once

#include <CLR/IRStructures.h>


IRAssembly* ILDecomposition_CreateAssembly(AppDomain* pDomain, CLIFile* pFile);
void ILDecomposition_LinkType(AppDomain* pDomain, IRAssembly* pAssembly, IRType* pType);
IRMethod** ILDecomposition_GetMethodLayout(IRType* pType, TypeDefinition* pTypeDefinition);
IRField** ILDecomposition_GetFieldLayout(IRType* pType, TypeDefinition* pTypeDefinition);
ElementType ILDecomposition_GetElementTypeFromIRType(IRType* pType);
void ILDecomposition_ConvertInstructions(IRMethod* pMethod);
void ILDecomposition_BranchLinker(IRMethod* pMethod);
void* ILDecomposition_ResolveInternalCall(MethodDefinition* methodDef, CLIFile* fil);
bool_t ILDecomposition_MethodUsesGenerics(IRMethod* pMethod);
