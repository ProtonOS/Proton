#pragma once


#include <CLR/IROptimizer.h>

void IROptimizer_EnterSSA(IRMethod* pMethod, IRBranch* pBranches, uint32_t pBranchCount);