#include <CLR/Optimizations/EnterSSA.h>

typedef struct _IRMethodNode IRMethodNode;

struct _IRMethodNode
{
	uint32_t* Instructions;
	uint32_t InstructionCount;
	IRMethodNode** Dominated;
	uint32_t DominatedCount;
	IRMethodNode** Dominates;
	uint32_t DominatesCount;
};


IRMethodNode* IRMethodNode_Create()
{
	IRMethodNode* node = (IRMethodNode*)calloc(1, sizeof(IRMethodNode));
	return node;
}

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
		if (instruction->Destination.Type == SourceType_Local)
		{
			prevLocalVariable = pMethod->LocalVariables[instruction->Destination.Data.LocalVariable.LocalVariableIndex];
			nextLocalVariable = prevLocalVariable;
			if (prevLocalVariable->Used)
			{
				nextLocalVariable = IRLocalVariable_Copy(prevLocalVariable);
				nextLocalVariable->LocalVariableIndex = pMethod->LocalVariableCount++;
				nextLocalVariable->Ancestor = pMethod->LocalVariables[instruction->Destination.Data.LocalVariable.LocalVariableIndex];

				pMethod->LocalVariables = (IRLocalVariable**)realloc(pMethod->LocalVariables, pMethod->LocalVariableCount * sizeof(IRLocalVariable*));
				pMethod->LocalVariables[nextLocalVariable->LocalVariableIndex] = nextLocalVariable;

				for (uint32_t searchIndex = index + 1; searchIndex < pMethod->IRCodesCount; ++searchIndex)
				{
					IRInstruction* searchInstruction = pMethod->IRCodes[searchIndex];
					if (searchInstruction->Source1.Type == SourceType_Local &&
						searchInstruction->Source1.Data.LocalVariable.LocalVariableIndex == instruction->Destination.Data.LocalVariable.LocalVariableIndex)
					{
						searchInstruction->Source1.Data.LocalVariable.LocalVariableIndex = nextLocalVariable->LocalVariableIndex;
					}
					else if (searchInstruction->Source2.Type == SourceType_Local &&
							 searchInstruction->Source2.Data.LocalVariable.LocalVariableIndex == instruction->Destination.Data.LocalVariable.LocalVariableIndex)
					{
						searchInstruction->Source2.Data.LocalVariable.LocalVariableIndex = nextLocalVariable->LocalVariableIndex;
					}
					else if (searchInstruction->Source3.Type == SourceType_Local &&
							 searchInstruction->Source3.Data.LocalVariable.LocalVariableIndex == instruction->Destination.Data.LocalVariable.LocalVariableIndex)
					{
						searchInstruction->Source3.Data.LocalVariable.LocalVariableIndex = nextLocalVariable->LocalVariableIndex;
					}
				}

				Log_WriteLine(LOGLEVEL__Optimize_SSA, "Iterating assignment of local %u into %u on instruction %u", (unsigned int)instruction->Destination.Data.LocalVariable.LocalVariableIndex, (unsigned int)nextLocalVariable->LocalVariableIndex, (unsigned int)index);

				instruction->Destination.Data.LocalVariable.LocalVariableIndex = nextLocalVariable->LocalVariableIndex;
			}
			nextLocalVariable->Used = TRUE;
		}
	}
	
	IRBranch** branches = NULL;
	uint32_t branchesCount = 0;
	IROptimizer_ProcessBranches(pMethod, 0, &branches, &branchesCount);
	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Found Convergences for %u Branches", (unsigned int)branchesCount);
	for (uint32_t index = 0; index < branchesCount; ++index)
	{
		IRBranch* branch = branches[index];
		Log_WriteLine(LOGLEVEL__Optimize_SSA, "    Left 0x%x/0x%x:0x%x/0x%x, Right 0x%x/0x%x:0x%x/0x%x", (unsigned int)branch->FirstLeftInstruction, (unsigned int)branch->LeftConvergence, (unsigned int)pMethod->IRCodes[branch->FirstLeftInstruction]->ILLocation, (unsigned int)pMethod->IRCodes[branch->LeftConvergence]->ILLocation, (unsigned int)branch->FirstRightInstruction, (unsigned int)branch->RightConvergence, (unsigned int)pMethod->IRCodes[branch->FirstRightInstruction]->ILLocation, (unsigned int)pMethod->IRCodes[branch->RightConvergence]->ILLocation);
	}

	// Stage 2
	// Break down sections of instructions into nodes, based on branching, AKA nodularize
	//IRMethodNode* rootNode = IRMethodNode_Create();
	//IRMethodNode* currentNode = rootNode;
	//for (uint32_t index = 0; index < pMethod->IRCodesCount; ++index)
	//{
	//	IRInstruction* instruction = pMethod->IRCodes[index];
	//	uint32_t instructionIndex = currentNode->InstructionCount++;
	//	currentNode->Instructions = (uint32_t*)realloc(currentNode->Instructions, currentNode->InstructionCount * sizeof(uint32_t));
	//	currentNode->Instructions[instructionIndex] = index;
	//	if (instruction->Opcode == IROpcode_Branch && (BranchCondition)(uint32_t)instruction->Arg1 != BranchCondition_Always)
	//	{
	//		uint32_t dominatesIndex = currentNode->DominatesCount++;
	//		currentNode->Dominates = (IRMethodNode**)realloc(currentNode->Dominates, currentNode->DominatesCount * sizeof(IRMethodNode*));
	//		currentNode->Dominates[dominatesIndex] = IRMethodNode_Create();
	//		IRMethodNode* newNode = currentNode->Dominates[dominatesIndex];
	//		uint32_t dominatedIndex = newNode->DominatedCount++;
	//		newNode->Dominated = (IRMethodNode**)realloc(newNode->Dominated, currentNode->DominatedCount * sizeof(IRMethodNode*));
	//		newNode->Dominated[dominatedIndex] = currentNode;

	//		currentNode = newNode;
	//	}
	//	else if (instruction->Opcode == IROpcode_Switch)
	//	{
	//	}
	//}

	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Finished Entering SSA for %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);
}