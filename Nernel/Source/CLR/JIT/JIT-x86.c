#include <CLR/JIT/x86/x86-codegen.h>
#include <CLR/JIT/JIT.h>
#include <CLR/Architecture.h>
#include <stdlib.h>
#include <stdio.h>

void Panic(const char* msg);

char* JIT_Emit_Prologue(IRMethod* mth, char* compMethod)
{
	x86_pushad(compMethod);
	// Now we push the Assembly & Method pair
	// so that we can back-trace later.
	x86_push_imm(compMethod, mth->ParentAssembly->AssemblyIndex);
	x86_push_imm(compMethod, mth->MethodIndex);

	uint32_t localsSize = 0;
	for (uint32_t i = 0; i < mth->LocalVariableCount; i++)
	{
		IRLocalVariable* var = mth->LocalVariables[i];
		if (var->VariableType->IsReferenceType)
		{
			localsSize += global_SizeOfPointerInBytes;
		}
		else if (var->VariableType->IsValueType)
		{
			localsSize += var->VariableType->Size;
		}
		else
		{
			Panic("Generics aren't currently supported!");
		}
	}

	// No idea why VS is complaining about an error here,
	// but there is no real error.
	// 
	// Either way, these next to op-codes set the 
	// current stack context.
	x86_mov_reg_reg(compMethod, X86_EBP, X86_ESP, global_SizeOfPointerInBytes);
	x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, (unsigned int)(localsSize));


	return compMethod;
}

char* JIT_Emit_Epilogue(IRMethod* mth, char* compMethod)
{
	// We don't care about the assembly
	// or method indexes anymore.
	x86_pop_reg(compMethod, X86_EAX); // Pop the Method index.
	x86_pop_reg(compMethod, X86_EAX); // Pop the Assembly index.

	// Now restore the registers state.
	x86_popad(compMethod);

	return compMethod;
}




char* JIT_Compile_Nop(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	x86_nop(compMethod);
	return compMethod;
}


char* JIT_Compile_BreakForDebugger			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Return					(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_LoadInt32_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_LoadInt64_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_LoadFloat32_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_LoadFloat64_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Branch					(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Jump						(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Store_LocalVar			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Load_LocalVar				(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Load_LocalVar_Address		(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Convert_OverflowCheck		(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Convert_Unchecked			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Load_Parameter			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Load_Parameter_Address	(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Store_Parameter			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Load_Element				(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Store_Element				(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Load_Array_Length			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Pop						(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Shift						(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Add						(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Sub						(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Mul						(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Div						(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Rem						(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_LoadIndirect				(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_StoreIndirect				(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_Call						(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_LoadNull					(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}
