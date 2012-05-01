#pragma once

#include <CLR/IRStructures.h>

uint32_t JIT_StackAlign(uint32_t pSize);
uint32_t JIT_GetStackSizeOfType(IRType* pType);
void JIT_CalculateParameterLayout(IRMethod* pMethod);
void JIT_CalculateLocalLayout(IRMethod* pMethod);
void JIT_CompileMethod(IRMethod* pMethod);

extern const uint32_t gSizeOfPointerInBytes;