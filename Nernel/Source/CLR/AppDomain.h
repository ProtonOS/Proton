#pragma once

typedef struct _AppDomain AppDomain;

#include <CLR/IRStructures.h>

struct _AppDomain
{
	uint32_t DomainIndex;

	uint32_t IRAssemblyCount;
	IRAssembly** IRAssemblies;
};

AppDomain* AppDomain_Create();
void AppDomain_Destroy(AppDomain* pDomain);
void AppDomain_AddAssembly(AppDomain* pDomain, IRAssembly* pAssembly);
