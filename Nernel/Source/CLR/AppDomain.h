#pragma once

#include "../Kernel/Multiboot.h"
#include "Assembly.h"

namespace CLR
{
	class AppDomain
	{
	public:
		uint32_t DomainIndex;

		uint32_t AssemblyCount;
		Assembly** Assemblies;

		AppDomain();
		~AppDomain();
	};
};
