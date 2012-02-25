#include <Nernel.h>
#include <CLR/Log.h>
#include <CLR/AppDomain.h>
#include <CLR/ILReader.h>

void AppDomain_LinkCorlib(CLIFile* fil, AppDomain* domain);
void AppDomainRegistry_AddDomain(AppDomain* dom);

AppDomain* AppDomain_CreateDomain()
{
	MultiBoot_LoadedModule* loadedModule = MultiBoot_GetLoadedModuleByFileName("corlib.dll");
    PEFile* peFile = PEFile_Create((uint8_t*)loadedModule->Address, loadedModule->Length);
    if (peFile)
    {
        CLIFile* cliFile = CLIFile_Create(peFile);
        if (cliFile)
        {
			AppDomain* domain = (AppDomain*)calloc(1, sizeof(AppDomain));
			AppDomainRegistry_AddDomain(domain);
			domain->GarbageCollector = GC_Create(domain);
			domain->RootObject = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
			AppDomain_LinkCorlib(cliFile, domain);
			Log_WriteLine(LogFlags_AppDomain_Loading, "Domain address: 0x%x", (unsigned int)domain);
            IRAssembly* asmb = ILReader_CreateAssembly(cliFile, domain);
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
	GC_Destroy(domain->GarbageCollector);
	free(domain->RootObject);
	free(domain);
}

void AppDomain_AddAssembly(AppDomain* domain, IRAssembly* assembly)
{
	// The index is 0 based, count is 1 based.
	assembly->AssemblyIndex = domain->IRAssemblyCount;
	assembly->ParentDomain = domain;
	domain->IRAssemblyCount++;
	// The reason this is safe to do even if
	// we haven't allocated for it yet, is because
	// realloc acts like a normal malloc if the first
	// argument is NULL.
	domain->IRAssemblies = (IRAssembly**)realloc(domain->IRAssemblies, sizeof(IRAssembly*) * domain->IRAssemblyCount);
	domain->IRAssemblies[domain->IRAssemblyCount - 1] = assembly;
}

void AppDomain_LinkCorlib(CLIFile* corlib, AppDomain* domain)
{
	for (uint32_t i = 1; i <= corlib->TypeDefinitionCount; i++)
	{
		TypeDefinition* tp = &(corlib->TypeDefinitions[i]);
		if (!strcmp(tp->Namespace, "System"))
		{
			if (!strcmp(tp->Name, "Array"))
			{
				domain->CachedType___System_Array = tp;
			}
			else if (!strcmp(tp->Name, "Boolean"))
			{
				domain->CachedType___System_Boolean = tp;
			}
			else if (!strcmp(tp->Name, "Byte"))
			{
				domain->CachedType___System_Byte = tp;
			}
			else if (!strcmp(tp->Name, "Char"))
			{
				domain->CachedType___System_Char = tp;
			}
			else if (!strcmp(tp->Name, "Double"))
			{
				domain->CachedType___System_Double = tp;
			}
			else if (!strcmp(tp->Name, "Enum"))
			{
				domain->CachedType___System_Enum = tp;
			}
			else if (!strcmp(tp->Name, "Int16"))
			{
				domain->CachedType___System_Int16 = tp;
			}
			else if (!strcmp(tp->Name, "Int32"))
			{
				domain->CachedType___System_Int32 = tp;
			}
			else if (!strcmp(tp->Name, "Int64"))
			{
				domain->CachedType___System_Int64 = tp;
			}
			else if (!strcmp(tp->Name, "IntPtr"))
			{
				domain->CachedType___System_IntPtr = tp;
			}
			else if (!strcmp(tp->Name, "Object"))
			{
				domain->CachedType___System_Object = tp;
			}
			else if (!strcmp(tp->Name, "SByte"))
			{
				domain->CachedType___System_SByte = tp;
			}
			else if (!strcmp(tp->Name, "Single"))
			{
				domain->CachedType___System_Single = tp;
			}
			else if (!strcmp(tp->Name, "String"))
			{
				domain->CachedType___System_String = tp;
			}
			else if (!strcmp(tp->Name, "Type"))
			{
				domain->CachedType___System_Type = tp;
			}
			else if (!strcmp(tp->Name, "TypedReference"))
			{
				domain->CachedType___System_TypedReference = tp;
			}
			else if (!strcmp(tp->Name, "UInt16"))
			{
				domain->CachedType___System_UInt16 = tp;
			}
			else if (!strcmp(tp->Name, "UInt32"))
			{
				domain->CachedType___System_UInt32 = tp;
			}
			else if (!strcmp(tp->Name, "UInt64"))
			{
				domain->CachedType___System_UInt64 = tp;
			}
			else if (!strcmp(tp->Name, "UIntPtr"))
			{
				domain->CachedType___System_UIntPtr = tp;
			}
			else if (!strcmp(tp->Name, "ValueType"))
			{
				domain->CachedType___System_ValueType = tp;
			}
			else if (!strcmp(tp->Name, "Void"))
			{
				domain->CachedType___System_Void = tp;
			}
		}
	}
}

#include <String_Format.h>
AppDomain** DomainRegistry;
uint32_t lastDomainIndex = 0;

AppDomain* AppDomainRegistry_GetDomain(uint32_t domNumber)
{
	if (domNumber >= lastDomainIndex)
		Panic(String_Format("Domain (%i) requested that doesn't exist!", (int)domNumber));
	return DomainRegistry[domNumber];
}

void AppDomainRegistry_AddDomain(AppDomain* dom)
{
	if (!DomainRegistry)
	{
		DomainRegistry = (AppDomain**)calloc(1, sizeof(AppDomain*));
		DomainRegistry[0] = dom;
		dom->DomainIndex = 0;
		lastDomainIndex++;
	}
	else
	{
		DomainRegistry = (AppDomain**)realloc(DomainRegistry, sizeof(AppDomain*) * (lastDomainIndex + 1));
		DomainRegistry[lastDomainIndex + 1] = dom;
		dom->DomainIndex = lastDomainIndex + 1;
		lastDomainIndex++;
	}

}