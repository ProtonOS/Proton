#include <CLR/JIT/x86/x86-codegen.h>
#include <CLR/JIT/JIT.h>
#include <CLR/JIT/JIT-x86.h>
#include <CLR/Architecture.h>
#include <CLR/SyntheticStack.h>
#include <String_Format.h>
#include <Inline.h>
#include <stdlib.h>
#include <stdio.h>

void Panic(const char* msg);


ALWAYS_INLINE void Align(uint32_t* val)
{
	if (*val % 4 != 0)
	{
		*val += 4 - (*val % 4);
	}
}

ALWAYS_INLINE char* JIT_Emit_ParamSwap(char* compMethod, uint32_t paramCount)
{
	uint32_t swapCount = paramCount / 2;
	// If paramCount is odd, the middle 
	// param won't move at all.
	for (uint32_t index = 0; index < swapCount; ++index)
	{
		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, index * 4, 4);
		x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, (paramCount - (index + 1)) * 4, 4);
		x86_mov_membase_reg(compMethod, X86_ESP, index * 4, X86_EBX, 4);
		x86_mov_membase_reg(compMethod, X86_ESP, (paramCount - (index + 1)) * 4, X86_EAX, 4);
	}

	return compMethod;
}


ALWAYS_INLINE uint32_t CalculateStackSize(IRType* tp)
{
	if (tp->Size > 0)
		return tp->Size;

	if (tp->IsReferenceType)
		return global_SizeOfPointerInBytes;
	else if (tp->IsValueType)
	{
		if (tp->TypeDef->ClassLayout)
		{
			tp->Size = tp->TypeDef->ClassLayout->ClassSize;
			return tp->TypeDef->ClassLayout->ClassSize;
		}
		AppDomain* domain = tp->ParentAssembly->ParentDomain;
		if (
			(tp->TypeDef == domain->CachedType___System_Byte)
		||  (tp->TypeDef == domain->CachedType___System_SByte)
		||  (tp->TypeDef == domain->CachedType___System_Boolean)
		)
		{
			tp->Size = 1;
			return 1;
		}
		else if (
			(tp->TypeDef == domain->CachedType___System_Int16)
		||  (tp->TypeDef == domain->CachedType___System_UInt16)
		||  (tp->TypeDef == domain->CachedType___System_Char)
		)
		{
			tp->Size = 2;
			return 2;
		}
		else if (
			(tp->TypeDef == domain->CachedType___System_Int32)
		||  (tp->TypeDef == domain->CachedType___System_UInt32)
		||  (tp->TypeDef == domain->CachedType___System_Single)
		)
		{
			tp->Size = 4;
			return 4;
		}
		else if (
			(tp->TypeDef == domain->CachedType___System_Int64)
		||  (tp->TypeDef == domain->CachedType___System_UInt64)
		||  (tp->TypeDef == domain->CachedType___System_Double)
		)
		{
			tp->Size = 8;
			return 8;
		}
		else if (
			(tp->TypeDef == domain->CachedType___System_IntPtr)
		||  (tp->TypeDef == domain->CachedType___System_UIntPtr)
		)
		{
			tp->Size = global_SizeOfPointerInBytes;
			return global_SizeOfPointerInBytes;
		}
		else
		{
			Panic("This isn't supported yet!");
		}
	}

	

	return global_SizeOfPointerInBytes;
}

uint32_t SizeOfType(IRType* pType)
{
	if (pType->Size) return pType->Size;
	if (pType->IsValueType) return CalculateStackSize(pType);
	uint32_t size = 0;
	printf("Getting SizeOfType %u\n", (unsigned int)pType->FieldCount);
	for (uint32_t index = 0; index < pType->FieldCount; ++index)
	{
		if (pType->Fields[index]->FieldType->IsReferenceType) size += global_SizeOfPointerInBytes;
		else size += SizeOfType(pType->Fields[index]->FieldType);
	}
	pType->Size = size;
	return size;
}

void JIT_Layout_Parameters(IRMethod* pMethod)
{
	if (!pMethod->ParametersLayedOut)
	{
		IRParameter* param = NULL;
		uint32_t offset = 8;
		for (uint32_t index = 0; index < pMethod->ParameterCount; ++index)
		{
			param = pMethod->Parameters[index];
			param->Size = CalculateStackSize(param->Type);
			param->Offset = offset;
			offset += param->Size;
			Align(&offset);
		}
		pMethod->ParametersLayedOut = TRUE;
	}
}

char* JIT_Emit_Prologue(IRMethod* mth, char* compMethod)
{
	x86_push_reg(compMethod, X86_EBP);

	uint32_t localsSize = mth->LocalVariableCount * global_SizeOfPointerInBytes;
	/*for (uint32_t i = 0; i < mth->LocalVariableCount; i++)
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
	}*/
	// Either way, these next to op-codes set the 
	// current stack context.
	x86_mov_reg_reg(compMethod, X86_EBP, X86_ESP, global_SizeOfPointerInBytes);
	//// Now we push the Assembly & Method pair
	//// so that we can back-trace later.
	//x86_push_imm(compMethod, mth->ParentAssembly->AssemblyIndex);
	//x86_push_imm(compMethod, mth->MethodIndex);
	if (localsSize > 0)
	{
		x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, (unsigned int)(localsSize));
	}


	return compMethod;
}

char* JIT_Emit_Epilogue(IRMethod* mth, char* compMethod)
{

	//// // We don't care about the assembly
	//// // or method indexes anymore.
	//x86_pop_reg(compMethod, X86_EAX); // Pop the Method index.
	//x86_pop_reg(compMethod, X86_EAX); // Pop the Assembly index.
	
	if (mth->Returns)
	{
		x86_pop_reg(compMethod, X86_EAX);
	}

	x86_leave(compMethod);
	x86_ret(compMethod);

	return compMethod;
}




char* JIT_Compile_Nop(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_nop(compMethod);
	return compMethod;
}


char* JIT_Compile_BreakForDebugger			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	//x86_breakpoint(compMethod);
	return compMethod;
}


char* JIT_Compile_Return					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	if (mth->Returns)
	{
		x86_pop_reg(compMethod, X86_EAX);
	}
	x86_leave(compMethod);
	x86_ret(compMethod);
	return compMethod;
}


char* JIT_Compile_LoadInt32_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_push_imm(compMethod, *((int32_t*)instr->Arg1));
	return compMethod;
}


char* JIT_Compile_LoadInt64_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	int64_t value = *((int64_t*)instr->Arg1);
	x86_push_imm(compMethod, (value & 0xFFFFFFFF));
	x86_push_imm(compMethod, (value >> 32));
	return compMethod;
}


char* JIT_Compile_LoadFloat32_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_push_imm(compMethod, *((int32_t*)instr->Arg1));
	return compMethod;
}


char* JIT_Compile_LoadFloat64_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	int64_t value = *((int64_t*)instr->Arg1);
	x86_push_imm(compMethod, (value & 0xFFFFFFFF));
	x86_push_imm(compMethod, (value >> 32));
	return compMethod;
}

