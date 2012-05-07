#include <CLR/IROptimizer.h>
#include <CLR/Optimizations/CFG.h>
#include <CLR/Optimizations/LinearizeStack.h>
#include <CLR/Optimizations/SSA.h>

void IROptimizer_Optimize(IRMethod* pMethod)
{
	Log_WriteLine(LOGLEVEL__Optimize, "Started Optimizing %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);

	uint32_t nodesCount = 0;
	IRCodeNode** nodes = IROptimizer_BuildControlFlowGraph(pMethod, &nodesCount);

	IROptimizer_LinearizeStack(pMethod);
	IROptimizer_EnterSSA(pMethod, nodes[0]);

	IROptimizer_LeaveSSA(pMethod, nodes[0]);

	for (uint32_t index = 0; index < nodesCount; ++index) IRCodeNode_Destroy(nodes[index]);
	free(nodes);

	Log_WriteLine(LOGLEVEL__Optimize, "Finished Optimizing %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);
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
