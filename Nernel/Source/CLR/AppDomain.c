#include <CLR/AppDomain.h>
#include <CLR/ILReader.h>
#include <System/Atomics.h>
#include <System/Multiboot.h>

AppDomain** gDomainRegistry = NULL;
uint32_t gDomainRegistry_NextIndex = 0;
uint8_t gDomainRegistry_Busy = 0;

void AppDomainRegistry_AddDomain(AppDomain* pDomain)
{
	Atomic_AquireLock(&gDomainRegistry_Busy);
	gDomainRegistry = (AppDomain**)realloc(gDomainRegistry, sizeof(AppDomain*) * (gDomainRegistry_NextIndex + 1));
	gDomainRegistry[gDomainRegistry_NextIndex] = pDomain;
	pDomain->DomainIndex = gDomainRegistry_NextIndex;
	gDomainRegistry_NextIndex++;
	Atomic_ReleaseLock(&gDomainRegistry_Busy);
}



AppDomain* AppDomain_Create()
{
	LoadedModule* loadedModule = Multiboot_GetLoadedModule("/gac/corlib.dll");
	if (loadedModule)
	{
		CLIFile* cliFile = CLIFile_Create((uint8_t*)loadedModule->Address, loadedModule->Size, "corlib.dll");
		if (cliFile)
		{
			AppDomain* domain = (AppDomain*)calloc(1, sizeof(AppDomain));
			Log_WriteLine(LOGLEVEL__Memory, "Memory: AppDomain_Create @ 0x%x", (unsigned int)domain);
			AppDomainRegistry_AddDomain(domain);
			AppDomain_LinkCorlib(cliFile, domain);
			//IRAssembly* assembly = ILReader_CreateAssembly(cliFile, domain);
			ILReader_CreateAssembly(cliFile, domain);
			return domain;
		}
		else
		{
			Panic("An error occured while loading corlib!");
		}
	}
	else
	{
		Panic("An error occured while loading corlib module!");
	}
}

void AppDomain_AddAssembly(AppDomain* pDomain, IRAssembly* pAssembly)
{
	pAssembly->AssemblyIndex = pDomain->IRAssemblyCount;
	pDomain->IRAssemblyCount++;
	pDomain->IRAssemblies = (IRAssembly**)realloc(pDomain->IRAssemblies, sizeof(IRAssembly*) * pDomain->IRAssemblyCount);
	pDomain->IRAssemblies[pDomain->IRAssemblyCount - 1] = pAssembly;
}

void AppDomain_LinkCorlib(CLIFile* pCorlibFile, AppDomain* pDomain)
{
	for (uint32_t i = 1; i <= pCorlibFile->TypeDefinitionCount; i++)
	{
		TypeDefinition* tp = &(pCorlibFile->TypeDefinitions[i]);
		if (!strcmp(tp->Namespace, "System"))
		{
			if (!strcmp(tp->Name, "Array"))
			{
				pDomain->CachedType___System_Array = tp;
			}
			else if (!strcmp(tp->Name, "Boolean"))
			{
				pDomain->CachedType___System_Boolean = tp;
			}
			else if (!strcmp(tp->Name, "Byte"))
			{
				pDomain->CachedType___System_Byte = tp;
			}
			else if (!strcmp(tp->Name, "Char"))
			{
				pDomain->CachedType___System_Char = tp;
			}
			else if (!strcmp(tp->Name, "Double"))
			{
				pDomain->CachedType___System_Double = tp;
			}
			else if (!strcmp(tp->Name, "Enum"))
			{
				pDomain->CachedType___System_Enum = tp;
			}
			else if (!strcmp(tp->Name, "Int16"))
			{
				pDomain->CachedType___System_Int16 = tp;
			}
			else if (!strcmp(tp->Name, "Int32"))
			{
				pDomain->CachedType___System_Int32 = tp;
			}
			else if (!strcmp(tp->Name, "Int64"))
			{
				pDomain->CachedType___System_Int64 = tp;
			}
			else if (!strcmp(tp->Name, "IntPtr"))
			{
				pDomain->CachedType___System_IntPtr = tp;
			}
			else if (!strcmp(tp->Name, "Object"))
			{
				pDomain->CachedType___System_Object = tp;
			}
			else if (!strcmp(tp->Name, "RuntimeType"))
			{
				pDomain->CachedType___System_RuntimeType = tp;
			}
			else if (!strcmp(tp->Name, "SByte"))
			{
				pDomain->CachedType___System_SByte = tp;
			}
			else if (!strcmp(tp->Name, "Single"))
			{
				pDomain->CachedType___System_Single = tp;
			}
			else if (!strcmp(tp->Name, "String"))
			{
				pDomain->CachedType___System_String = tp;
			}
			else if (!strcmp(tp->Name, "Type"))
			{
				pDomain->CachedType___System_Type = tp;
			}
			else if (!strcmp(tp->Name, "TypedReference"))
			{
				pDomain->CachedType___System_TypedReference = tp;
			}
			else if (!strcmp(tp->Name, "UInt16"))
			{
				pDomain->CachedType___System_UInt16 = tp;
			}
			else if (!strcmp(tp->Name, "UInt32"))
			{
				pDomain->CachedType___System_UInt32 = tp;
			}
			else if (!strcmp(tp->Name, "UInt64"))
			{
				pDomain->CachedType___System_UInt64 = tp;
			}
			else if (!strcmp(tp->Name, "UIntPtr"))
			{
				pDomain->CachedType___System_UIntPtr = tp;
			}
			else if (!strcmp(tp->Name, "ValueType"))
			{
				pDomain->CachedType___System_ValueType = tp;
			}
			else if (!strcmp(tp->Name, "Void"))
			{
				pDomain->CachedType___System_Void = tp;
			}
		}
	}
}
