#include <CLR/IROptimizer.h>
#include <CLR/Optimizations/CFG.h>
#include <CLR/Optimizations/DeadMoveElimination.h>
#include <CLR/Optimizations/IntrinsicSubstitution.h>
#include <CLR/Optimizations/LinearizeStack.h>
#include <CLR/Optimizations/MoveCompacting.h>
#include <CLR/Optimizations/Peephole_PreSSA.h>
#include <CLR/Optimizations/SSA.h>

void IROptimizer_Optimize(IRMethod* pMethod)
{
	Log_WriteLine(LOGLEVEL__Optimize, "Started Optimizing %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);

	uint32_t nodesCount = 0;
	IRCodeNode** nodes = IROptimizer_BuildControlFlowGraph(pMethod, &nodesCount);

	IROptimizer_LinearizeStack(pMethod);
	IROptimizer_IntrinsicSubstitution(pMethod);
	IROptimizer_Peephole_PreSSA(pMethod);
	
	uint32_t originalLocalCount = pMethod->LocalVariableCount;
	IROptimizer_EnterSSA(pMethod, nodes, nodesCount);

	//IROptimizer_MoveCompacting(pMethod, nodes, nodesCount);

	IROptimizer_LeaveSSA(pMethod, nodes, nodesCount, originalLocalCount);

	IROptimizer_DeadMoveElimination(pMethod);

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
	if (pCodeNode->PhiFunctions)
	{
		for (uint32_t index = 0; index < pCodeNode->PhiFunctionsCount; ++index)
		{
			IRPhi_Destroy(pCodeNode->PhiFunctions[index]);
		}
		free(pCodeNode->PhiFunctions);
	}
	if (pCodeNode->FinalIterations) free(pCodeNode->FinalIterations);
	if (pCodeNode->SourceFrontiers) free(pCodeNode->SourceFrontiers);
	if (pCodeNode->DestinationFrontiers) free(pCodeNode->DestinationFrontiers);
	if (pCodeNode->Children) free(pCodeNode->Children);
	if (pCodeNode->Parents) free(pCodeNode->Parents);
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

uint32_t IRCodeNode_AddDestinationFrontier(IRCodeNode* pCodeNode, IRCodeNode* pDestinationNode)
{
	uint32_t index = pCodeNode->DestinationFrontiersCount;
	pCodeNode->DestinationFrontiersCount++;
	pCodeNode->DestinationFrontiers = (IRCodeNode**)realloc(pCodeNode->DestinationFrontiers, pCodeNode->DestinationFrontiersCount * sizeof(IRCodeNode*));
	pCodeNode->DestinationFrontiers[index] = pDestinationNode;
	return index;
}

uint32_t IRCodeNode_AddSourceFrontier(IRCodeNode* pCodeNode, IRCodeNode* pSourceNode)
{
	uint32_t index = pCodeNode->SourceFrontiersCount;
	pCodeNode->SourceFrontiersCount++;
	pCodeNode->SourceFrontiers = (IRCodeNode**)realloc(pCodeNode->SourceFrontiers, pCodeNode->SourceFrontiersCount * sizeof(IRCodeNode*));
	pCodeNode->SourceFrontiers[index] = pSourceNode;
	return index;
}

void IRCodeNode_AddFrontier(IRCodeNode* pSourceNode, IRCodeNode* pDestinationNode)
{
	IRCodeNode_AddDestinationFrontier(pSourceNode, pDestinationNode);
	IRCodeNode_AddSourceFrontier(pDestinationNode, pSourceNode);
}

void IRCodeNode_AddPhi(IRCodeNode* pCodeNode, IRPhi* pPhi)
{
	uint32_t index = pCodeNode->PhiFunctionsCount;
	pCodeNode->PhiFunctionsCount++;
	pCodeNode->PhiFunctions = (IRPhi**)realloc(pCodeNode->PhiFunctions, pCodeNode->PhiFunctionsCount * sizeof(IRPhi*));
	pCodeNode->PhiFunctions[index] = pPhi;
}

IRPhi* IRPhi_Create(IRLocalVariable* pResult)
{
	IRPhi* phi = (IRPhi*)calloc(1, sizeof(IRPhi));
	phi->Result = pResult;
	return phi;
}

void IRPhi_Destroy(IRPhi* pPhi)
{
	if (pPhi->Arguments) free(pPhi->Arguments);
	free(pPhi);
}

void IRPhi_AddArgument(IRPhi* pPhi, IRLocalVariable* pArgument)
{
	uint32_t index = pPhi->ArgumentsCount;
	pPhi->ArgumentsCount++;
	pPhi->Arguments = (IRLocalVariable**)realloc(pPhi->Arguments, pPhi->ArgumentsCount * sizeof(IRLocalVariable*));
	pPhi->Arguments[index] = pArgument;
}
