#include <CLR/JIT/x86/x86-codegen.h>
#include <CLR/JIT/JIT.h>
#include <stdlib.h>
#include <stdio.h>

char* JIT_Emit_Prologue(IRMethod* mth, char* compMethod)
{


	return compMethod;
}

char* JIT_Emit_Epilogue(IRMethod* mth, char* compMethod)
{


	return compMethod;
}


char* JIT_Compile_Nop(IRInstruction* instr, char* compMethod)
{
	x86_nop(compMethod);
	return compMethod;
}

char* JIT_Compile_Convert_Unchecked(IRInstruction* instr, char* compMethod)
{
	
	return compMethod;
}