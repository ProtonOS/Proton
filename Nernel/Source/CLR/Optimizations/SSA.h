#pragma once


#include <CLR/IROptimizer.h>

void IROptimizer_EnterSSA(IRMethod* pMethod);
void IROptimizer_LeaveSSA(IRMethod* pMethod);
