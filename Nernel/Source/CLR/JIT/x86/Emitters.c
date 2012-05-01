#include <CLR/JIT.h>
#include <CLR/JIT/x86/x86-codegen.h>

void JIT_BranchLinker(BranchRegistry* pBranchRegistry)
{
	for (uint32_t index = 0; index < pBranchRegistry->InstructionCount; ++index)
	{
		if (pBranchRegistry->BranchLocations[index])
		{
			Log_WriteLine(LOGLEVEL__JIT_Link, "Linking %sBranch from 0x%x to 0x%x", pBranchRegistry->SpecialBranches[index] ? "Special " : "", (unsigned int)pBranchRegistry->BranchLocations[index], (unsigned int)pBranchRegistry->InstructionLocations[pBranchRegistry->TargetLocations[index]]);
			if (pBranchRegistry->SpecialBranches[index])
			{
				*(size_t*)pBranchRegistry->BranchLocations[index] = pBranchRegistry->InstructionLocations[pBranchRegistry->TargetLocations[index]];
			}
			else
			{
				x86_patch((unsigned char*)pBranchRegistry->BranchLocations[index], (unsigned char*)pBranchRegistry->InstructionLocations[pBranchRegistry->TargetLocations[index]]);
			}
		}
	}
}

char* JIT_Emit_Prologue(char* pCompiledCode, IRMethod* pMethod)
{
	return pCompiledCode;
}

char* JIT_Emit_Epilogue(char* pCompiledCode, IRMethod* pMethod)
{
	return pCompiledCode;
}

char* JIT_Emit_Nop(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	x86_nop(pCompiledCode);
	return pCompiledCode;
}
