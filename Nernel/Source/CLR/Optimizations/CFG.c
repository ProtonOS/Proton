#include <CLR/Optimizations/CFG.h>

bool_t IROptimizer_ExistsInDominanceTree(IRCodeNode* pDominator, IRCodeNode* pBranch)
{
	if (pBranch == pDominator) return TRUE;
	while (pBranch->Dominator != pBranch)
	{
		if (!pBranch->Dominator) return TRUE;
		pBranch = pBranch->Dominator;
		if (pBranch == pDominator) return TRUE;
	}
	return FALSE;
}

IRCodeNode** IROptimizer_BuildControlFlowGraph(IRMethod* pMethod, uint32_t* pNodesCount)
{
	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Started Building CFG for %s.%s.%s, %u instructions @ 0x%X", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name, (unsigned int)pMethod->IRCodesCount, (unsigned int)pMethod);

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
		else if (instruction->Opcode == IROpcode_Switch)
		{
			branchTargetsIndex = branchTargetsCount;
			branchTargetsCount += (uint32_t)instruction->Arg1;
			branchTargets = (uint32_t*)realloc(branchTargets, branchTargetsCount * sizeof(uint32_t));
			for (uint32_t branchIndex = 0; branchIndex < (uint32_t)instruction->Arg1; ++branchIndex)
			{
				branchTargets[branchIndex + branchTargetsIndex] = ((IRInstruction**)instruction->Arg2)[branchIndex]->IRLocation;
			}
		}
	}
	Log_WriteLine(LOGLEVEL__Optimize_CFG, "Starting new node @ 0:0x%x", (unsigned int)pMethod->IRCodes[0]->ILLocation);
	IRCodeNode** nodes = (IRCodeNode**)calloc(1, sizeof(IRCodeNode*));
	*pNodesCount = 1;
	nodes[0] = IRCodeNode_Create();
	IRCodeNode* currentNode = nodes[0];
	bool_t startNewNode = FALSE;
	bool_t addBefore = FALSE;
	bool_t lastInstruction = FALSE;
	uint32_t nodesIndex = 0;
	currentNode->Index = 0;
	for (uint32_t index = 0; index < pMethod->IRCodesCount; ++index)
	{
		instruction = pMethod->IRCodes[index];
		lastInstruction = index == pMethod->IRCodesCount - 1;
		startNewNode = instruction->Opcode == IROpcode_Branch || instruction->Opcode == IROpcode_Switch;
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
				if (!lastInstruction) Log_WriteLine(LOGLEVEL__Optimize_CFG, "Starting new node @ %u:0x%x for branch", (unsigned int)index + 1, (unsigned int)pMethod->IRCodes[index + 1]->ILLocation);
			}
			if (!addBefore || !lastInstruction)
			{
				nodesIndex = *pNodesCount;
				*pNodesCount += 1;
				nodes = (IRCodeNode**)realloc(nodes, *pNodesCount * sizeof(IRCodeNode*));
				nodes[nodesIndex] = IRCodeNode_Create();
				currentNode = nodes[nodesIndex];
				currentNode->Index = nodesIndex;
				if (!addBefore)
				{
					IRCodeNode_AddInstruction(currentNode, index);
					Log_WriteLine(LOGLEVEL__Optimize_CFG, "Starting new node @ %u:0x%x for target", (unsigned int)index, (unsigned int)instruction->ILLocation);
				}
			}
		}
		else IRCodeNode_AddInstruction(currentNode, index);
	}
	if (branchTargets) free(branchTargets);
	Log_WriteLine(LOGLEVEL__Optimize_CFG, "Created %u nodes for %s.%s.%s", (unsigned int)*pNodesCount, pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);

	uint32_t instructionIndex = 0;
	for (uint32_t index = 0; index < *pNodesCount; ++index)
	{
		currentNode = nodes[index];
		instructionIndex = currentNode->Instructions[currentNode->InstructionsCount - 1];
		instruction = pMethod->IRCodes[instructionIndex];
		if (instruction->Opcode == IROpcode_Branch)
		{
			IRCodeNode* childNode = NULL;
			uint32_t searchIndex = 0;
			for (; searchIndex < *pNodesCount; ++searchIndex)
			{
				if (nodes[searchIndex]->Instructions[0] == ((IRInstruction*)instruction->Arg2)->IRLocation)
				{
					childNode = nodes[searchIndex];
					break;
				}
			}
			if (!childNode) Panic("Unable to locate child for flow linking");
			if ((BranchCondition)(uint32_t)instruction->Arg1 != BranchCondition_Always)
			{
				IRCodeNode_AddRelationship(currentNode, nodes[index + 1]);
				Log_WriteLine(LOGLEVEL__Optimize_CFG, "Connected fallthrough parent %u to child %u", (unsigned int)index, (unsigned int)(index + 1));
			}
			IRCodeNode_AddRelationship(currentNode, childNode);
			Log_WriteLine(LOGLEVEL__Optimize_CFG, "Connected branch parent %u to child %u", (unsigned int)index, (unsigned int)searchIndex);
		}
		else if (instruction->Opcode == IROpcode_Switch)
		{
			IRCodeNode* childNode = NULL;
			uint32_t searchIndex = 0;

			IRCodeNode_AddRelationship(currentNode, nodes[index + 1]);
			Log_WriteLine(LOGLEVEL__Optimize_CFG, "Connected default parent %u to child %u", (unsigned int)index, (unsigned int)(index + 1));
			for (uint32_t searchIndex2 = 0; searchIndex2 < (uint32_t)instruction->Arg1; ++searchIndex2)
			{
				for (; searchIndex < *pNodesCount; ++searchIndex)
				{
					if (nodes[searchIndex]->Instructions[0] == ((IRInstruction**)instruction->Arg2)[searchIndex2]->IRLocation)
					{
						childNode = nodes[searchIndex];

						IRCodeNode_AddRelationship(currentNode, childNode);
						Log_WriteLine(LOGLEVEL__Optimize_CFG, "Connected case parent %u to child %u", (unsigned int)index, (unsigned int)searchIndex);
						break;
					}
				}
			}
		}
		else if (instruction->Opcode == IROpcode_Return) continue;
		else
		{
			IRCodeNode_AddRelationship(currentNode, nodes[index + 1]);
			Log_WriteLine(LOGLEVEL__Optimize_CFG, "Connected linear parent %u to child %u", (unsigned int)index, (unsigned int)(index + 1));
		}
	}

	for (uint32_t index = 0; index < *pNodesCount; ++index)
	{
		currentNode = nodes[index];
		if (currentNode->ParentsCount == 0) currentNode->Dominator = currentNode;
		else if (currentNode->ParentsCount == 1) currentNode->Dominator = currentNode->Parents[0];
		else
		{
			IRCodeNode* dominator = nodes[0];
			for (IRCodeNode* leftNode = currentNode->Parents[0]; leftNode->Dominator != leftNode; leftNode = leftNode->Dominator)
			{
				bool_t foundDominator = TRUE;
				for (uint32_t searchIndex = 1; foundDominator && searchIndex < currentNode->ParentsCount; ++searchIndex)
				{
					foundDominator = IROptimizer_ExistsInDominanceTree(leftNode, currentNode->Parents[searchIndex]);
				}
				if (foundDominator)
				{
					dominator = leftNode;
					break;
				}
			}
			currentNode->Dominator = dominator;
		}
		Log_WriteLine(LOGLEVEL__Optimize_CFG, "Node %u is dominated by node %u", (unsigned int)index, (unsigned int)currentNode->Dominator->Index);
	}

	for (uint32_t index = 0; index < *pNodesCount; ++index)
	{
		currentNode = nodes[index];
		if (currentNode->ParentsCount < 2) continue;
		for (uint32_t parentIndex = 0; parentIndex < currentNode->ParentsCount; ++parentIndex)
		{
			IRCodeNode* parentNode = currentNode->Parents[parentIndex];
			if (parentNode == currentNode->Dominator)
			{
				Log_WriteLine(LOGLEVEL__Optimize_CFG, "Destination Node %u is frontiered with Dominator Source Node %u", (unsigned int)index, (unsigned int)parentNode->Index);
				IRCodeNode_AddFrontier(parentNode, currentNode);
			}
			IRCodeNode* runnerNode = parentNode;
			while (runnerNode != currentNode->Dominator)
			{
				Log_WriteLine(LOGLEVEL__Optimize_CFG, "Destination Node %u is frontiered with Source Node %u", (unsigned int)index, (unsigned int)runnerNode->Index);
				IRCodeNode_AddFrontier(runnerNode, currentNode);
				runnerNode = currentNode->Dominator;
			}
		}
	}

	for (uint32_t index = 0; index < *pNodesCount; ++index)
	{
		currentNode = nodes[index];
		if (currentNode->SourceFrontiersCount < 2) continue;
		Log_WriteLine(LOGLEVEL__Optimize_CFG, "Node %u is a dominance frontier", (unsigned int)index);
	}

	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Finished Building CFG for %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);
	return nodes;
}