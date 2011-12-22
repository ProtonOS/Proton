#include <Nernel.h>
#include <CLR/Log.h>
#include <CLR/AppDomain.h>
#include <CLR/ILReader.h>


AppDomain* AppDomain_CreateDomain()
{
	MultiBoot_LoadedModule* loadedModule = MultiBoot_GetLoadedModuleByFileName("corlib.dll");
    PEFile* peFile = PEFile_Create((uint8_t*)loadedModule->Address, loadedModule->Length);
    if (peFile)
    {
        CLIFile* cliFile = CLIFile_Create(peFile);
        if (cliFile)
        {
			AppDomain* domain = (AppDomain*)malloc(sizeof(AppDomain));
            ILAssembly* asmb = ILReader_CreateAssembly(cliFile);
            Log_WriteLine(LogFlags_AppDomain_Loading, "Method Count: %u\n", (unsigned int)asmb->IRAssembly->MethodCount);
			AppDomain_AddAssembly(domain, asmb);
			return domain;
        }
		else
		{
			Panic("An error occured while loading corlib!");
		}
    }
	else
	{
		Panic("An error occured while loading corlib!");
	}
}

void AppDomain_Destroy(AppDomain* domain)
{
	for (uint32_t i = 0; i < domain->ILAssemblyCount; i++)
	{
		ILAssembly_Destroy(domain->ILAssemblies[i]);
	}
	free(domain->ILAssemblies);
	free(domain);
}

void AppDomain_AddAssembly(AppDomain* domain, ILAssembly* assembly)
{
	domain->ILAssemblyCount++;
	// The reason this is safe to do even if
	// we haven't allocated for it yet, is because
	// realloc acts like a normal malloc if the first
	// argument is NULL.
	domain->ILAssemblies = (ILAssembly**)realloc(domain->ILAssemblies, sizeof(ILAssembly*) * domain->ILAssemblyCount);
	domain->ILAssemblies[domain->ILAssemblyCount - 1] = assembly;

}