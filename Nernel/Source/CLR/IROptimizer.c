#include <CLR/IROptimizer.h>

void IROptimizer_EnterSSA(IRMethod* pMethod)
{
	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Entering SSA for %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);
}

void IROptimizer_LeaveSSA(IRMethod* pMethod)
{
	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Leaving SSA for %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);
}
