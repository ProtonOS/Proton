#pragma once

typedef struct _IRCodeNode IRCodeNode;

#include <CLR/IRStructures.h>

struct _IRCodeNode
{
	uint32_t* Instructions;
	uint32_t InstructionsCount;

	IRCodeNode** Parents;
	uint32_t ParentsCount;
	IRCodeNode** Children;
	uint32_t ChildrenCount;

	IRCodeNode* Dominator;
	bool_t IsFrontier;
};

void IROptimizer_Optimize(IRMethod* pMethod);

IRCodeNode* IRCodeNode_Create();
void IRCodeNode_Destroy(IRCodeNode* pCodeNode);
uint32_t IRCodeNode_AddInstruction(IRCodeNode* pCodeNode, uint32_t pInstructionIndex);
uint32_t IRCodeNode_AddParent(IRCodeNode* pCodeNode, IRCodeNode* pParentNode);
uint32_t IRCodeNode_AddChild(IRCodeNode* pCodeNode, IRCodeNode* pChildNode);
void IRCodeNode_AddRelationship(IRCodeNode* pParentNode, IRCodeNode* pChildNode);
bool_t IRCodeNode_TrimIfContainsInstruction(IRCodeNode* pCodeNode, uint32_t pInstructionIndex);