char* JIT_Compile_Branch					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	// it would be something like this:
	BranchCondition condition = *(BranchCondition*)instr->Arg1;
	IRInstruction* target = (IRInstruction*)instr->Arg2;
	if (condition == BranchCondition_Always)
	{
		if (target->InstructionLocation > instr->InstructionLocation)
		{
			unsigned char* comp = (unsigned char*)compMethod;
			x86_jump_code(comp, 0);
			BranchRegistry_RegisterBranchForLink(branchRegistry, instr->InstructionLocation, target->InstructionLocation, compMethod);
			compMethod = (char*)comp;
		}
		else
		{
			unsigned char* targLoc = (unsigned char*)BranchRegistry_GetInstructionLocation(branchRegistry, target->InstructionLocation);
			unsigned char* comp = (unsigned char*)compMethod;
			x86_jump_code(comp, targLoc);
			compMethod = (char*)comp;
		}
	}
	else if(condition == BranchCondition_False || condition == BranchCondition_True)
	{
		ElementType ArgType = *(ElementType*)instr->Arg3;
		switch(ArgType)
		{
			case ElementType_I1:
			case ElementType_I2:
			case ElementType_I4:
			case ElementType_I:
			case ElementType_U1:
			case ElementType_U2:
			case ElementType_U4:
			case ElementType_U:
			case ElementType_ManagedPointer:
			case ElementType_Ref:
				{
					x86_pop_reg(compMethod, X86_EAX);
					x86_alu_reg_imm(compMethod, X86_CMP, X86_EAX, 0);

					unsigned char* comp = (unsigned char*)compMethod;
					if (target->InstructionLocation > instr->InstructionLocation)
					{
						if (condition == BranchCondition_False)
						{
								x86_branch32(comp, X86_CC_E, 0, FALSE);
						}
						else
						{
								x86_branch32(comp, X86_CC_NE, 0, FALSE);
						}
						BranchRegistry_RegisterBranchForLink(branchRegistry, instr->InstructionLocation, target->InstructionLocation, compMethod);
					}
					else
					{
						unsigned char* targLoc = (unsigned char*)BranchRegistry_GetInstructionLocation(branchRegistry, target->InstructionLocation);
						if (condition == BranchCondition_False)
						{
								x86_branch32(comp, X86_CC_E, targLoc, FALSE);
						}
						else
						{
								x86_branch32(comp, X86_CC_NE, targLoc, FALSE);
						}
					}
					compMethod = (char*)comp;
				}
				break;
			default:
				Panic("Invalid argument type for simple branch!");
				break;
		}
	}
	else
	{
		ElementType Arg1Type = *(ElementType*)instr->Arg3;
		//ElementType Arg2Type = *(ElementType*)instr->Arg4;
		//if (Arg1Type != Arg2Type)
		//	Panic("Something went very wrong in the decomp, because this shouldn't be possible.");

		switch(Arg1Type) // Here we get to load the arguments and call cmp on them.
		{
			case ElementType_Ref:
			case ElementType_ManagedPointer:
			case ElementType_I:
			case ElementType_I4:
			{
				x86_pop_reg(compMethod, X86_EAX);
				x86_pop_reg(compMethod, X86_EBX);
				x86_alu_reg_reg(compMethod, X86_CMP, X86_EBX, X86_EAX);
				break;
			}
			case ElementType_I8:
			{
				x86_fild_membase(compMethod, X86_ESP, 0, TRUE);
				x86_fild_membase(compMethod, X86_ESP, 8, TRUE);
				x86_fcomip(compMethod, 1);
				x86_fdecstp(compMethod);
				x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 16);
				// Not fun on a 32-bit system
				break;
			}
			case ElementType_R4:
			case ElementType_R8:
			{
				x86_fcomip(compMethod, 1);
				x86_fdecstp(compMethod);
				break;
			}

			default:
				Panic("Invalid type for branch equal comparison.");
				break;
		}
		unsigned char* comp = (unsigned char*)compMethod;
		switch(condition) // the flags should now be set, so we just emit a conditional jump now.
		{
			case BranchCondition_Equal:
				x86_branch32(comp, X86_CC_E, 0, TRUE);
				break;
			case BranchCondition_Greater_Or_Equal:
				x86_branch32(comp, X86_CC_GE, 0, TRUE);
				break;
			case BranchCondition_Greater_Or_Equal_Unsigned:
				x86_branch32(comp, X86_CC_GE, 0, FALSE);
				break;
			case BranchCondition_Greater:
				x86_branch32(comp, X86_CC_GT, 0, TRUE);
				break;
			case BranchCondition_Greater_Unsigned:
				x86_branch32(comp, X86_CC_GT, 0, FALSE);
				break;
			case BranchCondition_Less_Or_Equal:
				x86_branch32(comp, X86_CC_LE, 0, TRUE);
				break;
			case BranchCondition_Less_Or_Equal_Unsigned:
				x86_branch32(comp, X86_CC_LE, 0, FALSE);
				break;
			case BranchCondition_Less:
				x86_branch32(comp, X86_CC_LT, 0, TRUE);
				break;
			case BranchCondition_Less_Unsigned:
				x86_branch32(comp, X86_CC_LT, 0, FALSE);
				break;
			case BranchCondition_NotEqual_Unsigned:
				x86_branch32(comp, X86_CC_NE, 0, FALSE);
				break;
			default:
				Panic("WTF?!?!?!"); // unknown branch condition.
				break;
		}
		BranchRegistry_RegisterBranchForLink(branchRegistry, instr->InstructionLocation, target->InstructionLocation, compMethod);
		compMethod = (char*)comp;

	}
	return compMethod;
}
char* JIT_LinkBranches						(char* compMethod, BranchRegistry* branchReg, uint32_t pLength)
{

	for (uint32_t i = 0; i < pLength; i++)
	{
		if (branchReg->BranchLocations[i] != 0)
		{
			x86_patch((unsigned char*)branchReg->BranchLocations[i], (unsigned char*)branchReg->InstructionLocations[branchReg->TargetLocations[i]]);
		}
	}

	return compMethod;
}


char* JIT_Compile_Optimized_Jump			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRInstruction* target = (IRInstruction*)instr->Arg1;
	if (target->InstructionLocation > instr->InstructionLocation)
	{
		unsigned char* comp = (unsigned char*)compMethod;
		x86_jump_code(comp, 0);
		BranchRegistry_RegisterBranchForLink(branchRegistry, instr->InstructionLocation, target->InstructionLocation, compMethod);
		compMethod = (char*)comp;
	}
	else
	{
		unsigned char* targLoc = (unsigned char*)BranchRegistry_GetInstructionLocation(branchRegistry, target->InstructionLocation);
		unsigned char* comp = (unsigned char*)compMethod;
		x86_jump_code(comp, targLoc);
		compMethod = (char*)comp;
	}
	return compMethod;
}


