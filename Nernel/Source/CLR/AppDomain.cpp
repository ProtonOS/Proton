#include "AppDomain.h"
#include "CLIFile.h"

CLR::AppDomain::AppDomain()
{
	Multiboot::LoadedModule* loadedModule = Multiboot::GetLoadedModule("/gac/mscorlib.dll");
	if (!loadedModule) Panic("Unable to get loaded corlib module");
	CLIFile* cliFile = new CLIFile();
	if (!cliFile->Load((uint8_t*)loadedModule->Address, loadedModule->Size, "mscorlib.dll")) Panic("Unable to load corlib");
}
