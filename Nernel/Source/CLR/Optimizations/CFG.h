#pragma once

#include <CLR/IROptimizer.h>

IRCodeNode** IROptimizer_BuildControlFlowGraph(IRMethod* pMethod, uint32_t* pNodesCount);