char* JIT_Compile_Store_LocalVar			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	uint32_t localIndex = *(uint32_t*)instr->Arg1;
	uint32_t size = global_SizeOfPointerInBytes;
	/*if (mth->LocalVariables[localIndex]->VariableType->IsValueType)
		size = mth->LocalVariables[localIndex]->VariableType->Size;*/
	uint32_t movCount = size / global_SizeOfPointerInBytes;
	if ((size % global_SizeOfPointerInBytes) != 0) ++movCount;
	for (uint32_t movIndex = 0; movIndex < movCount; ++movIndex)
	{
		x86_pop_reg(compMethod, X86_EAX);
		x86_mov_membase_reg(compMethod, X86_EBP, -((localIndex + 1) * global_SizeOfPointerInBytes), X86_EAX, global_SizeOfPointerInBytes);
	}
	return compMethod;
}


char* JIT_Compile_Load_LocalVar				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	uint32_t localIndex = *(uint32_t*)instr->Arg1;
	uint32_t size = global_SizeOfPointerInBytes;
	/*if (mth->LocalVariables[localIndex]->VariableType->IsValueType)
		size = mth->LocalVariables[localIndex]->VariableType->Size;*/
	uint32_t movCount = size / global_SizeOfPointerInBytes;
	if ((size % global_SizeOfPointerInBytes) != 0) ++movCount;
	for (uint32_t movIndex = 0; movIndex < movCount; ++movIndex)
	{
		x86_push_membase(compMethod, X86_EBP, -((localIndex + 1) * global_SizeOfPointerInBytes));
	}
	return compMethod;
}


char* JIT_Compile_Load_LocalVar_Address		(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	uint32_t localIndex = *(uint32_t*)instr->Arg1;
	x86_push_reg(compMethod, X86_EBP);
	x86_alu_membase_imm(compMethod, X86_SUB, X86_ESP, 0, (localIndex + 1) * global_SizeOfPointerInBytes);
	return compMethod;
}


char* JIT_Compile_Load_Parameter			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	uint32_t paramIndex = *(uint32_t*)instr->Arg1;
	uint32_t size = global_SizeOfPointerInBytes;
	if (mth->Parameters[paramIndex]->Type->IsValueType)
		size = mth->Parameters[paramIndex]->Type->Size;
	Align(&size);
	uint32_t movCount = size / global_SizeOfPointerInBytes;
	if ((size % global_SizeOfPointerInBytes) != 0) ++movCount;
	for (uint32_t movIndex = 0; movIndex < movCount; ++movIndex)
	{
		x86_push_membase(compMethod, X86_EBP,  mth->Parameters[paramIndex]->Offset);
	}
	return compMethod;
}


char* JIT_Compile_Load_Parameter_Address	(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	uint32_t paramIndex = *(uint32_t*)instr->Arg1;
	x86_push_reg(compMethod, X86_EBP);
	x86_alu_membase_imm(compMethod, X86_ADD, X86_ESP, 0, mth->Parameters[paramIndex]->Offset);
	return compMethod;
}


char* JIT_Compile_Store_Parameter			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	uint32_t paramIndex = *(uint32_t*)instr->Arg1;
	uint32_t size = global_SizeOfPointerInBytes;
	if (mth->Parameters[paramIndex]->Type->IsValueType)
		size = mth->Parameters[paramIndex]->Type->Size;
	Align(&size);
	uint32_t movCount = size / global_SizeOfPointerInBytes;
	for (uint32_t movIndex = 0; movIndex < movCount; ++movIndex)
	{
		x86_pop_reg(compMethod, X86_EAX);
		x86_mov_membase_reg(compMethod, X86_EBP, mth->Parameters[paramIndex]->Offset, X86_EAX, 4);
	}
	return compMethod;
}



