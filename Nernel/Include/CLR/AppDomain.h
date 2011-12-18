#pragma once

#include <CLR/IRStructures.h>

typedef struct _AppDomain AppDomain;


struct _AppDomain
{
	uint32_t ILAssemblyCount;
	ILAssembly** ILAssemblies;
};


AppDomain* AppDomain_CreateDomain();
void AppDomain_Destroy(AppDomain* domain);


void AppDomain_AddAssembly(AppDomain* domain, ILAssembly* assembly);