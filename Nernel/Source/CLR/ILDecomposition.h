#pragma once

#include <CLR/IRStructures.h>

IRAssembly* ILDecomposition_CreateAssembly(AppDomain* pDomain, CLIFile* pFile);
void ILDecomposition_LinkType(AppDomain* pDomain, IRAssembly* pAssembly, IRType* pType);
IRMethod** ILDecomposition_GetMethodLayout(IRType* pType, TypeDefinition* pTypeDefinition);
IRField** ILDecomposition_GetFieldLayout(IRType* pType, TypeDefinition* pTypeDefinition);
void ILDecomposition_ConvertInstructions(IRMethod* pMethod);
