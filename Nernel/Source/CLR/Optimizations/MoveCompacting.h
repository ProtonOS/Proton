#pragma once


#include <CLR/IROptimizer.h>

void IROptimizer_MoveCompacting(IRMethod* pMethod, IRCodeNode** pNodes, uint32_t pNodesCount);