char* JIT_Compile_Convert_OverflowCheck		(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_Convert_Unchecked			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	char buf[128];
	ConversionArgumentType fromType = *(ConversionArgumentType*)instr->Arg1;
	ConversionArgumentType toType = *(ConversionArgumentType*)instr->Arg2;
	switch (fromType)
	{
		case ConversionArgumentType_I:
		case ConversionArgumentType_U:
		case ConversionArgumentType_I4:
			switch (toType)
			{
				case ConversionArgumentType_I1:
					x86_pop_reg(compMethod, X86_EAX);
					x86_widen_reg(compMethod, X86_EAX, X86_EAX, TRUE, FALSE);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_U1:
					x86_pop_reg(compMethod, X86_EAX);
					x86_widen_reg(compMethod, X86_EAX, X86_EAX, FALSE, FALSE);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_I2:
					x86_pop_reg(compMethod, X86_EAX);
					x86_widen_reg(compMethod, X86_EAX, X86_EAX, TRUE, TRUE);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_U2:
					x86_pop_reg(compMethod, X86_EAX);
					x86_widen_reg(compMethod, X86_EAX, X86_EAX, FALSE, TRUE);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_I:
				case ConversionArgumentType_I4:
				case ConversionArgumentType_U:
				case ConversionArgumentType_U4: break;
				case ConversionArgumentType_I8:
					x86_pop_reg(compMethod, X86_EAX);
					x86_cdq(compMethod);
					x86_push_reg(compMethod, X86_EDX);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_U8:
					x86_pop_reg(compMethod, X86_EAX);
					x86_push_imm(compMethod, 0);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_R4:
					x86_fild_membase(compMethod, X86_ESP, 0, FALSE);
					x86_fst_membase(compMethod, X86_ESP, 0, FALSE, TRUE);
					x86_fld_membase(compMethod, X86_ESP, 0, FALSE);
					x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 4);
					break;
				case ConversionArgumentType_R8:
					x86_fild_membase(compMethod, X86_ESP, 0, FALSE);
					x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 4);
					break;
				default:
					{
						snprintf(buf, 128, "Convert Unchecked (Branch 1), Invalid Arguments: From = 0x%x, To = 0x%x", (unsigned int)fromType, (unsigned int)toType);
						Panic(buf);
					}
					break;
			}
			break;
		case ConversionArgumentType_I8:
		case ConversionArgumentType_U8:
			switch (toType)
			{
				case ConversionArgumentType_I1:
				case ConversionArgumentType_U1:
					x86_pop_reg(compMethod, X86_EAX);
					x86_pop_reg(compMethod, X86_EBX);
					x86_alu_reg_imm(compMethod, X86_AND, X86_EAX, 0xFF);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_I2:
				case ConversionArgumentType_U2:
					x86_pop_reg(compMethod, X86_EAX);
					x86_pop_reg(compMethod, X86_EBX);
					x86_alu_reg_imm(compMethod, X86_AND, X86_EAX, 0xFFFF);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_I:
				case ConversionArgumentType_I4:
				case ConversionArgumentType_U:
				case ConversionArgumentType_U4:
					x86_pop_reg(compMethod, X86_EAX);
					x86_pop_reg(compMethod, X86_EBX);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_I8:
				case ConversionArgumentType_U8: 
					break;
				case ConversionArgumentType_R4:
					x86_fild_membase(compMethod, X86_ESP, 0, TRUE);
					x86_fst_membase(compMethod, X86_ESP, 0, FALSE, TRUE);
					x86_fld_membase(compMethod, X86_ESP, 0, FALSE);
					x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 8);
					break;
				case ConversionArgumentType_R8:
					x86_fild_membase(compMethod, X86_ESP, 0, TRUE);
					x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 8);
					break;
				default:
					{
						snprintf(buf, 128, "Convert Unchecked (Branch 2), Invalid Arguments: From = 0x%x, To = 0x%x", (unsigned int)fromType, (unsigned int)toType);
						Panic(buf);
					}
					break;
			}
			break;
		case ConversionArgumentType_R8:
			switch (toType)
			{
				case ConversionArgumentType_I1:
					x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 8);
					x86_fst_membase(compMethod, X86_ESP, 0, TRUE, TRUE);
					x86_movsd_reg_membase(compMethod, X86_XMM0, X86_ESP, 0);
					x86_cvttsd2si(compMethod, X86_EAX, X86_XMM0);
					x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 8);
					x86_widen_reg(compMethod, X86_EAX, X86_EAX, TRUE, FALSE);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_U1:
					x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 8);
					x86_fst_membase(compMethod, X86_ESP, 0, TRUE, TRUE);
					x86_movsd_reg_membase(compMethod, X86_XMM0, X86_ESP, 0);
					x86_cvttsd2si(compMethod, X86_EAX, X86_XMM0);
					x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 8);
					x86_widen_reg(compMethod, X86_EAX, X86_EAX, FALSE, FALSE);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_I2:
					x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 8);
					x86_fst_membase(compMethod, X86_ESP, 0, TRUE, TRUE);
					x86_movsd_reg_membase(compMethod, X86_XMM0, X86_ESP, 0);
					x86_cvttsd2si(compMethod, X86_EAX, X86_XMM0);
					x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 8);
					x86_widen_reg(compMethod, X86_EAX, X86_EAX, TRUE, TRUE);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_U2:
					x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 8);
					x86_fst_membase(compMethod, X86_ESP, 0, TRUE, TRUE);
					x86_movsd_reg_membase(compMethod, X86_XMM0, X86_ESP, 0);
					x86_cvttsd2si(compMethod, X86_EAX, X86_XMM0);
					x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 8);
					x86_widen_reg(compMethod, X86_EAX, X86_EAX, FALSE, TRUE);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_I:
				case ConversionArgumentType_I4:
				case ConversionArgumentType_U:
				case ConversionArgumentType_U4:
					x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 8);
					x86_fst_membase(compMethod, X86_ESP, 0, TRUE, TRUE);
					x86_movsd_reg_membase(compMethod, X86_XMM0, X86_ESP, 0);
					x86_cvttsd2si(compMethod, X86_EAX, X86_XMM0);
					x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 8);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_I8:
				case ConversionArgumentType_U8:
					x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 4);
					x86_fnstcw_membase(compMethod, X86_ESP, 0);
					x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, 2);
					x86_alu_reg_imm(compMethod, X86_OR, X86_EAX, 0x0C00);
					x86_mov_membase_reg(compMethod, X86_ESP, 2, X86_EAX, 2);
					x86_fldcw_membase(compMethod, X86_ESP, 2);
					x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 8);
					x86_fist_pop_membase(compMethod, X86_ESP, 0, TRUE);
					x86_pop_reg(compMethod, X86_EAX);
					x86_pop_reg(compMethod, X86_EBX);
					x86_fldcw_membase(compMethod, X86_ESP, 0);
					x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 4);
					x86_push_reg(compMethod, X86_EBX);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ConversionArgumentType_R4:
					x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 4);
					x86_fst_membase(compMethod, X86_ESP, 0, FALSE, TRUE);
					x86_fld_membase(compMethod, X86_ESP, 0, FALSE);
					x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 4);
					break;
				case ConversionArgumentType_R8: break;
				default:
					{
						snprintf(buf, 128, "Convert Unchecked (Branch 3), Invalid Arguments: From = 0x%x, To = 0x%x", (unsigned int)fromType, (unsigned int)toType);
						Panic(buf);
					}
					break;
			}
			break;
		default:
			{
				snprintf(buf, 128, "Convert Unchecked (Branch 4), Invalid Arguments: From = 0x%x, To = 0x%x", (unsigned int)fromType, (unsigned int)toType);
				Panic(buf);
			}
			break;
	}
	return compMethod;
}


char* JIT_Compile_Load_Element				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType elementType = *(ElementType*)instr->Arg1;
	//StackObjectType indexType = *(StackObjectType*)instr->Arg2;
	uint32_t sizeOfElementType = 0;
	GetSizeOfElementType(sizeOfElementType, elementType);

	x86_pop_reg(compMethod, X86_EAX); // Index
	x86_pop_reg(compMethod, X86_ECX); // Array
	x86_mov_reg_membase(compMethod, X86_ECX, X86_ECX, 0, global_SizeOfPointerInBytes);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ECX, sizeof(GCArray));
	x86_imul_reg_reg_imm(compMethod, X86_EAX, X86_EAX, sizeOfElementType);
	x86_alu_reg_reg(compMethod, X86_ADD, X86_ECX, X86_EAX);
	x86_mov_reg_membase(compMethod, X86_EAX, X86_ECX, 0, sizeOfElementType);
	x86_push_reg(compMethod, X86_EAX);
	return compMethod;
}

char* JIT_Compile_Load_Element_Evil			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_Load_Element_Address		(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_Store_Element				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType elementType = *(ElementType*)instr->Arg1;
	//StackObjectType indexType = *(StackObjectType*)instr->Arg2;
	uint32_t sizeOfElementType = 0;
	GetSizeOfElementType(sizeOfElementType, elementType);

	x86_pop_reg(compMethod, X86_EDX); // Value
	x86_pop_reg(compMethod, X86_ECX); // Index
	x86_pop_reg(compMethod, X86_EAX); // Array
	x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, global_SizeOfPointerInBytes);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_EAX, sizeof(GCArray));
	x86_imul_reg_reg_imm(compMethod, X86_ECX, X86_ECX, sizeOfElementType);
	x86_alu_reg_reg(compMethod, X86_ADD, X86_EAX, X86_ECX);
	x86_mov_membase_reg(compMethod, X86_EAX, 0, X86_EDX, sizeOfElementType);
	return compMethod;
}

char* JIT_Compile_Store_Element_Evil		(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_Load_Array_Length			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_pop_reg(compMethod, X86_ECX);
	x86_mov_reg_membase(compMethod, X86_ECX, X86_ECX, 0, global_SizeOfPointerInBytes);
	x86_mov_reg_membase(compMethod, X86_ECX, X86_ECX, 0, 4);
	x86_push_reg(compMethod, X86_ECX);
	return compMethod;
}


