#pragma once

#include <CLR/IRStructures.h>


void JIT_CompileMethod(IRMethod* mthd);


char* JIT_Emit_Prologue(IRMethod* mth, char* compMethod);
char* JIT_Emit_Epilogue(IRMethod* mth, char* compMethod);


char* JIT_Compile_Nop						(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_BreakForDebugger			(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Return					(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_LoadInt32_Val				(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_LoadInt64_Val				(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_LoadFloat32_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_LoadFloat64_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Branch					(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Jump						(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Store_LocalVar			(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Load_LocalVar				(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Load_LocalVar_Address		(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Convert_OverflowCheck		(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Convert_Unchecked			(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Load_Parameter			(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Load_Parameter_Address	(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Store_Parameter			(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Load_Element				(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Store_Element				(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Load_Array_Length			(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Pop						(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Shift						(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Add						(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Sub						(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Mul						(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Div						(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Rem						(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_LoadIndirect				(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_StoreIndirect				(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Call						(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_LoadNull					(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_NewObj					(IRInstruction* instr, char* compMethod, IRMethod* mth);
char* JIT_Compile_Dup						(IRInstruction* instr, char* compMethod, IRMethod* mth);

