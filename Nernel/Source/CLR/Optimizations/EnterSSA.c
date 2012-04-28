#include <CLR/Optimizations/EnterSSA.h>

void IROptimizer_EnterSSA(IRMethod* pMethod)
{
	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Started Entering SSA for %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);

	// Stage 1: Expansion
	// Find each assignment to a local variable, and replace with assignment to new iteration of the local
	// After each rewrite, fix targets that use the previous iteration, to use the new iteration after that point
	IRLocalVariable* prevLocalVariable = NULL;
	IRLocalVariable* nextLocalVariable = NULL;
	for (uint32_t index = 0; index < pMethod->IRCodesCount; ++index)
	{
		IRInstruction* instruction = pMethod->IRCodes[index];
		if (instruction->DestinationType == SourceType_Local)
		{
			prevLocalVariable = pMethod->LocalVariables[instruction->DestinationData.LocalVariable.LocalVariableIndex];
			nextLocalVariable = prevLocalVariable;
			if (prevLocalVariable->Used)
			{
				nextLocalVariable = IRLocalVariable_Copy(prevLocalVariable);
				nextLocalVariable->LocalVariableIndex = pMethod->LocalVariableCount++;
				nextLocalVariable->Ancestor = pMethod->LocalVariables[instruction->DestinationData.LocalVariable.LocalVariableIndex];

				pMethod->LocalVariables = (IRLocalVariable**)realloc(pMethod->LocalVariables, pMethod->LocalVariableCount * sizeof(IRLocalVariable*));
				pMethod->LocalVariables[nextLocalVariable->LocalVariableIndex] = nextLocalVariable;

				for (uint32_t searchIndex = index + 1; searchIndex < pMethod->IRCodesCount; ++searchIndex)
				{
					IRInstruction* searchInstruction = pMethod->IRCodes[searchIndex];
					if (searchInstruction->Source1Type == SourceType_Local &&
						searchInstruction->Source1Data.LocalVariable.LocalVariableIndex == instruction->DestinationData.LocalVariable.LocalVariableIndex)
					{
						searchInstruction->Source1Data.LocalVariable.LocalVariableIndex = nextLocalVariable->LocalVariableIndex;
					}
					else if (searchInstruction->Source2Type == SourceType_Local &&
							 searchInstruction->Source2Data.LocalVariable.LocalVariableIndex == instruction->DestinationData.LocalVariable.LocalVariableIndex)
					{
						searchInstruction->Source2Data.LocalVariable.LocalVariableIndex = nextLocalVariable->LocalVariableIndex;
					}
					else if (searchInstruction->Source3Type == SourceType_Local &&
							 searchInstruction->Source3Data.LocalVariable.LocalVariableIndex == instruction->DestinationData.LocalVariable.LocalVariableIndex)
					{
						searchInstruction->Source3Data.LocalVariable.LocalVariableIndex = nextLocalVariable->LocalVariableIndex;
					}
				}

				Log_WriteLine(LOGLEVEL__Optimize_SSA, "Iterating assignment of local %u into %u on instruction %u", (unsigned int)instruction->DestinationData.LocalVariable.LocalVariableIndex, (unsigned int)nextLocalVariable->LocalVariableIndex, (unsigned int)index);

				instruction->DestinationData.LocalVariable.LocalVariableIndex = nextLocalVariable->LocalVariableIndex;
			}
			nextLocalVariable->Used = TRUE;
		}
	}
	// Stage 2
	// Break down sections of instructions into nodes, based on branching
	for (uint32_t index = 0; index < pMethod->IRCodesCount; ++index)
	{
		IRInstruction* instruction = pMethod->IRCodes[index];
		if (instruction->Opcode == IROpcode_Branch)
		{
		}
		else if (instruction->Opcode == IROpcode_Switch)
		{
		}
	}

	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Finished Entering SSA for %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);
}