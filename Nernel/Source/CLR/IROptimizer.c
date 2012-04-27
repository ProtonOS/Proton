#include <CLR/IROptimizer.h>

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
		if (instruction->Opcode == IROpcode_Store_Local)
		{
			prevLocalVariable = pMethod->LocalVariables[(uint32_t)instruction->Arg1];
			nextLocalVariable = prevLocalVariable;
			if (prevLocalVariable->Used)
			{
				nextLocalVariable = IRLocalVariable_Copy(prevLocalVariable);
				nextLocalVariable->LocalVariableIndex = pMethod->LocalVariableCount++;
				nextLocalVariable->Ancestor = pMethod->LocalVariables[(uint32_t)instruction->Arg1];

				pMethod->LocalVariables = (IRLocalVariable**)realloc(pMethod->LocalVariables, pMethod->LocalVariableCount * sizeof(IRLocalVariable*));
				pMethod->LocalVariables[nextLocalVariable->LocalVariableIndex] = nextLocalVariable;

				for (uint32_t searchIndex = index + 1; searchIndex < pMethod->IRCodesCount; ++searchIndex)
				{
					IRInstruction* searchInstruction = pMethod->IRCodes[searchIndex];
					if ((searchInstruction->Opcode == IROpcode_Load_Local || searchInstruction->Opcode == IROpcode_Load_Local_Address) &&
						(uint32_t)searchInstruction->Arg1 == (uint32_t)instruction->Arg1)
					{
						searchInstruction->Arg1 = (uint32_t*)nextLocalVariable->LocalVariableIndex;
					}
				}

				instruction->Arg1 = (uint32_t*)nextLocalVariable->LocalVariableIndex;
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

void IROptimizer_LeaveSSA(IRMethod* pMethod)
{
	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Started Leaving SSA for %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);

	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Finished Leaving SSA for %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);
}
