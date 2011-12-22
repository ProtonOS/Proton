#pragma once

#include <CLR/CLIFile.h>
#include <CLR/AppDomain.h>

TypeDefinition* Metadata_FindType(AppDomain* domain, char* typeNamespace, char* typeName);
MethodDefinition* Metadata_FindMethodInType(TypeDefinition* type, char* methodName);