char* JIT_Compile_Pop						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	switch(*((ElementType*)instr->Arg1))
	{
		case ElementType_I:
		case ElementType_U:
		case ElementType_I1:
		case ElementType_U1:
		case ElementType_I2:
		case ElementType_U2:
		case ElementType_I4:
		case ElementType_U4:
		case ElementType_R4:
		case ElementType_Ref:
		case ElementType_ManagedPointer:
			x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, (unsigned int)4);
			break;
		case ElementType_I8:
		case ElementType_U8:
		case ElementType_R8:
			x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, (unsigned int)8);
			break;
		case ElementType_DataType:
			{
				uint32_t pCount = ((IRType*)instr->Arg2)->Size / 4;
				uint32_t extra = ((IRType*)instr->Arg2)->Size % 4;
				if (extra)
				{
					pCount++;
				}
				// Repeated popping would just be slow,
				// and, as we're not using the values,
				// it's easier just to move the stack pointer.
				x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, (unsigned int)pCount * 4);
			}
			break;
		default:
			Panic("Unsupported Element Type!");
			break;
	}
	return compMethod;
}


char* JIT_Compile_Shift						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ShiftType shiftType = *(ShiftType*)instr->Arg1;
	StackObjectType shiftedType = *(StackObjectType*)instr->Arg2;
	StackObjectType shifterType = *(StackObjectType*)instr->Arg3;

	switch (shiftType)
	{
		case ShiftType_Right:
			switch (shiftedType)
			{
				case StackObjectType_Int32:
				case StackObjectType_NativeInt:
					switch (shifterType)
					{
						case StackObjectType_Int32:
						case StackObjectType_NativeInt:
							x86_pop_reg(compMethod, X86_ECX);
							x86_pop_reg(compMethod, X86_EAX);
							x86_shift_reg(compMethod, X86_SAR, X86_EAX);
							x86_push_reg(compMethod, X86_EAX);
							break;
						default: Panic("Invalid shifter type"); break;
					}
					break;
				case StackObjectType_Int64:
					switch (shifterType)
					{
						case StackObjectType_Int32:
						case StackObjectType_NativeInt:
							{
								x86_pop_reg(compMethod, X86_ECX);
								x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 4, 4);
								x86_test_membase_imm(compMethod, X86_ECX, 0, 32);
								unsigned char* jumpHighPartIsZero = (unsigned char *)compMethod;
								x86_branch8(compMethod, X86_CC_AE, 0, FALSE);
								// 32bit or less shift
								x86_pop_reg(compMethod, X86_EBX);
								x86_shrd_reg(compMethod, X86_EBX, X86_EBX);
								x86_push_reg(compMethod, X86_EBX);
								x86_shift_reg(compMethod, X86_SAR, X86_EAX);
								x86_mov_membase_reg(compMethod, X86_ESP, 4, X86_EAX, 4);
								unsigned char* jumpEnd = (unsigned char *)compMethod;
								x86_jump8(compMethod, 0);
								x86_patch(jumpHighPartIsZero, (unsigned char *)compMethod);
								// 33-64bit shift
								x86_alu_reg_imm(compMethod, X86_AND, X86_ECX, 0x1F);
								x86_shift_reg(compMethod, X86_SAR, X86_EAX);
								x86_push_reg(compMethod, X86_EAX);
								x86_mov_membase_imm(compMethod, X86_ESP, 4, 0, 4);
								x86_patch(jumpEnd, (unsigned char *)compMethod);
								break;
							}
						default: Panic("Invalid shifter type"); break;
					}
					break;
				default: Panic("Invalid shifted type"); break;
			}
			break;
		case ShiftType_Left:
			switch (shiftedType)
			{
				case StackObjectType_Int32:
				case StackObjectType_NativeInt:
					switch (shifterType)
					{
						case StackObjectType_Int32:
						case StackObjectType_NativeInt:
							x86_pop_reg(compMethod, X86_ECX);
							x86_pop_reg(compMethod, X86_EAX);
							x86_shift_reg(compMethod, X86_SHL, X86_EAX);
							x86_push_reg(compMethod, X86_EAX);
							break;
						default: Panic("Invalid shifter type"); break;
					}
					break;
				case StackObjectType_Int64:
					switch (shifterType)
					{
						case StackObjectType_Int32:
						case StackObjectType_NativeInt:
							{
								x86_pop_reg(compMethod, X86_ECX);
								x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, 4, 4);
								x86_test_membase_imm(compMethod, X86_ECX, 0, 32);
								unsigned char* jumpHighPartIsZero = (unsigned char *)compMethod;
								x86_branch8(compMethod, X86_CC_AE, 0, FALSE);
								// 32bit or less shift
								x86_pop_reg(compMethod, X86_EAX);
								x86_shld_reg(compMethod, X86_EAX, X86_EAX);
								x86_push_reg(compMethod, X86_EAX);
								x86_shift_reg(compMethod, X86_SHL, X86_EBX);
								x86_mov_membase_reg(compMethod, X86_ESP, 4, X86_EBX, 4);
								unsigned char* jumpEnd = (unsigned char *)compMethod;
								x86_jump8(compMethod, 0);
								x86_patch(jumpHighPartIsZero, (unsigned char *)compMethod);
								// 33-64bit shift
								x86_alu_reg_imm(compMethod, X86_AND, X86_ECX, 0x1F);
								x86_shift_reg(compMethod, X86_SHL, X86_EBX);
								x86_push_reg(compMethod, X86_EBX);
								x86_mov_membase_imm(compMethod, X86_ESP, 0, 0, 4);
								x86_patch(jumpEnd, (unsigned char *)compMethod);
								break;
							}
						default: Panic("Invalid shifter type"); break;
					}
					break;
				default: Panic("Invalid shifted type"); break;
			}
			break;
		default: Panic("Invalid/Unsupported shift direction"); break;
	}
	return compMethod;
}


char* JIT_Compile_Add						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	OverflowType ovfType = *(OverflowType*)instr->Arg1;
	ElementType argEins = *(ElementType*)instr->Arg2;
	//ElementType argZwei = *(ElementType*)instr->Arg3;

	switch(ovfType)
	{
		case OverflowType_None:
			switch(argEins)
			{
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U:
				case ElementType_U4:
					x86_pop_reg(compMethod, X86_EAX);
					x86_alu_membase_reg(compMethod, X86_ADD, X86_ESP, 0, X86_EAX);
					break;
				case ElementType_I8:
				case ElementType_U8:
					x86_pop_reg(compMethod, X86_EAX);
					x86_pop_reg(compMethod, X86_EBX);
					x86_alu_membase_reg(compMethod, X86_ADD, X86_ESP, 0, X86_EAX);
					x86_alu_membase_reg(compMethod, X86_ADC, X86_ESP, 4, X86_EBX);
					break;
				case ElementType_R4:
					x86_sse_alu_ss_reg_reg(compMethod, X86_SSE_ADD, X86_XMM1, X86_XMM0);
					break;
				case ElementType_R8:
					x86_sse_alu_sd_reg_reg(compMethod, X86_SSE_ADD, X86_XMM1, X86_XMM0);
					break;
				default: Panic("Invalid operand ElementType"); break;
			}
			break;
		case OverflowType_Signed:
		case OverflowType_Unsigned:
		default: Panic("Unsupported OverflowType!"); break;
	}
	return compMethod;
}


