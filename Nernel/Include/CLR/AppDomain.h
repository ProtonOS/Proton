#pragma once

#include <CLR/IRStructures.h>



struct _AppDomain
{
	uint32_t ILAssemblyCount;
	ILAssembly* ILAssemblies;
};
