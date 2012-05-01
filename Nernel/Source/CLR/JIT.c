#include <CLR/ILDecomposition.h>
#include <CLR/IROptimizer.h>
#include <CLR/JIT.h>

const uint32_t gSizeOfPointerInBytes = sizeof(void*);


#define EMITTER(pInstruction) \
case IROpcode_##pInstruction: \
{ Log_WriteLine(LOGLEVEL__JIT_Emit, "Emitting " #pInstruction " @ 0x%x", (unsigned int)compiledCode); \
	compiledCode = JIT_Emit_##pInstruction(compiledCode, pMethod, pMethod->IRCodes[index], branchRegistry); \
	break; }

void JIT_CompileMethod(IRMethod* pMethod)
{
	if (pMethod->AssembledMethod) return;

	ILDecomposition_ConvertInstructions(pMethod);
	IROptimizer_Optimize(pMethod);

	JIT_CalculateParameterLayout(pMethod);
	JIT_CalculateLocalLayout(pMethod);

	uint32_t compiledCodeLength = pMethod->IRCodesCount * 128;
	bool_t compiled = FALSE;
	while (!compiled)
	{
		char* compiledCode = malloc(compiledCodeLength);
		char* startOfCompiledCode = compiledCode;
		Log_WriteLine(LOGLEVEL__JIT, "Started Compiling %s.%s.%s @ 0x%x, Size: 0x%x", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name, (unsigned int)compiledCode, (unsigned int)(pMethod->IRCodesCount * 128));
		pMethod->AssembledMethod = ((void(*)())((unsigned int)compiledCode));

		BranchRegistry* branchRegistry = BranchRegistry_Create(pMethod->IRCodes[pMethod->IRCodesCount - 1]->ILLocation);

		compiledCode = JIT_Emit_Prologue(compiledCode, pMethod);

		bool_t restartCompiling = FALSE;
		for (uint32_t index = 0; index < pMethod->IRCodesCount; ++index)
		{
			branchRegistry->InstructionLocations[pMethod->IRCodes[index]->ILLocation] = (size_t)compiledCode;
			switch (pMethod->IRCodes[index]->Opcode)
			{
				EMITTER(Nop);
				default:
					printf("Missing emitter for opcode 0x%x\n", pMethod->IRCodes[index]->Opcode);
					break;
			}
			if ((compiledCodeLength - (compiledCode - startOfCompiledCode)) < 32)
			{
				Log_WriteLine(LOGLEVEL__JIT, "Insufficient space to compile method, adding space and restarting");
				compiledCodeLength += (pMethod->IRCodesCount - index) * 128;
				BranchRegistry_Destroy(branchRegistry);
				free(startOfCompiledCode);
				restartCompiling = TRUE;
				break;
			}
		}
		if (restartCompiling) continue;

		compiledCode = JIT_Emit_Epilogue(compiledCode, pMethod);

		JIT_BranchLinker(branchRegistry);
		BranchRegistry_Destroy(branchRegistry);

		compiledCodeLength = compiledCode - startOfCompiledCode;
		startOfCompiledCode = (char*)realloc(startOfCompiledCode, compiledCodeLength);
		compiledCode = startOfCompiledCode + compiledCodeLength;
		Log_WriteLine(LOGLEVEL__JIT, "Finished Compiling %s.%s.%s @ 0x%x to 0x%x, Size: 0x%x", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name, (unsigned int)startOfCompiledCode, (unsigned int)compiledCode, (unsigned int)compiledCodeLength);
		compiled = TRUE;
	}
}
