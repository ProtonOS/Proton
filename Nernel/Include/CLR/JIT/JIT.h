#pragma once

#include <CLR/IRStructures.h>


void JIT_CompileMethod(IRMethod* mthd);


char* JIT_Emit_Prologue(IRMethod* mth, char* compMethod);
char* JIT_Emit_Epilogue(IRMethod* mth, char* compMethod);


char* JIT_Compile_Nop(IRInstruction* instr, char* compMethod);
//char* JIT_Compile_BreakForDebugger(IRInstruction* instr, char* compMethod);
//char* JIT_Compile_Load_Int32_Val(IRInstruction* instr, char* compMethod);
//char* JIT_Compile_Load_Int64_Val(IRInstruction* instr, char* compMethod);
//char* JIT_Compile_Load_Float32_Val(IRInstruction* instr, char* compMethod);
//char* JIT_Compile_Load_Float64_Val(IRInstruction* instr, char* compMethod);
char* JIT_Compile_Convert_Unchecked(IRInstruction* instr, char* compMethod);