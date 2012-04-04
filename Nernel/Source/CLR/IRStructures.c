#include <CLR/IRStructures.h>

IRAssembly* IRAssembly_Create(AppDomain* pDomain, CLIFile *pFile)
{
    IRAssembly* assembly = (IRAssembly*)calloc(1, sizeof(IRAssembly));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRAssembly_Create @ 0x%x", (unsigned int)assembly);
	assembly->ParentDomain = pDomain;
	assembly->ParentFile = pFile;
	AppDomain_AddAssembly(pDomain, assembly);
    return assembly;
}

void IRAssembly_Destroy(IRAssembly* pAssembly)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRAssembly_Destroy @ 0x%x", (unsigned int)pAssembly);
    free(pAssembly);
}
