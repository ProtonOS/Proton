#pragma once


#include <CLR/IROptimizer.h>

IRType* GetIRTypeOfSourceType(SourceType sourceType, SourceData sourceData, IRMethod* pMethod);
void IROptimizer_LinearizeStack(IRMethod* pMethod);
