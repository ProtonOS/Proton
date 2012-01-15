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
            IRAssembly* asmb = ILReader_CreateAssembly(cliFile);
            Log_WriteLine(LogFlags_AppDomain_Loading, "Method Count: %u\n", (unsigned int)asmb->MethodCount);
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
	for (uint32_t i = 0; i < domain->IRAssemblyCount; i++)
	{
		IRAssembly_Destroy(domain->IRAssemblies[i]);
	}
	free(domain->IRAssemblies);
	free(domain);
}

void AppDomain_AddAssembly(AppDomain* domain, IRAssembly* assembly)
{
	// The index is 0 based, count is 1 based.
	assembly->AssemblyIndex = domain->IRAssemblyCount;
	domain->IRAssemblyCount++;
	// The reason this is safe to do even if
	// we haven't allocated for it yet, is because
	// realloc acts like a normal malloc if the first
	// argument is NULL.
	domain->IRAssemblies = (IRAssembly**)realloc(domain->IRAssemblies, sizeof(IRAssembly*) * domain->IRAssemblyCount);
	domain->IRAssemblies[domain->IRAssemblyCount - 1] = assembly;
}