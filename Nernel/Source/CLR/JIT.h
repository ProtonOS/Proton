#pragma once

#include <CLR/IRStructures.h>
#include <CLR/JIT/BranchRegistry.h>

extern const uint32_t gSizeOfPointerInBytes;

uint32_t JIT_StackAlign(uint32_t pSize);
uint32_t JIT_GetStackSizeOfType(IRType* pType);
void JIT_CalculateParameterLayout(IRMethod* pMethod);
void JIT_CalculateLocalLayout(IRMethod* pMethod);
void JIT_CompileMethod(IRMethod* pMethod);


void JIT_BranchLinker(BranchRegistry* pBranchRegistry);
char* JIT_Emit_Prologue(char* pCompiledCode, IRMethod* pMethod);
char* JIT_Emit_Epilogue(char* pCompiledCode, IRMethod* pMethod);

char* JIT_Emit_Nop(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry);
