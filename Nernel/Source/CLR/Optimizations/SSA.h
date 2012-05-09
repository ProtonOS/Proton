#pragma once


#include <CLR/IROptimizer.h>

void IROptimizer_EnterSSA(IRMethod* pMethod, IRCodeNode** pNodes, uint32_t pNodesCount);
void IROptimizer_LeaveSSA(IRMethod* pMethod, IRCodeNode** pNodes, uint32_t pNodesCount);
