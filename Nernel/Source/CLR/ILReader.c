#include <CLR/ILReader.h>

IRAssembly* ILReader_CreateAssembly(CLIFile* pFile, AppDomain* pDomain)
{
	IRAssembly* assembly = IRAssembly_Create(pDomain, pFile);
	pFile->Assembly = assembly;

	return assembly;
}
