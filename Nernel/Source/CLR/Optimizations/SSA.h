#pragma once


#include <CLR/IROptimizer.h>

void IROptimizer_EnterSSA(IRMethod* pMethod, IRCodeNode* pRootNode);
void IROptimizer_LeaveSSA(IRMethod* pMethod, IRCodeNode* pRootNode);
