#pragma once

#include <CLR/IRStructures.h>

IRAssembly* ILDecomposition_CreateAssembly(AppDomain* pDomain, CLIFile* pFile);
IRMethod** ILDecomposition_GetMethodLayout(IRType* pType, TypeDefinition* pTypeDefinition);
IRField** ILDecomposition_GetFieldLayout(IRType* pType, TypeDefinition* pTypeDefinition);
