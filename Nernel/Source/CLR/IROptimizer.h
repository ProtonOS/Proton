#pragma once

#include <CLR/IRStructures.h>

void IROptimizer_EnterSSA(IRMethod* pMethod);
void IROptimizer_LeaveSSA(IRMethod* pMethod);