char* JIT_Compile_Sub						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	OverflowType ovfType = *(OverflowType*)instr->Arg1;
	ElementType argEins = *(ElementType*)instr->Arg2;
	//ElementType argZwei = *(ElementType*)instr->Arg3;

	switch(ovfType)
	{
		case OverflowType_None:
			switch(argEins)
			{
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U:
				case ElementType_U4:
					x86_pop_reg(compMethod, X86_EAX);
					x86_alu_membase_reg(compMethod, X86_SUB, X86_ESP, 0, X86_EAX);
					break;
				case ElementType_I8:
				case ElementType_U8:
					x86_pop_reg(compMethod, X86_EAX);
					x86_pop_reg(compMethod, X86_EBX);
					x86_alu_membase_reg(compMethod, X86_SUB, X86_ESP, 0, X86_EAX);
					x86_alu_membase_reg(compMethod, X86_SBB, X86_ESP, 4, X86_EBX);
					break;
				case ElementType_R4:
					x86_sse_alu_ss_reg_reg(compMethod, X86_SSE_SUB, X86_XMM1, X86_XMM0);
					break;
				case ElementType_R8:
					x86_sse_alu_sd_reg_reg(compMethod, X86_SSE_SUB, X86_XMM1, X86_XMM0);
					break;
				default: Panic("Invalid operand ElementType"); break;
			}
			break;
		case OverflowType_Signed:
		case OverflowType_Unsigned:
		default: Panic("Unsupported OverflowType!"); break;
	}
	return compMethod;
}


char* JIT_Compile_Mul						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	OverflowType ovfType = *(OverflowType*)instr->Arg1;
	ElementType argEins = *(ElementType*)instr->Arg2;
	//ElementType argZwei = *(ElementType*)instr->Arg3;

	switch(ovfType)
	{
		case OverflowType_None:
			switch(argEins)
			{
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U:
				case ElementType_U4:
					x86_pop_reg(compMethod, X86_EAX);
					x86_pop_reg(compMethod, X86_EBX);
					x86_imul_reg_reg(compMethod, X86_EAX, X86_EBX);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ElementType_I8:
				case ElementType_U8:
					Panic("Unsupported! No, it doesn't matter what this is.");
					break;
				case ElementType_R4:
					x86_sse_alu_ss_reg_reg(compMethod, X86_SSE_MUL, X86_XMM1, X86_XMM0);
					break;
				case ElementType_R8:
					x86_sse_alu_sd_reg_reg(compMethod, X86_SSE_MUL, X86_XMM1, X86_XMM0);
					break;
				default: Panic("Invalid operand ElementType"); break;
			}
			break;
		case OverflowType_Signed:
		case OverflowType_Unsigned:
		default: Panic("Unsupported OverflowType!"); break;
	}
	return compMethod;
}


char* JIT_Compile_Div						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	OverflowType ovfType = *(OverflowType*)instr->Arg1;
	ElementType argEins = *(ElementType*)instr->Arg2;
	//ElementType argZwei = *(ElementType*)instr->Arg3;

	switch(ovfType)
	{
		case OverflowType_Unsigned:
		case OverflowType_None:
			switch(argEins)
			{
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U:
				case ElementType_U4:
					x86_pop_reg(compMethod, X86_EAX);
					x86_cdq(compMethod);
					x86_div_membase(compMethod, X86_ESP, 0, TRUE);
					break;
				case ElementType_I8:
				case ElementType_U8:
					Panic("Unsupported! No, it doesn't matter what this is.");
					break;
				case ElementType_R4:
					x86_sse_alu_ss_reg_reg(compMethod, X86_SSE_DIV, X86_XMM1, X86_XMM0);
					break;
				case ElementType_R8:
					x86_sse_alu_sd_reg_reg(compMethod, X86_SSE_DIV, X86_XMM1, X86_XMM0);
					break;
				default: Panic("Invalid operand ElementType"); break;
			}
			break;
		case OverflowType_Signed:
		default:
			Panic("Unsupported OverflowType!");
			break;
	}
	return compMethod;
}


char* JIT_Compile_Rem						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	OverflowType ovfType = *(OverflowType*)instr->Arg1;
	ElementType argEins = *(ElementType*)instr->Arg2;
	//ElementType argZwei = *(ElementType*)instr->Arg3;

	switch(ovfType)
	{
		case OverflowType_None:
			switch(argEins)
			{
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U:
				case ElementType_U4:
					x86_pop_reg(compMethod, X86_EAX);
					x86_cdq(compMethod);
					x86_div_membase(compMethod, X86_ESP, 0, TRUE);
					x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_EDX, 4);
					break;
				case ElementType_I8:
				case ElementType_U8:
					Panic("Unsupported! No, it doesn't matter what this is.");
					break;
				case ElementType_R4:
					x86_sse_alu_ps_reg_reg(compMethod, X86_SSE_XOR, X86_XMM2, X86_XMM2);
					x86_sse_alu_ss_reg_reg(compMethod, X86_SSE_DIV, X86_XMM1, X86_XMM0);
					x86_fxch(compMethod, 2);
					break;
				case ElementType_R8:
					x86_sse_alu_ps_reg_reg(compMethod, X86_SSE_XOR, X86_XMM2, X86_XMM2);
					x86_sse_alu_sd_reg_reg(compMethod, X86_SSE_DIV, X86_XMM1, X86_XMM0);
					x86_fxch(compMethod, 2);
					break;
				default: Panic("Invalid operand ElementType"); break;
			}
			break;
		case OverflowType_Unsigned:
		case OverflowType_Signed:
		default: 
			Panic(String_Format("Unsupported OverflowType (%i)!", (int)ovfType)); 
			break;
	}
	return compMethod;
}


char* JIT_Compile_LoadIndirect				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType elementType = *(ElementType*)instr->Arg1;
	uint32_t sizeOfElementType = 0;
	GetSizeOfElementType(sizeOfElementType, elementType);
	x86_pop_reg(compMethod, X86_ECX);
	x86_mov_reg_membase(compMethod, X86_ECX, X86_ECX, 0, sizeOfElementType);
	x86_push_reg(compMethod, X86_ECX);
	return compMethod;
}


char* JIT_Compile_StoreIndirect				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType elementType = *(ElementType*)instr->Arg1;
	uint32_t sizeOfElementType = 0;
	GetSizeOfElementType(sizeOfElementType, elementType);
	x86_pop_reg(compMethod, X86_EAX);
	x86_pop_reg(compMethod, X86_ECX);
	x86_mov_membase_reg(compMethod, X86_ECX, 0, X86_EAX, sizeOfElementType);
	return compMethod;
}


