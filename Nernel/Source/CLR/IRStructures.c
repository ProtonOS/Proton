#include <CLR/IRStructures.h>

#include <stdlib.h>


void IRAssembly_Destroy(IRAssembly* assembly)
{
	for (uint32_t i = 0; i < assembly->MethodCount; i++)
	{
		free(assembly->Methods[i].AssembledMethod);
		free(assembly->Methods[i].IRCodes);
		free(assembly->Methods);
	}
}