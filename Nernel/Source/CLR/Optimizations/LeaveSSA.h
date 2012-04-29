#pragma once


#include <CLR/IROptimizer.h>

void IROptimizer_LeaveSSA(IRMethod* pMethod, IRBranch* pBranches, uint32_t pBranchCount);