#include <CLR/ILDecomposition.h>
#include <CLR/IROptimizer.h>
#include <CLR/JIT.h>

const uint32_t gSizeOfPointerInBytes = sizeof(void*);

uint32_t JIT_StackAlign(uint32_t pSize)
{
	uint32_t remainder = pSize % gSizeOfPointerInBytes;
	if (remainder)
	{
		pSize += gSizeOfPointerInBytes - remainder;
	}
	return pSize;
}

uint32_t JIT_GetStackSizeOfType(IRType* pType)
{
	if (pType->IsValueType)
	{
		if (pType->Size) return pType->Size;
		if (pType->TypeDefinition->ClassLayout)
		{
			pType->Size = pType->TypeDefinition->ClassLayout->ClassSize;
			return pType->Size;
		}
		AppDomain* domain = pType->ParentAssembly->ParentDomain;
		if (pType->TypeDefinition == domain->CachedType___System_Byte ||
			pType->TypeDefinition == domain->CachedType___System_SByte ||
			pType->TypeDefinition == domain->CachedType___System_Boolean)
		{
			pType->Size = 1;
			return 1;
		}
		if (pType->TypeDefinition == domain->CachedType___System_Int16 ||
			pType->TypeDefinition == domain->CachedType___System_UInt16 ||
			pType->TypeDefinition == domain->CachedType___System_Char)
		{
			pType->Size = 2;
			return 2;
		}
		if (pType->TypeDefinition == domain->CachedType___System_Int32 ||
			pType->TypeDefinition == domain->CachedType___System_UInt32 ||
			pType->TypeDefinition == domain->CachedType___System_Single)
		{
			pType->Size = 4;
			return 4;
		}
		if (pType->TypeDefinition == domain->CachedType___System_Int64 ||
			pType->TypeDefinition == domain->CachedType___System_UInt64 ||
			pType->TypeDefinition == domain->CachedType___System_Double)
		{
			pType->Size = 8;
			return 8;
		}
		if (pType->TypeDefinition == domain->CachedType___System_IntPtr ||
			pType->TypeDefinition == domain->CachedType___System_UIntPtr)
		{
			pType->Size = gSizeOfPointerInBytes;
			return gSizeOfPointerInBytes;
		}

		uint32_t totalSize = 0;
		for (uint32_t index = 0; index < pType->FieldCount; ++index)
		{
			if (pType->Fields[index]->FieldType->IsReferenceType) totalSize += gSizeOfPointerInBytes;
			else totalSize += JIT_GetStackSizeOfType(pType->Fields[index]->FieldType);
		}
		pType->Size = totalSize;
		return totalSize;
	}
	return gSizeOfPointerInBytes;
}

void JIT_CalculateParameterLayout(IRMethod* pMethod)
{
	if (!pMethod->ParametersLayedOut)
	{
		IRParameter* parameter = NULL;
		uint32_t offset = 24;
		for (uint32_t index = 0; index < pMethod->ParameterCount; ++index)
		{
			parameter = pMethod->Parameters[index];
			parameter->Offset = offset;
			parameter->Size = JIT_GetStackSizeOfType(parameter->Type);
			offset += JIT_StackAlign(parameter->Size);
			Log_WriteLine(LOGLEVEL__Compile_Layout, "Layout Parameter %u @ 0x%x, Size: 0x%x, Aligned: 0x%x", (unsigned int)index, (unsigned int)parameter->Offset, (unsigned int)parameter->Size, (unsigned int)JIT_StackAlign(parameter->Size));
		}
		pMethod->ParametersLayedOut = TRUE;
	}
}

void JIT_CalculateLocalLayout(IRMethod* pMethod)
{
	if (!pMethod->LocalsLayedOut)
	{
		IRLocalVariable* local = NULL;
		uint32_t offset = 0;
		for (uint32_t index = 0; index < pMethod->LocalVariableCount; ++index)
		{
			local = pMethod->LocalVariables[index];
			local->Size = JIT_GetStackSizeOfType(local->VariableType);
			offset += JIT_StackAlign(local->Size);
			local->Offset = offset;
			Log_WriteLine(LOGLEVEL__Compile_Layout, "Layout Local %u @ 0x%x, Size: 0x%x, Aligned: 0x%x", (unsigned int)index, (unsigned int)local->Offset, (unsigned int)local->Size, (unsigned int)JIT_StackAlign(local->Size));
		}
		pMethod->LocalsLayedOut = TRUE;
	}
}

#define EMITTER(pInstruction) \
case IROpcode_##pInstruction: \
{ Log_WriteLine(LOGLEVEL__Compile_Emit, "Emitting " #pInstruction " @ 0x%x", (unsigned int)compiledCode); \
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
	char* compiledCode = malloc(compiledCodeLength);
	char* startOfCompiledCode = compiledCode;
	Log_WriteLine(LOGLEVEL__Compile, "Started Compiling @ 0x%x, Size: 0x%x", (unsigned int)compiledCode, (unsigned int)(pMethod->IRCodesCount * 128));
	pMethod->AssembledMethod = ((void(*)())((unsigned int)compiledCode));

	BranchRegistry* branchRegistry = BranchRegistry_Create(pMethod->IRCodes[pMethod->IRCodesCount - 1]->ILLocation);

	compiledCode = JIT_Emit_Prologue(compiledCode, pMethod);

	for (uint32_t index = 0, offset = 0; index < pMethod->IRCodesCount; ++index)
	{
		branchRegistry->InstructionLocations[pMethod->IRCodes[index]->ILLocation] = (size_t)compiledCode;
		switch (pMethod->IRCodes[index]->Opcode)
		{
			EMITTER(Nop);
			default:
				printf("Missing emitter for opcode 0x%x\n", pMethod->IRCodes[index]->Opcode);
				break;
		}
		if ((compiledCodeLength - (compiledCode - startOfCompiledCode)) < 128)
		{
			offset = compiledCode - startOfCompiledCode;
			compiledCodeLength += (pMethod->IRCodesCount - index) * 128;
			startOfCompiledCode = (char*)realloc(startOfCompiledCode, compiledCodeLength);
			compiledCode = startOfCompiledCode + offset;
		}
	}

	compiledCode = JIT_Emit_Epilogue(compiledCode, pMethod);

	JIT_BranchLinker(branchRegistry);
	BranchRegistry_Destroy(branchRegistry);

	compiledCodeLength = compiledCode - startOfCompiledCode;
	startOfCompiledCode = (char*)realloc(startOfCompiledCode, compiledCodeLength);
	compiledCode = startOfCompiledCode + compiledCodeLength;
	Log_WriteLine(LOGLEVEL__Compile, "Finished Compiling @ 0x%x to 0x%x, Size: 0x%x", (unsigned int)startOfCompiledCode, (unsigned int)compiledCode, (unsigned int)compiledCodeLength);
}
