#include <CLR/IROptimizer.h>
#include <CLR/Optimizations/LinearizeStack.h>
#include <CLR/Optimizations/SSA.h>

void IROptimizer_Optimize(IRMethod* pMethod)
{
	Log_WriteLine(LOGLEVEL__Optimize, "Optimizing %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);

	uint32_t* branchTargets = NULL;
	uint32_t branchTargetsCount = 0;
	uint32_t branchTargetsIndex = 0;
	IRInstruction* instruction = NULL;
	for (uint32_t index = 0; index < pMethod->IRCodesCount; ++index)
	{
		instruction = pMethod->IRCodes[index];
		if (instruction->Opcode == IROpcode_Branch)
		{
			branchTargetsIndex = branchTargetsCount++;
			branchTargets = (uint32_t*)realloc(branchTargets, branchTargetsCount * sizeof(uint32_t));
			branchTargets[branchTargetsIndex] = ((IRInstruction*)instruction->Arg2)->IRLocation;
		}
	}
	Log_WriteLine(LOGLEVEL__Optimize, "Detected %u branch targets", (unsigned int)branchTargetsCount);

	// We now have all the targets of all branching in the method,
	// these can be used to determine the beginning of new blocks
	Log_WriteLine(LOGLEVEL__Optimize, "Starting new node @ 0x0:0x%x", (unsigned int)pMethod->IRCodes[0]->ILLocation);
	IRCodeNode** nodes = (IRCodeNode**)calloc(1, sizeof(IRCodeNode*));
	uint32_t nodesCount = 1;
	nodes[0] = IRCodeNode_Create();
	IRCodeNode* currentNode = nodes[0];
	bool_t startNewNode = FALSE;
	bool_t addBefore = FALSE;
	uint32_t nodesIndex = 0;
	for (uint32_t index = 0; index < pMethod->IRCodesCount; ++index)
	{
		instruction = pMethod->IRCodes[index];
		startNewNode = instruction->Opcode == IROpcode_Branch;
		addBefore = startNewNode;
		if (!startNewNode)
		{
			for (uint32_t searchIndex = 0; searchIndex < branchTargetsCount; ++searchIndex)
			{
				if (branchTargets[searchIndex] == index)
				{
					startNewNode = TRUE;
					break;
				}
			}
		}

		if (startNewNode && currentNode->InstructionsCount > 0)
		{
			if (addBefore)
			{
				IRCodeNode_AddInstruction(currentNode, index);
				Log_WriteLine(LOGLEVEL__Optimize, "Starting new node @ 0x%x:0x%x for branch", (unsigned int)index + 1, (unsigned int)pMethod->IRCodes[index + 1]->ILLocation);
			}
			nodesIndex = nodesCount++;
			nodes = (IRCodeNode**)realloc(nodes, nodesCount * sizeof(IRCodeNode*));
			nodes[nodesIndex] = IRCodeNode_Create();
			currentNode = nodes[nodesIndex];
			if (!addBefore)
			{
				IRCodeNode_AddInstruction(currentNode, index);
				Log_WriteLine(LOGLEVEL__Optimize, "Starting new node @ 0x%x:0x%x for target", (unsigned int)index, (unsigned int)instruction->ILLocation);
			}
		}
		else IRCodeNode_AddInstruction(currentNode, index);
	}
	if (branchTargets) free(branchTargets);
	Log_WriteLine(LOGLEVEL__Optimize, "Identified %u nodes", (unsigned int)nodesCount);

	// Now we should have an array of IRCodeNode's that describe the blocks of code,
	// but they have not been linked to each other yet through dominance checks
	uint32_t instructionIndex = 0;
	for (uint32_t index = 0; index < nodesCount; ++index)
	{
		currentNode = nodes[index];
		instructionIndex = currentNode->Instructions[currentNode->InstructionsCount - 1];
		instruction = pMethod->IRCodes[instructionIndex];
		if (instruction->Opcode == IROpcode_Branch)
		{
			IRCodeNode* childNode = NULL;
			uint32_t searchIndex = 0;
			for (; searchIndex < nodesCount; ++searchIndex)
			{
				if (nodes[searchIndex]->Instructions[0] == ((IRInstruction*)instruction->Arg2)->IRLocation)
				{
					childNode = nodes[searchIndex];
					break;
				}
			}
			if (!childNode) Panic("Unable to locate child for dominator linking");
			if ((BranchCondition)(uint32_t)instruction->Arg1 != BranchCondition_Always)
			{
				IRCodeNode_AddRelationship(currentNode, nodes[index + 1]);
				Log_WriteLine(LOGLEVEL__Optimize, "Connected parent %u to child %u", (unsigned int)index, (unsigned int)(index + 1));
			}
			IRCodeNode_AddRelationship(currentNode, childNode);
			Log_WriteLine(LOGLEVEL__Optimize, "Connected parent %u to child %u", (unsigned int)index, (unsigned int)searchIndex);
		}
	}

	IROptimizer_LinearizeStack(pMethod);
	IROptimizer_EnterSSA(pMethod);

	IROptimizer_LeaveSSA(pMethod);

	for (uint32_t index = 0; index < nodesCount; ++index) IRCodeNode_Destroy(nodes[index]);
	free(nodes);
}

IRCodeNode* IRCodeNode_Create()
{
	IRCodeNode* node = (IRCodeNode*)calloc(1, sizeof(IRCodeNode));
	return node;
}

void IRCodeNode_Destroy(IRCodeNode* pCodeNode)
{
	free(pCodeNode->Children);
	free(pCodeNode->Parents);
	free(pCodeNode);
}

uint32_t IRCodeNode_AddInstruction(IRCodeNode* pCodeNode, uint32_t pInstructionIndex)
{
	uint32_t index = pCodeNode->InstructionsCount;
	pCodeNode->InstructionsCount++;
	pCodeNode->Instructions = (uint32_t*)realloc(pCodeNode->Instructions, pCodeNode->InstructionsCount * sizeof(uint32_t));
	pCodeNode->Instructions[index] = pInstructionIndex;
	return index;
}

uint32_t IRCodeNode_AddParent(IRCodeNode* pCodeNode, IRCodeNode* pParentNode)
{
	uint32_t index = pCodeNode->ParentsCount;
	pCodeNode->ParentsCount++;
	pCodeNode->Parents = (IRCodeNode**)realloc(pCodeNode->Parents, pCodeNode->ParentsCount * sizeof(IRCodeNode*));
	pCodeNode->Parents[index] = pParentNode;
	return index;
}

uint32_t IRCodeNode_AddChild(IRCodeNode* pCodeNode, IRCodeNode* pChildNode)
{
	uint32_t index = pCodeNode->ChildrenCount;
	pCodeNode->ChildrenCount++;
	pCodeNode->Children = (IRCodeNode**)realloc(pCodeNode->Children, pCodeNode->ChildrenCount * sizeof(IRCodeNode*));
	pCodeNode->Children[index] = pChildNode;
	return index;
}

void IRCodeNode_AddRelationship(IRCodeNode* pParentNode, IRCodeNode* pChildNode)
{
	IRCodeNode_AddParent(pChildNode, pParentNode);
	IRCodeNode_AddChild(pParentNode, pChildNode);
}

bool_t IRCodeNode_TrimIfContainsInstruction(IRCodeNode* pCodeNode, uint32_t pInstructionIndex)
{
	for (uint32_t index = 0; index < pCodeNode->InstructionsCount; ++index)
	{
		if (pCodeNode->Instructions[index] == pInstructionIndex)
		{
			pCodeNode->InstructionsCount = index;
			if (pCodeNode->InstructionsCount == 0 && pCodeNode->Instructions)
			{
				free(pCodeNode->Instructions);
				pCodeNode->Instructions = NULL;
			}
			else pCodeNode->Instructions = (uint32_t*)realloc(pCodeNode->Instructions, pCodeNode->InstructionsCount * sizeof(uint32_t));
			return TRUE;
		}
	}
	return FALSE;
}
