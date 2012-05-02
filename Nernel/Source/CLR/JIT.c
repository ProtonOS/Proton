#include <CLR/ILDecomposition.h>
#include <CLR/IROptimizer.h>
#include <CLR/JIT.h>


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
		char* compiledCode = calloc(1, compiledCodeLength);
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
				EMITTER(Break);
				EMITTER(Return);
				EMITTER(Load_Parameter);
				EMITTER(Load_ParameterAddress);
				EMITTER(Store_Parameter);
				EMITTER(Load_Local);
				EMITTER(Load_LocalAddress);
				EMITTER(Store_Local);
				EMITTER(Load_Null);
				EMITTER(Load_String);
				EMITTER(Load_Int32);
				EMITTER(Load_Int64);
				EMITTER(Load_Float32);
				EMITTER(Load_Float64);
				EMITTER(Dup);
				EMITTER(Pop);
				EMITTER(Load_Indirect);
				EMITTER(Store_Indirect);
				EMITTER(Add);
				EMITTER(Sub);
				EMITTER(Mul);
				EMITTER(Div);
				EMITTER(Rem);
				EMITTER(And);
				EMITTER(Or);
				EMITTER(Xor);
				EMITTER(Neg);
				EMITTER(Not);
				EMITTER(Shift);
				EMITTER(Convert_Unchecked);
				EMITTER(Convert_Checked);
				EMITTER(CastClass);
				EMITTER(IsInst);
				EMITTER(Unbox);
				EMITTER(UnboxAny);
				EMITTER(Box);
				EMITTER(Throw);
				EMITTER(Load_Object);
				EMITTER(Store_Object);
				EMITTER(Copy_Object);
				EMITTER(New_Object);
				EMITTER(Load_ArrayLength);
				EMITTER(New_Array);
				EMITTER(CheckFinite);
				EMITTER(Load_Field);
				EMITTER(Load_FieldAddress);
				EMITTER(Store_Field);
				EMITTER(Load_StaticField);
				EMITTER(Load_StaticFieldAddress);
				EMITTER(Store_StaticField);
				EMITTER(Load_Element);
				EMITTER(Load_ElementAddress);
				EMITTER(Store_Element);
				EMITTER(Allocate_Local);
				EMITTER(Initialize_Object);
				EMITTER(Copy_Block);
				EMITTER(Initialize_Block);
				EMITTER(SizeOf);
				EMITTER(Jump);
				EMITTER(Call_Virtual);
				EMITTER(Call_Constrained);
				EMITTER(Call_Absolute);
				EMITTER(Call_Internal);
				EMITTER(Branch);
				EMITTER(Load_Function);
				EMITTER(Load_VirtualFunction);
				EMITTER(Compare);
				EMITTER(Load_Token);
				EMITTER(MkRefAny);
				EMITTER(RefAnyVal);
				EMITTER(RefAnyType);
				case IROpcode_Switch:
				{
					if (((int)compiledCodeLength - (compiledCode - startOfCompiledCode) - ((uint32_t)pMethod->IRCodes[index]->Arg1 * gSizeOfPointerInBytes)) < 32)
					{
						// Trigger insufficient space
						compiledCodeLength += ((uint32_t)pMethod->IRCodes[index]->Arg1 * gSizeOfPointerInBytes);
						compiledCode = startOfCompiledCode + compiledCodeLength;
					}
					else
					{
						Log_WriteLine(LOGLEVEL__JIT_Emit, "Emitting Switch @ 0x%x", (unsigned int)compiledCode);
						compiledCode = JIT_Emit_Switch(compiledCode, pMethod, pMethod->IRCodes[index], branchRegistry);
					}
					break;
				}
				default:
					printf("Missing emitter for opcode 0x%x\n", pMethod->IRCodes[index]->Opcode);
					break;
			}
			if (((int)compiledCodeLength - (compiledCode - startOfCompiledCode)) < 32)
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

		branchRegistry->InstructionLocations[branchRegistry->InstructionCount + 1] = (size_t)compiledCode;
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
