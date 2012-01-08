#pragma once

#include <CLR/IRStructures.h>


void JIT_CompileMethod(IRMethod* mthd);


void JIT_Compile_Nop(IRInstruction* instr, char* compMethod);
void JIT_Compile_BreakForDebugger(IRInstruction* instr, char* compMethod);
void JIT_Compile_Load_Int32_Val(IRInstruction* instr, char* compMethod);
void JIT_Compile_Load_Int64_Val(IRInstruction* instr, char* compMethod);
void JIT_Compile_Load_Float32_Val(IRInstruction* instr, char* compMethod);
void JIT_Compile_Load_Float64_Val(IRInstruction* instr, char* compMethod);