char* JIT_Compile_LoadNull					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_push_imm(compMethod, (unsigned int)0);
	return compMethod;
}


char* JIT_Compile_NewObject					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRMethod* method = (IRMethod*)instr->Arg1;
	IRType* type = method->ParentAssembly->Types[method->MethodDefinition->TypeDefinition->TableIndex - 1];
	if (type->TypeDef != method->ParentAssembly->ParentDomain->CachedType___System_String)
	{
		compMethod = JIT_Emit_ParamSwap(compMethod, method->ParameterCount - 1);

		uint32_t sizeOfType = SizeOfType(type);
		x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 28);
		x86_mov_membase_imm(compMethod, X86_ESP, 20, mth->MethodDefinition->TypeDefinition->TableIndex - 1, 4);
		x86_mov_membase_imm(compMethod, X86_ESP, 16, mth->ParentAssembly->AssemblyIndex, 4);
		x86_mov_membase_imm(compMethod, X86_ESP, 12, mth->ParentAssembly->ParentDomain->DomainIndex, 4);
		x86_mov_membase_imm(compMethod, X86_ESP, 8, sizeOfType, 4);
		x86_mov_membase_imm(compMethod, X86_ESP, 4, (int)mth->ParentAssembly->ParentDomain->RootObject, 4);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (int)mth->ParentAssembly->ParentDomain->GarbageCollector, 4);
		x86_call_code(compMethod, GC_AllocateObject);
		x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 24);
		x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_EAX, 4);
	
		if (!method->AssembledMethod) JIT_CompileMethod(method);

		x86_call_code(compMethod, method->AssembledMethod);
		x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, (method->ParameterCount - 1) * 4);
	}
	else // Strings return their new obj from constructor
	{
		compMethod = JIT_Emit_ParamSwap(compMethod, method->ParameterCount - 1);
		x86_push_imm(compMethod, 0);
		x86_push_imm(compMethod, method->ParentAssembly->ParentDomain);

		x86_call_code(compMethod, method->AssembledMethod);
		x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, (method->ParameterCount + 1) * 4);
		x86_push_reg(compMethod, X86_EAX);
	}

	return compMethod;
}

char* JIT_Compile_NewArray					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRType* type = (IRType*)instr->Arg1;
	uint32_t sizeOfType = SizeOfType(type);
	x86_pop_reg(compMethod, X86_EAX); // Number of Elements
	x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 32);
	x86_mov_membase_imm(compMethod, X86_ESP, 24, type->TypeIndex, 4);
	x86_mov_membase_imm(compMethod, X86_ESP, 20, type->ParentAssembly->AssemblyIndex, 4);
	x86_mov_membase_imm(compMethod, X86_ESP, 16, type->ParentAssembly->ParentDomain->DomainIndex, 4);
	x86_mov_membase_imm(compMethod, X86_ESP, 12, sizeOfType, 4);
	x86_mov_membase_reg(compMethod, X86_ESP, 8, X86_EAX, 4);
	x86_mov_membase_imm(compMethod, X86_ESP, 4, (int)mth->ParentAssembly->ParentDomain->RootObject, 4);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (int)mth->ParentAssembly->ParentDomain->GarbageCollector, 4);
	x86_call_code(compMethod, GC_AllocateArray);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 28);
	x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_EAX, 4);
	return compMethod;
}


char* JIT_Compile_Dup						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, 4);
	x86_push_reg(compMethod, X86_EAX);
	if (*(ElementType*)instr->Arg1 == ElementType_I8 || *(ElementType*)instr->Arg1 == ElementType_U8)
	{
		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 8, 4);
		x86_push_reg(compMethod, X86_EAX);
	}
	return compMethod;
}


char* JIT_Compile_And						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType arg1Type = *(ElementType*)instr->Arg1;
	ElementType arg2Type = *(ElementType*)instr->Arg2;

	switch(arg1Type)
	{
		case ElementType_I:
		case ElementType_I4:
		case ElementType_U:
		case ElementType_U4:
			if (arg2Type != ElementType_I && arg2Type != ElementType_I4 && arg2Type != ElementType_U && arg2Type != ElementType_U4)
				Panic("Invalid type combination for binary AND");
			x86_pop_reg(compMethod, X86_EAX);
			x86_alu_membase_reg(compMethod, X86_AND, X86_ESP, 0, X86_EAX);
			break;
		case ElementType_I8:
			if (arg2Type != ElementType_I8)
				Panic("Invalid type combination for binary AND");
			x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, 4);
			x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, 4, 4);
			x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 8);
			x86_alu_membase_reg(compMethod, X86_AND, X86_ESP, 0, X86_EAX);
			x86_alu_membase_reg(compMethod, X86_AND, X86_ESP, 4, X86_EBX);
			break;
		default:
			Panic("Invalid type combination for binary AND");
			break;
	}

	return compMethod;
}


char* JIT_Compile_Or						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType arg1Type = *(ElementType*)instr->Arg1;
	ElementType arg2Type = *(ElementType*)instr->Arg2;

	switch(arg1Type)
	{
		case ElementType_I:
		case ElementType_I4:
		case ElementType_U:
		case ElementType_U4:
			if (arg2Type != ElementType_I && arg2Type != ElementType_I4 && arg2Type != ElementType_U && arg2Type != ElementType_U4)
				Panic("Invalid type combination for binary OR");
			x86_pop_reg(compMethod, X86_EAX);
			x86_alu_membase_reg(compMethod, X86_OR, X86_ESP, 0, X86_EAX);
			break;
		case ElementType_I8:
			if (arg2Type != ElementType_I8)
				Panic("Invalid type combination for binary OR");
			x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, 4);
			x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, 4, 4);
			x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 8);
			x86_alu_membase_reg(compMethod, X86_OR, X86_ESP, 0, X86_EAX);
			x86_alu_membase_reg(compMethod, X86_OR, X86_ESP, 4, X86_EBX);
			break;
		default:
			Panic("Invalid type combination for binary OR");
			break;
	}

	return compMethod;
}


char* JIT_Compile_XOr						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType arg1Type = *(ElementType*)instr->Arg1;
	ElementType arg2Type = *(ElementType*)instr->Arg2;

	switch(arg1Type)
	{
		case ElementType_I:
		case ElementType_I4:
		case ElementType_U:
		case ElementType_U4:
			if (arg2Type != ElementType_I && arg2Type != ElementType_I4 && arg2Type != ElementType_U && arg2Type != ElementType_U4)
				Panic("Invalid type combination for binary XOR");
			x86_pop_reg(compMethod, X86_EAX);
			x86_alu_membase_reg(compMethod, X86_XOR, X86_ESP, 0, X86_EAX);
			break;
		case ElementType_I8:
			if (arg2Type != ElementType_I8)
				Panic("Invalid type combination for binary XOR");
			x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, 4);
			x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, 4, 4);
			x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 8);
			x86_alu_membase_reg(compMethod, X86_XOR, X86_ESP, 0, X86_EAX);
			x86_alu_membase_reg(compMethod, X86_XOR, X86_ESP, 4, X86_EBX);
			break;
		default:
			Panic("Invalid type combination for binary XOR");
			break;
	}

	return compMethod;
}


