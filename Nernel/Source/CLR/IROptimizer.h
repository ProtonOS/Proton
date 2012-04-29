#pragma once

typedef struct _IRBranch IRBranch;

#include <CLR/IRStructures.h>

struct _IRBranch
{
	uint32_t FirstLeftInstruction;
	uint32_t FirstRightInstruction;
	uint32_t LeftConvergence;
	uint32_t RightConvergence;
};

void IROptimizer_Optimize(IRMethod* pMethod);
uint32_t IROptimizer_ProcessBranches(IRMethod* pMethod, uint32_t pStartIndex, IRBranch*** pBranches, uint32_t* pBranchesCount);
