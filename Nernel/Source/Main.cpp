#include "CLR/AppDomain.h"
#include "Kernel/Arch.h"
#include "Kernel/Multiboot.h"

extern "C" {
void Main(uint32_t pMultibootMagic, void* pMultibootHeader);
}

void Main(uint32_t pMultibootMagic, void* pMultibootHeader)
{
	Multiboot::Startup(pMultibootMagic, reinterpret_cast<Multiboot::Header*>(pMultibootHeader));
	Arch::Startup();

	while (true);
}

CLR::AppDomain* gMernelDomain = nullptr;

void Started()
{
	time_t startupTime = time(NULL);
	printf("Nernel Started @ %24.24s\n", ctime(&startupTime));
	

	gMernelDomain = new CLR::AppDomain();
	while (true) ;
}