char* JIT_Compile_Neg						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType arg1Type = *(ElementType*)instr->Arg1;
	if (arg1Type != ElementType_I && arg1Type != ElementType_I4 && arg1Type != ElementType_I8)
		Panic("Invalid type for binary NEG");
	x86_neg_membase(compMethod, X86_ESP, 0);
	if (arg1Type == ElementType_I8)
		x86_neg_membase(compMethod, X86_ESP, 4);
	return compMethod;
}


char* JIT_Compile_Not						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType arg1Type = *(ElementType*)instr->Arg1;
	if (arg1Type != ElementType_I && arg1Type != ElementType_I4 && arg1Type != ElementType_I8)
		Panic("Invalid type for binary NOT");
	x86_not_membase(compMethod, X86_ESP, 0);
	if (arg1Type == ElementType_I8)
		x86_not_membase(compMethod, X86_ESP, 4);
	return compMethod;
}


char* JIT_Compile_Load_String				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	// This should work, because the strings getting loaded
	// would be root objects, thus there is no need to be
	// trying to re-add them to the GC at runtime.
	ReferenceTypeObject* str = GC_AllocateString(mth->ParentAssembly->ParentDomain->GarbageCollector, mth->ParentAssembly->ParentDomain->RootObject, (uint8_t*)instr->Arg2, *(uint32_t*)instr->Arg1);
	
	x86_push_imm(compMethod, str);

	return compMethod;
}


char* JIT_Compile_Load_Field				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRFieldSpec* spec = (IRFieldSpec*)instr->Arg1;

	// Still need to handle large fields.
	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, 4);
	x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, spec->FieldIndex * global_SizeOfPointerInBytes, 4);
	x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_EAX, 4);

	return compMethod;
}

char* JIT_Compile_Load_Field_Address		(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_Store_Field				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRFieldSpec* spec = (IRFieldSpec*)instr->Arg1;

	// Still need to handle large fields.
	x86_pop_reg(compMethod, X86_EAX);
	x86_pop_reg(compMethod, X86_EBX); // Pop the value
	x86_mov_membase_reg(compMethod, X86_EAX, spec->FieldIndex * global_SizeOfPointerInBytes, X86_EBX, 4);
	return compMethod;
}


char* JIT_Compile_Load_Static_Field			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_Load_Static_Field_Address	(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_Store_Static_Field		(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_Load_Object				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_Store_Object				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_Copy_Object				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_Switch					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_CastClass					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_IsInst					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_Unbox						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_Unbox_Any					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_Box						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_Compare					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_CheckFinite				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_LocalAllocate				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_InitObject				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_SizeOf					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_push_imm(compMethod, SizeOfType((IRType*)instr->Arg1));
	return compMethod;
}


char* JIT_Compile_LoadFunction				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_LoadVirtualFunction		(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_Call_Absolute				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRMethod* m = (IRMethod*)instr->Arg1;
	if (!m->AssembledMethod)
	{
		JIT_CompileMethod(m);
	}

	compMethod = JIT_Emit_ParamSwap(compMethod, m->ParameterCount);

	x86_call_code(compMethod, m->AssembledMethod);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, (m->ParameterCount * 4));
	if (m->Returns)
	{
		x86_push_reg(compMethod, X86_EAX);
	}
	return compMethod;
}

char* JIT_Compile_Call_Internal				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	void* mthd = instr->Arg1; // This is the method we will be calling.
	IRMethod* m = (IRMethod*)instr->Arg2;
	
	compMethod = JIT_Emit_ParamSwap(compMethod, m->ParameterCount);

	x86_push_imm(compMethod, (unsigned int)m->ParentAssembly->ParentDomain); // Push the domain
	x86_call_code(compMethod, mthd); // Call the method.
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, ((m->ParameterCount + 1) * 4)); // Fix the stack after calling the method
	if (m->Returns)
	{
		x86_push_reg(compMethod, X86_EAX);
	}

	return compMethod;
}

#include <CLR/ReferenceTypeObject.h>
void* JIT_Trampoline_DoCall(IRMethodSpec* mth, ReferenceTypeObject* obj);
char* JIT_Compile_Call						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRMethodSpec* spec = (IRMethodSpec*)instr->Arg1;
	IRMethod* m = spec->ParentType->Methods[spec->MethodIndex];

	
	compMethod = JIT_Emit_ParamSwap(compMethod, m->ParameterCount);
	
	x86_push_imm(compMethod, instr->Arg1);
	x86_call_code(compMethod, JIT_Trampoline_DoCall);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 4); // Fix the stack from calling the trampoline
	x86_call_reg(compMethod, X86_EAX);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, (spec->ParentType->Methods[spec->MethodIndex]->ParameterCount * 4));
	if (m->Returns)
	{
		x86_push_reg(compMethod, X86_EAX);
	}

	return compMethod;
}

void* JIT_Trampoline_DoCall(IRMethodSpec* mth, ReferenceTypeObject* obj)
{
	void* variable;
	//AppDomain* dom = AppDomainRegistry_GetDomain(obj->DomainIndex);
	variable = AppDomainRegistry_GetDomain(obj->DomainIndex);
	if (obj->AssemblyIndex >= ((AppDomain*)variable)->IRAssemblyCount)
	{
		Panic("Assembly Index is too High!");
	}

	//IRAssembly* asmbly = dom->IRAssemblies[obj->AssemblyIndex];
	variable = ((AppDomain*)variable)->IRAssemblies[obj->AssemblyIndex];
	if (obj->TypeIndex >= ((IRAssembly*)variable)->TypeCount)
	{
		Panic("Type Index is too High!");
	}

	//IRType* tp = asmbly->Types[obj->TypeIndex];
	variable = ((IRAssembly*)variable)->Types[obj->TypeIndex];
	if (mth->MethodIndex >= ((IRType*)variable)->MethodCount)
	{
		Panic("Method Index is too High!");
	}

	//IRMethod* m = tp->Methods[mth->MethodIndex];
	variable = ((IRType*)variable)->Methods[mth->MethodIndex];
	if (((IRMethod*)variable)->MethodDefinition->ImplFlags & MethodImplAttributes_InternalCall)
	{
		return (((IRMethod*)variable)->MethodDefinition->InternalCall);
	}
	else if (!((IRMethod*)variable)->AssembledMethod)
	{
		JIT_CompileMethod(((IRMethod*)variable));
	}
	return ((IRMethod*)variable)->AssembledMethod;
}

char* JIT_Compile_Jump						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}
