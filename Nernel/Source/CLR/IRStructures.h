#pragma once

typedef struct _IRAssembly IRAssembly;

#include <CLR/AppDomain.h>
#include <CLR/CLIFile.h>

struct _IRAssembly
{
	uint32_t AssemblyIndex;
	AppDomain* ParentDomain;
	CLIFile* ParentFile;
};

IRAssembly* IRAssembly_Create(AppDomain* pDomain, CLIFile* pFile);
void IRAssembly_Destroy(IRAssembly* pAssembly);
