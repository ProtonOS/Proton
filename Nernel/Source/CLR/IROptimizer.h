#pragma once

typedef struct _IRCodeNode IRCodeNode;
typedef struct _IRPhi IRPhi;

#include <CLR/IRStructures.h>

struct _IRCodeNode
{
	uint32_t Index;
	uint32_t* Instructions;
	uint32_t InstructionsCount;

	IRCodeNode** Parents;
	uint32_t ParentsCount;
	IRCodeNode** Children;
	uint32_t ChildrenCount;

	IRCodeNode* Dominator;

	IRCodeNode** DestinationFrontiers;
	uint32_t DestinationFrontiersCount;
	IRCodeNode** SourceFrontiers;
	uint32_t SourceFrontiersCount;

	IRLocalVariable** FinalIterations;
	uint32_t FinalIterationsCount;
	IRPhi** PhiFunctions;
	uint32_t PhiFunctionsCount;
};

struct _IRPhi
{
	IRLocalVariable* Result;
	IRLocalVariable** Arguments;
	uint32_t ArgumentsCount;
};

void IROptimizer_Optimize(IRMethod* pMethod);

IRCodeNode* IRCodeNode_Create();
void IRCodeNode_Destroy(IRCodeNode* pCodeNode);
uint32_t IRCodeNode_AddInstruction(IRCodeNode* pCodeNode, uint32_t pInstructionIndex);
uint32_t IRCodeNode_AddParent(IRCodeNode* pCodeNode, IRCodeNode* pParentNode);
uint32_t IRCodeNode_AddChild(IRCodeNode* pCodeNode, IRCodeNode* pChildNode);
void IRCodeNode_AddRelationship(IRCodeNode* pParentNode, IRCodeNode* pChildNode);
bool_t IRCodeNode_TrimIfContainsInstruction(IRCodeNode* pCodeNode, uint32_t pInstructionIndex);
uint32_t IRCodeNode_AddDestinationFrontier(IRCodeNode* pCodeNode, IRCodeNode* pDestinationNode);
uint32_t IRCodeNode_AddSourceFrontier(IRCodeNode* pCodeNode, IRCodeNode* pSourceNode);
void IRCodeNode_AddFrontier(IRCodeNode* pSourceNode, IRCodeNode* pDestinationNode);
void IRCodeNode_AddPhi(IRCodeNode* pCodeNode, IRPhi* pPhi);

IRPhi* IRPhi_Create(IRLocalVariable* pResult);
void IRPhi_Destroy(IRPhi* pPhi);
void IRPhi_AddArgument(IRPhi* pPhi, IRLocalVariable* pArgument);
