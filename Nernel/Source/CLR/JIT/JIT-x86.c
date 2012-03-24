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

ALWAYS_INLINE char* JIT_Emit_ParamSwap(char* compMethod, IRMethod* pMethod, bool_t pIgnoreThis)
{
	JIT_Layout_Parameters(pMethod);
	int32_t paramsSize = 0;
	if ((pMethod->ParameterCount - 1) > 0)
	{
		paramsSize += pMethod->Parameters[pMethod->ParameterCount - 1]->Offset + pMethod->Parameters[pMethod->ParameterCount - 1]->Size - 8;
		if (pIgnoreThis)
			paramsSize -= global_SizeOfPointerInBytes;
		if (paramsSize > 0)
		{
			bool_t SimpleParams = TRUE;
			for (uint32_t i = 0; i < pMethod->ParameterCount; i++)
			{
				if (pMethod->Parameters[i]->Size > 4 
				   || pMethod->Parameters[i]->Type->TypeDef == pMethod->ParentAssembly->ParentDomain->CachedType___System_Single 
				   || pMethod->Parameters[i]->Type->TypeDef == pMethod->ParentAssembly->ParentDomain->CachedType___System_Double)
				{
					SimpleParams = FALSE;
					break;
				}
			}
			if (!SimpleParams)
			{
				x86_mov_reg_reg(compMethod, X86_EAX, X86_ESP, 4);
				//printf("\nParams aren't simple, size = %u.\n\n", (unsigned int)paramsSize);
				x86_adjust_stack(compMethod, -paramsSize);
				uint32_t dParamsCount = 0; // number of double-precision values.
				uint32_t fParamsCount = 0;
				for (uint32_t i = ((pIgnoreThis) ? 1 : 0); i < pMethod->ParameterCount; i++)
				{
					if (pMethod->Parameters[i]->Type->TypeDef == pMethod->ParentAssembly->ParentDomain->CachedType___System_Single)
					{
						fParamsCount++;
					}
					else if (pMethod->Parameters[i]->Type->TypeDef == pMethod->ParentAssembly->ParentDomain->CachedType___System_Double)
					{
						dParamsCount++;
						fParamsCount++;
					}
				}

				uint32_t curFParam = 0;
				uint32_t paramOffset = paramsSize;
				for (uint32_t i = ((pIgnoreThis) ? 1 : 0); i < pMethod->ParameterCount; i++)
				{
					if (pMethod->Parameters[i]->Type->TypeDef == pMethod->ParentAssembly->ParentDomain->CachedType___System_Single)
					{
						curFParam++;
						x86_fld_reg(compMethod, fParamsCount - curFParam);
						x86_fst_membase(compMethod, X86_ESP, paramsSize - pMethod->Parameters[i]->Offset, FALSE, TRUE);
					}
					else if (pMethod->Parameters[i]->Type->TypeDef == pMethod->ParentAssembly->ParentDomain->CachedType___System_Double)
					{
						curFParam++;
						x86_fld_reg(compMethod, fParamsCount - curFParam);
						x86_fst_membase(compMethod, X86_ESP, paramsSize - pMethod->Parameters[i]->Offset, TRUE, TRUE);
					}
					else
					{
						paramOffset -= pMethod->Parameters[i]->Size;
						for (uint32_t i2 = 0; i2 < pMethod->Parameters[i]->Size >> 2; i2++)
						{
							x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, pMethod->Parameters[i]->Offset - 8 + (i2 << 2), 4);
							x86_mov_membase_reg(compMethod, X86_ESP, paramOffset + (i2 << 2), X86_EBX, 4);
							//printf("Param %i offset: %i + extra: %i, into %u\n", (int)i, (int)pMethod->Parameters[i]->Offset - 8, (int)(i2 << 2), (unsigned int)(paramsSize - pMethod->Parameters[i]->Offset - 8 + (i2 << 2)));
						}
					}
				}

				if (fParamsCount > 0)
				{
					for (uint32_t i = 0; i < fParamsCount; i++)
					{
						x86_fdecstp(compMethod);
					}
				}

				// Now, everything is on the stack, and we just need to move it up a bit.
				int32_t fValueSize = ((fParamsCount - dParamsCount) << 2) + (dParamsCount << 3);
				for (int32_t i = 0; i < paramsSize >> 2; i++)
				{
					x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, i << 2, 4);
					x86_mov_membase_reg(compMethod, X86_EAX, i << 2, X86_EBX, 4);
				}
				x86_adjust_stack(compMethod, paramsSize - fValueSize);
			}
			else // Otherwise the params can be swapped in-place.
			{
				uint32_t paramCount = pMethod->ParameterCount - ((pIgnoreThis) ? 1 : 0);
				uint32_t swapCount = paramCount >> 1;
				// If paramCount is odd, the middle 
				// param won't move at all.
				for (uint32_t index = 0; index < swapCount; ++index)
				{
					x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, index << 2, 4);
					x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, (paramCount - (index + 1)) << 2, 4);
					x86_mov_membase_reg(compMethod, X86_ESP, index << 2, X86_EBX, 4);
					x86_mov_membase_reg(compMethod, X86_ESP, (paramCount - (index + 1)) << 2, X86_EAX, 4);
				}
			}
		}
	}



	return compMethod;
}


uint32_t StackSizeOfType(IRType* tp)
{
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
			if (tp->Size > 0)
				return tp->Size;
			uint32_t size = 0;
			for (uint32_t index = 0; index < tp->FieldCount; ++index)
			{
				if (tp->Fields[index]->FieldType->IsReferenceType)
					size += global_SizeOfPointerInBytes;
				else  // It's a value type.
					size += StackSizeOfType(tp->Fields[index]->FieldType);
			}
			tp->Size = size;
			return size;
		}
	}

	

	return global_SizeOfPointerInBytes;
}

ALWAYS_INLINE uint32_t SizeOfType(IRType* pType)
{
	//printf("Getting SizeOfType %u @ 0x%x\n", (unsigned int)pType->FieldCount, (unsigned int)pType);
	if (pType->Size) 
		return pType->Size;
	if (pType->IsValueType) 
		return StackSizeOfType(pType);
	uint32_t size = 0;
	for (uint32_t index = 0; index < pType->FieldCount; ++index)
	{
		if (pType->Fields[index]->FieldType->IsReferenceType)
			size += global_SizeOfPointerInBytes;
		else  // It's a value type.
			size += StackSizeOfType(pType->Fields[index]->FieldType);
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
			param->Size = StackSizeOfType(param->Type);
			param->Offset = offset;
			offset += param->Size;
			Align(&offset);
		}
		pMethod->ParametersLayedOut = TRUE;
	}
}

void JIT_Layout_LocalVariables(IRMethod* pMethod)
{
	if (!pMethod->LocalsLayedOut)
	{
		IRLocalVariable* local = NULL;
		uint32_t offset = 0;
		for (uint32_t index = 0; index < pMethod->LocalVariableCount; ++index)
		{
			local = pMethod->LocalVariables[index];
			local->Size = StackSizeOfType(local->VariableType);
			offset += local->Size;
			Align(&offset);
			local->Offset = offset;
		}
		pMethod->LocalsLayedOut = TRUE;
	}
}

void JIT_Layout_Fields(IRType* tp)
{
	if (!tp->FieldsLayedOut)
	{
		IRField* fld = NULL;
		uint32_t offset = 0;
		for (uint32_t i = 0; i < tp->FieldCount; i++)
		{
			fld = tp->Fields[i];
			fld->Size = StackSizeOfType(fld->FieldType);
			fld->Offset = offset;
			offset += fld->Size;
			// Fields are NOT aligned.
		}
		tp->FieldsLayedOut = TRUE;
	}
}

char* JIT_Emit_Prologue(IRMethod* mth, char* compMethod)
{
	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, 4);
	x86_mov_membase_reg(compMethod, X86_ESP, 4, X86_EAX, 4);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 4);
	x86_push_reg(compMethod, X86_EBP);

	uint32_t localsSize = 0;
	if (mth->LocalVariableCount > 0)
	{
		JIT_Layout_LocalVariables(mth);
		localsSize += mth->LocalVariables[mth->LocalVariableCount - 1]->Offset + mth->LocalVariables[mth->LocalVariableCount - 1]->Size;
		Align(&localsSize);
		//printf("Prologue: LastLocal %u Offset %u + %u Size\n", (unsigned int)mth->LocalVariableCount - 1, (unsigned int)mth->LocalVariables[mth->LocalVariableCount - 1]->Offset, (unsigned int)mth->LocalVariables[mth->LocalVariableCount - 1]->Size);
	}
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
		if (mth->MethodDefinition->Body.Flags & MethodDefinitionBody_Fat_Flags_InitializeLocals)
		{
			for (uint32_t count = 0; count < localsSize; count += 4)
			{
				x86_mov_membase_imm(compMethod, X86_ESP, count, 0, 4);
			}
		}
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
		if (StackSizeOfType(mth->ReturnType) == 8)
		{
			x86_pop_reg(compMethod, X86_EDX);
		}
	}
	x86_leave(compMethod);
	x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 4);
	x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, 4, 4);
	x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_EBX, 4);
	x86_ret(compMethod);
	return compMethod;
}




char* JIT_Compile_Nop(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	// Only emit a nop if this was
	// a nop emitted by the decomp,
	// not a nop created by the optimizers.
	if ((uint32_t)instr->Arg1 != 1)
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
	for (uint32_t index = 0; index < mth->LocalVariableCount; ++index)
	{
		if (mth->LocalVariables[index]->VariableType->IsReferenceType)
		{
			// Remove 1 RootObject reference for removing the local reference as we leave scope
			x86_mov_reg_membase(compMethod, X86_EAX, X86_EBP, -((int)mth->LocalVariables[index]->Offset), global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
			unsigned char* skipRemove = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipRemove, (unsigned char*)compMethod);
		}
	}
	x86_push_imm(compMethod, (unsigned int)mth->ParentAssembly->ParentDomain->GarbageCollector);
	x86_call_code(compMethod, GC_Collect);
	x86_adjust_stack(compMethod, global_SizeOfPointerInBytes);
	if (mth->Returns)
	{
		x86_pop_reg(compMethod, X86_EAX);
		if (StackSizeOfType(mth->ReturnType) == 8)
		{
			x86_pop_reg(compMethod, X86_EDX);
		}
	}
	x86_leave(compMethod);
	x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 4);
	x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, 4, 4);
	x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_EBX, 4);
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
	x86_push_imm(compMethod, (value >> 32));
	x86_push_imm(compMethod, (value & 0xFFFFFFFF));
	return compMethod;
}


char* JIT_Compile_LoadFloat32_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_fld(compMethod, instr->Arg1, FALSE);
	//x86_push_imm(compMethod, *((int32_t*)instr->Arg1));
	return compMethod;
}


char* JIT_Compile_LoadFloat64_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_fld(compMethod, instr->Arg1, TRUE);
	//int64_t value = *((int64_t*)instr->Arg1);
	//x86_push_imm(compMethod, (value & 0xFFFFFFFF));
	//x86_push_imm(compMethod, (value >> 32));
	return compMethod;
}

char* JIT_Compile_Branch					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	BranchCondition condition = *(BranchCondition*)instr->Arg1;
	IRInstruction* target = (IRInstruction*)instr->Arg2;
	if (condition == BranchCondition_Always)
	{
		unsigned char* comp = (unsigned char*)compMethod;
		x86_jump_code(comp, 0);
		BranchRegistry_RegisterBranchForLink(branchRegistry, instr->InstructionLocation, target->InstructionLocation, compMethod);
		compMethod = (char*)comp;
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
					x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);

					unsigned char* comp = (unsigned char*)compMethod;
					if (condition == BranchCondition_False)
					{
							x86_branch32(comp, X86_CC_Z, 0, FALSE);
					}
					else
					{
							x86_branch32(comp, X86_CC_NZ, 0, FALSE);
					}
					BranchRegistry_RegisterBranchForLink(branchRegistry, instr->InstructionLocation, target->InstructionLocation, compMethod);
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
			case ElementType_U:
			case ElementType_I4:
			case ElementType_U4:
			case ElementType_I2:
			case ElementType_I1:
			case ElementType_U2:
			case ElementType_U1:
			{
				x86_pop_reg(compMethod, X86_EAX);
				x86_pop_reg(compMethod, X86_EBX);
				x86_alu_reg_reg(compMethod, X86_CMP, X86_EBX, X86_EAX);
				break;
			}
			case ElementType_I8:
			case ElementType_U8:
			{
				x86_fild_membase(compMethod, X86_ESP, 0, TRUE);
				x86_fild_membase(compMethod, X86_ESP, 8, TRUE);
				x86_fcomip(compMethod, 1);
				x86_fdecstp(compMethod);
				x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 16);
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
				Panic("Invalid type for non-simple branch comparison.");
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
			if (branchReg->SpecialBranch[i])
			{
				*(size_t*)branchReg->BranchLocations[i] = branchReg->InstructionLocations[branchReg->TargetLocations[i]];
			}
			else
			{
				x86_patch((unsigned char*)branchReg->BranchLocations[i], (unsigned char*)branchReg->InstructionLocations[branchReg->TargetLocations[i]]);
			}
		}
	}

	return compMethod;
}


char* JIT_Compile_Optimized_Jump			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRInstruction* target = (IRInstruction*)instr->Arg1;
	unsigned char* comp = (unsigned char*)compMethod;
	x86_jump_code(comp, 0);
	BranchRegistry_RegisterBranchForLink(branchRegistry, instr->InstructionLocation, target->InstructionLocation, compMethod);
	compMethod = (char*)comp;
	return compMethod;
}


char* JIT_Compile_Store_LocalVar			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	uint32_t localIndex = *(uint32_t*)instr->Arg1;
	if (mth->LocalVariables[localIndex]->VariableType->TypeDef == mth->ParentAssembly->ParentDomain->CachedType___System_Single)
	{
		x86_fst_membase(compMethod, X86_EBP, -(mth->LocalVariables[localIndex]->Offset), FALSE, TRUE);
	}
	else if (mth->LocalVariables[localIndex]->VariableType->TypeDef == mth->ParentAssembly->ParentDomain->CachedType___System_Double)
	{
		x86_fst_membase(compMethod, X86_EBP, -(mth->LocalVariables[localIndex]->Offset), TRUE, TRUE);
	}
	else
	{
		uint32_t size = global_SizeOfPointerInBytes;
		if (mth->LocalVariables[localIndex]->VariableType->IsValueType)
			size = StackSizeOfType(mth->LocalVariables[localIndex]->VariableType);
		Align(&size);
		//printf("Storing to local variable %i of type %s\n", (int)localIndex, mth->LocalVariables[localIndex]->VariableType->TypeDef->Name);

		if (mth->LocalVariables[localIndex]->VariableType->IsReferenceType)
		{
			// Remove 1 RootObject reference for removing the local reference
			x86_mov_reg_membase(compMethod, X86_EAX, X86_EBP, -((int)mth->LocalVariables[localIndex]->Offset), global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
			unsigned char* skipRemove = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipRemove, (unsigned char*)compMethod);

			// Remove 1 RootObject reference for popping from stack
			x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
			skipRemove = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipRemove, (unsigned char*)compMethod);
		}
		
		uint32_t movCount = size / global_SizeOfPointerInBytes;
		uint32_t curBas = 0;
		for (uint32_t i = 0; i < movCount; i++)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, curBas, 4);
			x86_mov_membase_reg(compMethod, X86_EBP, -(mth->LocalVariables[localIndex]->Offset - (i << 2)), X86_EBX, 4);
			curBas += 4;
		}
		x86_adjust_stack(compMethod, (int32_t)size);

		if (mth->LocalVariables[localIndex]->VariableType->IsReferenceType)
		{
			x86_mov_reg_membase(compMethod, X86_EAX, X86_EBP, -((int)mth->LocalVariables[localIndex]->Offset), global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
			unsigned char* skipAdd = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			// Add reference to RootObject for storing a local reference
			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_AddReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipAdd, (unsigned char*)compMethod);
		}
	}
	return compMethod;
}


char* JIT_Compile_Load_LocalVar				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	uint32_t localIndex = *(uint32_t*)instr->Arg1;
	if (mth->LocalVariables[localIndex]->VariableType->TypeDef == mth->ParentAssembly->ParentDomain->CachedType___System_Single)
	{
		x86_fld_membase(compMethod, X86_ESP, -(mth->LocalVariables[localIndex]->Offset), FALSE);
	}
	else if (mth->LocalVariables[localIndex]->VariableType->TypeDef == mth->ParentAssembly->ParentDomain->CachedType___System_Double)
	{
		x86_fld_membase(compMethod, X86_ESP, -(mth->LocalVariables[localIndex]->Offset), TRUE);
	}
	else
	{
		uint32_t size = global_SizeOfPointerInBytes;
		if (mth->LocalVariables[localIndex]->VariableType->IsValueType)
			size = StackSizeOfType(mth->LocalVariables[localIndex]->VariableType);
		Align(&size);

		if (mth->LocalVariables[localIndex]->VariableType->IsReferenceType)
		{
			x86_mov_reg_membase(compMethod, X86_EAX, X86_EBP, -((int)mth->LocalVariables[localIndex]->Offset), global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
			unsigned char* skipRemove = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			// Add 1 RootObject reference for pushing to the stack
			x86_mov_reg_membase(compMethod, X86_EAX, X86_EBP, -((int)mth->LocalVariables[localIndex]->Offset), global_SizeOfPointerInBytes);
			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_AddReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipRemove, (unsigned char*)compMethod);
		}


		x86_adjust_stack(compMethod, -((int32_t)size));
		uint32_t movCount = size / global_SizeOfPointerInBytes;
		uint32_t curBas = 0;
		for (uint32_t i = 0; i < movCount; i++)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_EBP, -(mth->LocalVariables[localIndex]->Offset - (i << 2)), 4);
			x86_mov_membase_reg(compMethod, X86_ESP, curBas, X86_EBX, 4);
			curBas += 4;
		}
	}

	return compMethod;
}


char* JIT_Compile_Load_LocalVar_Address		(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	uint32_t localIndex = *(uint32_t*)instr->Arg1;
	x86_push_reg(compMethod, X86_EBP);
	x86_alu_membase_imm(compMethod, X86_SUB, X86_ESP, 0, mth->LocalVariables[localIndex]->Offset);
	return compMethod;
}


char* JIT_Compile_Load_Parameter			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	uint32_t paramIndex = *(uint32_t*)instr->Arg1;

	if (mth->Parameters[paramIndex]->Type->TypeDef == mth->ParentAssembly->ParentDomain->CachedType___System_Single)
	{
		x86_fld_membase(compMethod, X86_ESP, mth->Parameters[paramIndex]->Offset, FALSE);
	}
	else if (mth->Parameters[paramIndex]->Type->TypeDef == mth->ParentAssembly->ParentDomain->CachedType___System_Double)
	{
		x86_fld_membase(compMethod, X86_ESP, mth->Parameters[paramIndex]->Offset, TRUE);
	}
	else
	{
		uint32_t size = global_SizeOfPointerInBytes;
		if (mth->Parameters[paramIndex]->Type->IsValueType)
			size = StackSizeOfType(mth->Parameters[paramIndex]->Type);
		Align(&size);

		if (mth->Parameters[paramIndex]->Type->IsReferenceType)
		{
			x86_mov_reg_membase(compMethod, X86_EAX, X86_EBP, (int)mth->Parameters[paramIndex]->Offset, global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
			unsigned char* skipRemove = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			// Add 1 RootObject reference for pushing to the stack
			x86_mov_reg_membase(compMethod, X86_EAX, X86_EBP, (int)mth->Parameters[paramIndex]->Offset, global_SizeOfPointerInBytes);
			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_AddReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipRemove, (unsigned char*)compMethod);
		}

		x86_adjust_stack(compMethod, (int32_t)-size);
		uint32_t movCount = size / global_SizeOfPointerInBytes;
		uint32_t curBas = size - global_SizeOfPointerInBytes;
		for (uint32_t i = 0; i < movCount; i++)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_EBP, mth->Parameters[paramIndex]->Offset - (i << 2), 4);
			x86_mov_membase_reg(compMethod, X86_ESP, curBas, X86_EBX, 4);
			curBas -= 4;
		}
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
	if (mth->Parameters[paramIndex]->Type->TypeDef == mth->ParentAssembly->ParentDomain->CachedType___System_Single)
	{
		x86_fst_membase(compMethod, X86_ESP, mth->Parameters[paramIndex]->Offset, FALSE, TRUE);
	}
	else if (mth->Parameters[paramIndex]->Type->TypeDef == mth->ParentAssembly->ParentDomain->CachedType___System_Double)
	{
		x86_fst_membase(compMethod, X86_ESP, mth->Parameters[paramIndex]->Offset, TRUE, TRUE);
	}
	else
	{
		uint32_t size = global_SizeOfPointerInBytes;
		if (mth->Parameters[paramIndex]->Type->IsValueType)
			size = StackSizeOfType(mth->Parameters[paramIndex]->Type);
		Align(&size);

		if (mth->Parameters[paramIndex]->Type->IsReferenceType)
		{
			// Remove 1 RootObject reference for removing the parameter reference
			x86_mov_reg_membase(compMethod, X86_EAX, X86_EBP, (int)mth->Parameters[paramIndex]->Offset, global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
			unsigned char* skipRemove = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipRemove, (unsigned char*)compMethod);

			// Remove 1 RootObject reference for popping from stack
			x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
			skipRemove = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipRemove, (unsigned char*)compMethod);
		}

		uint32_t movCount = size / global_SizeOfPointerInBytes;
		for (uint32_t movIndex = 0; movIndex < movCount; ++movIndex)
		{
			x86_pop_reg(compMethod, X86_EAX);
			x86_mov_membase_reg(compMethod, X86_EBP, mth->Parameters[paramIndex]->Offset + (movIndex << 2), X86_EAX, 4);
		}

		if (mth->Parameters[paramIndex]->Type->IsReferenceType)
		{
			// Add reference to RootObject for storing a parameter reference
			x86_mov_reg_membase(compMethod, X86_EAX, X86_EBP, (int)mth->Parameters[paramIndex]->Offset, global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
			unsigned char* skipAdd = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_AddReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipAdd, (unsigned char*)compMethod);
		}
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
	//bool_t IsDouble = TRUE;
 	switch (fromType)
	{
		case ConversionArgumentType_I:
		case ConversionArgumentType_U:
		case ConversionArgumentType_I4:
		case ConversionArgumentType_I1:
		case ConversionArgumentType_I2:
		case ConversionArgumentType_U1:
		case ConversionArgumentType_U2:
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
		case ConversionArgumentType_R4:
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

	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 4, global_SizeOfPointerInBytes);
	x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
	unsigned char* skipRemove = (unsigned char*)compMethod;
	x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

	// Remove 1 RootObject reference for popping array from the stack
	x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
	x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
	x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
	x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

	x86_patch(skipRemove, (unsigned char*)compMethod);


	x86_pop_reg(compMethod, X86_EAX); // Index
	x86_pop_reg(compMethod, X86_ECX); // Array
	x86_mov_reg_membase(compMethod, X86_ECX, X86_ECX, 0, global_SizeOfPointerInBytes);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ECX, sizeof(GCArray));
	x86_imul_reg_reg_imm(compMethod, X86_EAX, X86_EAX, sizeOfElementType);
	x86_alu_reg_reg(compMethod, X86_ADD, X86_ECX, X86_EAX);

	if (elementType == ElementType_R4)
	{
		x86_fld_membase(compMethod, X86_ECX, 0, FALSE);
	}
	else if (elementType == ElementType_R8)
	{
		x86_fld_membase(compMethod, X86_ECX, 0, TRUE);
	}
	else
	{
		if (elementType == ElementType_Ref)
		{
			// Add 1 RootObject reference for pushing element to the stack
			x86_mov_reg_membase(compMethod, X86_EAX, X86_ECX, 0, global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
			unsigned char* skipAdd = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_AddReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipAdd, (unsigned char*)compMethod);
		}

		uint32_t movCount = sizeOfElementType >> 2;
		for (uint32_t index = 0; index < movCount; ++index)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_ECX, index << 2, 4);
			x86_push_reg(compMethod, X86_EBX);
		}
		uint32_t remCount = sizeOfElementType & 0x03;
		if (remCount)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_ECX, sizeOfElementType - remCount, remCount);
			x86_push_reg(compMethod, X86_EBX);
		}
	}

	return compMethod;
}

char* JIT_Compile_Load_Element_Evil			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_Load_Element_Address		(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRType* type = (IRType*)instr->Arg1;
	uint32_t sizeOfType = StackSizeOfType(type);

	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 4, global_SizeOfPointerInBytes);
	x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
	unsigned char* skipRemove = (unsigned char*)compMethod;
	x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

	// Remove 1 RootObject reference for popping array from the stack
	x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
	x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
	x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
	x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

	x86_patch(skipRemove, (unsigned char*)compMethod);

	x86_pop_reg(compMethod, X86_EAX); // Index
	x86_pop_reg(compMethod, X86_ECX); // Array
	x86_mov_reg_membase(compMethod, X86_ECX, X86_ECX, 0, global_SizeOfPointerInBytes);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ECX, sizeof(GCArray));
	x86_imul_reg_reg_imm(compMethod, X86_EAX, X86_EAX, sizeOfType);
	x86_alu_reg_reg(compMethod, X86_ADD, X86_EAX, X86_ECX);
	x86_push_reg(compMethod, X86_EAX);
	return compMethod;
}

char* JIT_Compile_Store_Element				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType elementType = *(ElementType*)instr->Arg1;
	//StackObjectType indexType = *(StackObjectType*)instr->Arg2;
	uint32_t sizeOfElementType = 0;
	GetSizeOfElementType(sizeOfElementType, elementType);
	uint32_t alignedSizeOfElementType = sizeOfElementType;
	Align(&alignedSizeOfElementType);

	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, alignedSizeOfElementType + 4, global_SizeOfPointerInBytes);
	x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
	unsigned char* skipRemove = (unsigned char*)compMethod;
	x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

	// Remove 1 RootObject reference for popping array from the stack
	x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
	x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
	x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
	x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

	x86_patch(skipRemove, (unsigned char*)compMethod);

	if (elementType == ElementType_R4)
	{
		x86_adjust_stack(compMethod, -4);
		x86_fst_membase(compMethod, X86_ESP, 0, FALSE, TRUE);
	}
	else if (elementType == ElementType_R8)
	{
		x86_adjust_stack(compMethod, -8);
		x86_fst_membase(compMethod, X86_ESP, 0, TRUE, TRUE);
	}
	else if (elementType == ElementType_Ref)
	{
		// Remove 1 RootObject reference for popping new array reference from stack
		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		skipRemove = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipRemove, (unsigned char*)compMethod);
	}
	//x86_pop_reg(compMethod, X86_EDX); // Value
	x86_mov_reg_membase(compMethod, X86_ECX, X86_ESP, alignedSizeOfElementType, 4); // Index
	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, alignedSizeOfElementType + 4, 4); // Array
	x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, global_SizeOfPointerInBytes);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_EAX, sizeof(GCArray));
	x86_imul_reg_reg_imm(compMethod, X86_ECX, X86_ECX, sizeOfElementType);
	x86_alu_reg_reg(compMethod, X86_ADD, X86_EAX, X86_ECX);
	//x86_mov_membase_reg(compMethod, X86_EAX, 0, X86_EDX, sizeOfElementType);

	if (elementType == ElementType_Ref)
	{
		x86_push_reg(compMethod, X86_EAX); // Save and restore this, needed for actual data copy

		// Remove 1 reference for removing the current array element reference
		x86_mov_reg_membase(compMethod, X86_ECX, X86_ESP, alignedSizeOfElementType + global_SizeOfPointerInBytes + 4, global_SizeOfPointerInBytes);
		x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		skipRemove = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_ECX, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipRemove, (unsigned char*)compMethod);

		// Add 1 reference for adding the new array element reference
		x86_mov_reg_membase(compMethod, X86_ECX, X86_ESP, alignedSizeOfElementType + global_SizeOfPointerInBytes + 4, global_SizeOfPointerInBytes);
		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, global_SizeOfPointerInBytes, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		unsigned char* skipAdd = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_ECX, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_AddReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipAdd, (unsigned char*)compMethod);

		x86_pop_reg(compMethod, X86_EAX);
	}

	uint32_t movCount = sizeOfElementType >> 2;
	for (uint32_t index = 0; index < movCount; ++index)
	{
		x86_pop_reg(compMethod, X86_EBX);
		x86_mov_membase_reg(compMethod, X86_EAX, index << 2, X86_EBX, 4);
	}
	uint32_t remCount = sizeOfElementType & 0x03;
	if (remCount)
	{
		x86_pop_reg(compMethod, X86_EBX);
		x86_mov_membase_reg(compMethod, X86_EAX, sizeOfElementType - remCount, X86_EBX, remCount);
	}
	x86_adjust_stack(compMethod, 8);

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
	ElementType et = *((ElementType*)instr->Arg1);

	if (et == ElementType_Ref)
	{
		// Remove 1 RootObject reference for popping element from the stack
		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		unsigned char* skipRemove = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipRemove, (unsigned char*)compMethod);
	}

	switch(et)
	{
		case ElementType_I:
		case ElementType_U:
		case ElementType_I1:
		case ElementType_U1:
		case ElementType_I2:
		case ElementType_U2:
		case ElementType_I4:
		case ElementType_U4:
		case ElementType_Ref:
		case ElementType_ManagedPointer:
			x86_adjust_stack(compMethod, 4);
			break;
		case ElementType_I8:
		case ElementType_U8:
			x86_adjust_stack(compMethod, 8);
			break;
		case ElementType_R4:
		case ElementType_R8:
			x86_fdecstp(compMethod);
			break;
		case ElementType_DataType:
			{
				uint32_t tSz = StackSizeOfType((IRType*)instr->Arg2);
				Align(&tSz);
				// Repeated popping would just be slow,
				// and, as we're not using the values,
				// it's easier just to move the stack pointer.
				x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, tSz);
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
		case ShiftType_Right_Sign_Extended:
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
							x86_shift_reg(compMethod, X86_SHR, X86_EAX);
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
								x86_shift_reg(compMethod, X86_SHR, X86_EAX);
								x86_mov_membase_reg(compMethod, X86_ESP, 4, X86_EAX, 4);
								unsigned char* jumpEnd = (unsigned char *)compMethod;
								x86_jump8(compMethod, 0);
								x86_patch(jumpHighPartIsZero, (unsigned char *)compMethod);
								// 33-64bit shift
								x86_alu_reg_imm(compMethod, X86_AND, X86_ECX, 0x1F);
								x86_shift_reg(compMethod, X86_SHR, X86_EAX);
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
		case OverflowType_Signed:
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
					if (instr->HasConstant32Arg1)
					{
						if (instr->Constant32Arg1 == 1)
							x86_inc_membase(compMethod, X86_ESP, 0);
						else
							x86_alu_membase_imm(compMethod, X86_ADD, X86_ESP, 0, instr->Constant32Arg1);
					}
					else if (instr->HasConstant32Arg2)
					{
						if (instr->Constant32Arg2 == 1)
							x86_inc_membase(compMethod, X86_ESP, 0);
						else
							x86_alu_membase_imm(compMethod, X86_ADD, X86_ESP, 0, instr->Constant32Arg2);
					}
					else
					{
						x86_pop_reg(compMethod, X86_EAX);
						x86_alu_membase_reg(compMethod, X86_ADD, X86_ESP, 0, X86_EAX);
					}
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
		case OverflowType_Signed:
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
					if (instr->HasConstant32Arg1)
					{
						if (instr->Constant32Arg1 == 1)
							x86_dec_membase(compMethod, X86_ESP, 0);
						else
							x86_alu_membase_imm(compMethod, X86_SUB, X86_ESP, 0, instr->Constant32Arg1);
					}
					else if (instr->HasConstant32Arg2)
					{
						x86_mov_reg_imm(compMethod, X86_EAX, instr->Constant32Arg2);
						x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, 0, 4);
						x86_alu_reg_reg(compMethod, X86_SUB, X86_EAX, X86_EBX);
						x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_EAX, 4);
					}
					else
					{
						x86_pop_reg(compMethod, X86_EAX);
						x86_alu_membase_reg(compMethod, X86_SUB, X86_ESP, 0, X86_EAX);
					}
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
		case OverflowType_Signed:
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
					x86_pop_reg(compMethod, X86_EBX);
					x86_imul_reg_reg(compMethod, X86_EAX, X86_EBX);
					x86_push_reg(compMethod, X86_EAX);
					break;
				case ElementType_I8:
				case ElementType_U8:
					x86_fild_membase(compMethod, X86_ESP, 8, TRUE);
					x86_fild_membase(compMethod, X86_ESP, 0, TRUE);
					x86_adjust_stack(compMethod, 8);
					x86_fp_op_reg(compMethod, X86_FMUL, 1, TRUE);
					x86_fist_pop_membase(compMethod, X86_ESP, 0, TRUE);
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
		case OverflowType_Signed:
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
					x86_fild_membase(compMethod, X86_ESP, 4, FALSE);
					x86_fild_membase(compMethod, X86_ESP, 0, FALSE);
					x86_adjust_stack(compMethod, 4);
					x86_fp_op_reg(compMethod, X86_FDIV, 1, TRUE);
					x86_fisttp_membase(compMethod, X86_ESP, 0, FALSE);
					break;
				case ElementType_I8:
				case ElementType_U8:
					x86_fild_membase(compMethod, X86_ESP, 8, TRUE);
					x86_fild_membase(compMethod, X86_ESP, 0, TRUE);
					x86_adjust_stack(compMethod, 8);
					x86_fp_op_reg(compMethod, X86_FDIV, 1, TRUE);
					x86_fisttp_membase(compMethod, X86_ESP, 0, TRUE);
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
		case OverflowType_Unsigned:
		case OverflowType_Signed:
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
					x86_fild_membase(compMethod, X86_ESP, 0, FALSE);
					x86_fild_membase(compMethod, X86_ESP, 4, FALSE);
					x86_adjust_stack(compMethod, 4);
					x86_fprem(compMethod);
					x86_fisttp_membase(compMethod, X86_ESP, 0, FALSE);
					x86_fdecstp(compMethod);
					break;
				case ElementType_I8:
				case ElementType_U8:
					x86_fild_membase(compMethod, X86_ESP, 0, TRUE);
					x86_fild_membase(compMethod, X86_ESP, 8, TRUE);
					x86_adjust_stack(compMethod, 8);
					x86_fprem(compMethod);
					x86_fisttp_membase(compMethod, X86_ESP, 0, TRUE);
					x86_fdecstp(compMethod);
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
	if (elementType == ElementType_R4)
	{
		x86_fld_membase(compMethod, X86_ECX, 0, FALSE);
	}
	else if (elementType == ElementType_R8)
	{
		x86_fld_membase(compMethod, X86_ECX, 0, TRUE);
	}
	else
	{
		uint32_t movCount = sizeOfElementType >> 2;
		for (uint32_t index = 0; index < movCount; ++index)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_ECX, index << 2, 4);
			x86_push_reg(compMethod, X86_EBX);
		}
		uint32_t remCount = sizeOfElementType & 0x03;
		if (remCount)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_ECX, sizeOfElementType - remCount, remCount);
			x86_push_reg(compMethod, X86_EBX);
		}
	}
	return compMethod;
}


char* JIT_Compile_StoreIndirect				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType elementType = *(ElementType*)instr->Arg1;
	uint32_t sizeOfElementType = 0;
	GetSizeOfElementType(sizeOfElementType, elementType);
	uint32_t alignedSizeOfElementType = sizeOfElementType;
	Align(&alignedSizeOfElementType);

	if (elementType == ElementType_R4)
	{
		x86_adjust_stack(compMethod, -4);
		x86_fst_membase(compMethod, X86_ESP, 0, FALSE, TRUE);
	}
	else if (elementType == ElementType_R8)
	{
		x86_adjust_stack(compMethod, -8);
		x86_fst_membase(compMethod, X86_ESP, 0, TRUE, TRUE);
	}
	//x86_pop_reg(compMethod, X86_EAX); // Value
	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, alignedSizeOfElementType, 4); // Pointer
	//x86_mov_membase_reg(compMethod, X86_ECX, 0, X86_EAX, sizeOfElementType);

	uint32_t movCount = sizeOfElementType >> 2;
	for (uint32_t index = 0; index < movCount; ++index)
	{
		x86_pop_reg(compMethod, X86_EBX);
		x86_mov_membase_reg(compMethod, X86_EAX, index << 2, X86_EBX, 4);
	}
	uint32_t remCount = sizeOfElementType & 0x03;
	if (remCount)
	{
		x86_pop_reg(compMethod, X86_EBX);
		x86_mov_membase_reg(compMethod, X86_EAX, sizeOfElementType - remCount, X86_EBX, remCount);
	}
	x86_adjust_stack(compMethod, 4);
	return compMethod;
}


char* JIT_Compile_LoadNull					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_push_imm(compMethod, (unsigned int)0);
	return compMethod;
}


char* JIT_Compile_Dup						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType elemTp = *(ElementType*)instr->Arg1;
	uint32_t size = 0;
	GetSizeOfElementType(size, elemTp);
	Align(&size);

	if (elemTp == ElementType_Ref)
	{
		// Add 1 RootObject reference for pushing duped element to the stack
		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		unsigned char* skipAdd = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_AddReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipAdd, (unsigned char*)compMethod);
	}

	x86_mov_reg_reg(compMethod, X86_EAX, X86_ESP, 4);
	x86_adjust_stack(compMethod, -((int)size));
	for (uint32_t i = 0; i < size; i += 4)
	{
		x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, i, 4);
		x86_mov_membase_reg(compMethod, X86_ESP, i, X86_EBX, 4);
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
		case ElementType_I2:
		case ElementType_I1:
		case ElementType_U:
		case ElementType_U4:
		case ElementType_U2:
		case ElementType_U1:
			if (arg2Type != ElementType_I && arg2Type != ElementType_I1 && arg2Type != ElementType_I2 && arg2Type != ElementType_I4 && arg2Type != ElementType_U && arg2Type != ElementType_U1 && arg2Type != ElementType_U2 && arg2Type != ElementType_U4)
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
		case ElementType_I2:
		case ElementType_I1:
		case ElementType_U:
		case ElementType_U4:
		case ElementType_U2:
		case ElementType_U1:
			if (arg2Type != ElementType_I && arg2Type != ElementType_I1 && arg2Type != ElementType_I2 && arg2Type != ElementType_I4 && arg2Type != ElementType_U && arg2Type != ElementType_U1 && arg2Type != ElementType_U2 && arg2Type != ElementType_U4)
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
		case ElementType_I2:
		case ElementType_I1:
		case ElementType_U:
		case ElementType_U4:
		case ElementType_U2:
		case ElementType_U1:
			if (arg2Type != ElementType_I && arg2Type != ElementType_I1 && arg2Type != ElementType_I2 && arg2Type != ElementType_I4 && arg2Type != ElementType_U && arg2Type != ElementType_U1 && arg2Type != ElementType_U2 && arg2Type != ElementType_U4)
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

	// Add 1 RootObject reference for pushing to the stack
	x86_mov_reg_imm(compMethod, X86_EAX, (unsigned int)str);
	x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
	x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
	x86_call_code(compMethod, ReferenceTypeObject_AddReference);
	x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

	return compMethod;
}

char* JIT_Compile_Load_Field				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRFieldSpec* spec = (IRFieldSpec*)instr->Arg1;
	JIT_Layout_Fields(spec->ParentType);

	uint32_t fSz = StackSizeOfType(spec->FieldType);
	uint32_t alSz = fSz;
	Align(&alSz);

	// Remove 1 RootObject reference for popping object from the stack
	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, global_SizeOfPointerInBytes);
	x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
	unsigned char* skipRemove = (unsigned char*)compMethod;
	x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

	x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
	x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
	x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
	x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

	x86_patch(skipRemove, (unsigned char*)compMethod);

	x86_pop_reg(compMethod, X86_EAX);

	x86_mov_reg_reg(compMethod, X86_ECX, X86_EAX, 4);
	x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, 4);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_EAX, spec->ParentType->Fields[spec->FieldIndex]->Offset);

	if (spec->FieldType->IsReferenceType)
	{
		x86_push_reg(compMethod, X86_EAX);

		// Add 1 RootObject reference for pushing field to the stack
		x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		unsigned char* skipRemove = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_AddReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipRemove, (unsigned char*)compMethod);

		x86_pop_reg(compMethod, X86_EAX);
	}


	x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, alSz);

	uint32_t movCount = fSz / global_SizeOfPointerInBytes;
	uint32_t curBas = 0;
	for (uint32_t i = 0; i < movCount; i++)
	{
		x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, curBas, 4);
		x86_mov_membase_reg(compMethod, X86_ESP, curBas, X86_EBX, 4);
		curBas += 4;
	}
	
	uint32_t modFSz = fSz % 4;
	if (modFSz)
	{
		x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, curBas, modFSz);
		x86_mov_membase_reg(compMethod, X86_ESP, curBas, X86_EBX, modFSz);
	}

	if (spec->FieldType->TypeDef == spec->FieldType->ParentAssembly->ParentDomain->CachedType___System_Single)
	{
		x86_fld_membase(compMethod, X86_ESP, 0, FALSE);
		x86_adjust_stack(compMethod, 4);
	}
	else if (spec->FieldType->TypeDef == spec->FieldType->ParentAssembly->ParentDomain->CachedType___System_Double)
	{
		x86_fld_membase(compMethod, X86_ESP, 0, TRUE);
		x86_adjust_stack(compMethod, 8);
	}

	return compMethod;
}

char* JIT_Compile_Load_Field_Address		(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRFieldSpec* spec = (IRFieldSpec*)instr->Arg1;
	JIT_Layout_Fields(spec->ParentType);

	// Remove 1 RootObject reference for popping object from the stack
	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, global_SizeOfPointerInBytes);
	x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
	unsigned char* skipRemove = (unsigned char*)compMethod;
	x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

	x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
	x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
	x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
	x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

	x86_patch(skipRemove, (unsigned char*)compMethod);

	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, 4); // Get the RTO
	x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, 4); // Get the object
	x86_alu_reg_imm(compMethod, X86_ADD, X86_EAX, spec->ParentType->Fields[spec->FieldIndex]->Offset); // Get the field
	x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_EAX, 4); // Store the field address

	return compMethod;
}

char* JIT_Compile_Store_Field				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRFieldSpec* spec = (IRFieldSpec*)instr->Arg1;
	JIT_Layout_Fields(spec->ParentType);

	uint32_t fSz = StackSizeOfType(spec->FieldType);
	uint32_t alSz = fSz;
	Align(&alSz);

	// Remove 1 RootObject reference for popping object from the stack
	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, alSz, global_SizeOfPointerInBytes);
	x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
	unsigned char* skipRemove = (unsigned char*)compMethod;
	x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

	x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
	x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
	x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
	x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

	x86_patch(skipRemove, (unsigned char*)compMethod);

	if (spec->FieldType->IsReferenceType)
	{
		// Remove 1 RootObject reference for popping value from the stack
		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		skipRemove = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipRemove, (unsigned char*)compMethod);
	}

	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, alSz, 4); // Get the RTO.
	x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, 4); // Get the object
	// The pointer to the start of the type is now in EAX.

	x86_alu_reg_imm(compMethod, X86_ADD, X86_EAX, spec->ParentType->Fields[spec->FieldIndex]->Offset);
	// The pointer to the start of the field is now in EAX.

	if (spec->FieldType->IsReferenceType)
	{
		x86_mov_reg_membase(compMethod, X86_ECX, X86_ESP, alSz, 4);
		x86_push_reg(compMethod, X86_EAX);

		// Remove 1 reference of old value from object
		x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		skipRemove = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_ECX, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipRemove, (unsigned char*)compMethod);

		// Add 1 reference of value to object
		x86_mov_reg_membase(compMethod, X86_ECX, X86_ESP, alSz + 4, 4);
		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 4, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		unsigned char* skipAdd = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_ECX, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_AddReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipAdd, (unsigned char*)compMethod);

		x86_pop_reg(compMethod, X86_EAX);
	}
	else if (spec->FieldType->TypeDef == spec->FieldType->ParentAssembly->ParentDomain->CachedType___System_Single)
	{
		x86_adjust_stack(compMethod, -4);
		x86_fst_membase(compMethod, X86_ESP, 0, FALSE, TRUE);
	}
	else if (spec->FieldType->TypeDef == spec->FieldType->ParentAssembly->ParentDomain->CachedType___System_Double)
	{
		x86_adjust_stack(compMethod, -8);
		x86_fst_membase(compMethod, X86_ESP, 0, TRUE, TRUE);
	}
	uint32_t movCount = fSz / global_SizeOfPointerInBytes;
	uint32_t curBas = 0;
	for (uint32_t i = 0; i < movCount; i++)
	{
		x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, curBas, 4);
		x86_mov_membase_reg(compMethod, X86_EAX, curBas, X86_EBX, 4);
		curBas += 4;
	}

	uint32_t modFSz = fSz % 4;
	if (modFSz)
	{
		x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, curBas, modFSz);
		x86_mov_membase_reg(compMethod, X86_EAX, curBas, X86_EBX, modFSz);
	}
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, alSz + 4);

	return compMethod;
}

ALWAYS_INLINE char* EmitStaticConstructorCall(char* compMethod, IRType* tp)
{
	if (tp->HasStaticConstructor)
	{
		printf("EmitStaticConstructorCall: tp @ 0x%x, Called = %u\n", (unsigned int)tp, (unsigned int)tp->StaticConstructorCalled);
		x86_alu_reg_reg(compMethod, X86_XOR, X86_EAX, X86_EAX);
		x86_mov_reg_mem(compMethod, X86_EAX, &tp->StaticConstructorCalled, 1);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);

		unsigned char* finalBranch = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_NZ, 0, FALSE);
		x86_mov_mem_imm(compMethod, &tp->StaticConstructorCalled, 1, 1);

		if (!tp->StaticConstructor->AssembledMethod)
			JIT_CompileMethod(tp->StaticConstructor);
		x86_push_imm(compMethod, (unsigned int)tp->ParentAssembly->ParentDomain);
		x86_call_code(compMethod, tp->StaticConstructor->AssembledMethod);
		x86_adjust_stack(compMethod, 4);


		x86_patch(finalBranch, (unsigned char*)compMethod);

		x86_mov_mem_imm(compMethod, (unsigned int)finalBranch - 13, 0xE9, 1); // Now patch ourselves.
		uint32_t compMthVal = (uint32_t)compMethod;
		x86_mov_mem_imm(compMethod, (unsigned int)finalBranch - 12, (compMthVal - (uint32_t)finalBranch + 18), 4); 
	}
	return compMethod;
}

char* JIT_Compile_Load_Static_Field			(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRField* fld = (IRField*)instr->Arg1;
	compMethod = EmitStaticConstructorCall(compMethod, fld->ParentType);

	uint32_t fieldSize = StackSizeOfType(fld->FieldType);
	Align(&fieldSize);
	if (!fld->StaticValue)
	{
		fld->StaticValue = (void*)calloc(1, fieldSize);
		printf("Allocating a static field @ 0x%x fieldtype = 0x%x (size %i) at 0x%x\n", (unsigned int)fld, (unsigned int)fld->FieldType, (int)fieldSize, (unsigned int)fld->StaticValue);
	}

	x86_mov_reg_imm(compMethod, X86_EAX, (unsigned int)fld->StaticValue);
	if (fld->FieldType->IsValueType)
	{
		printf("LoadStaticField: Value Type = %s @ 0x%x\n", fld->FieldType->TypeDef->Name, (unsigned int)fld->StaticValue);
		x86_adjust_stack(compMethod, -((int32_t)fieldSize));
		uint32_t movCount = fieldSize / global_SizeOfPointerInBytes;
		uint32_t curBas = 0;
		for (uint32_t i = 0; i < movCount; i++)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, i << 2, 4);
			x86_mov_membase_reg(compMethod, X86_ESP, curBas, X86_EBX, 4);
			curBas += 4;
		}

		/*uint32_t movCount = fieldSize >> 2;
		for (uint32_t index = 0; index < movCount; ++index)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, index << 2, 4);
			x86_push_reg(compMethod, X86_EBX);
		}*/
		
		if (fld->FieldType->TypeDef == fld->ParentAssembly->ParentDomain->CachedType___System_Single)
		{
			x86_fld_membase(compMethod, X86_ESP, 0, FALSE);
			x86_adjust_stack(compMethod, 4);
		}
		else if (fld->FieldType->TypeDef == fld->ParentAssembly->ParentDomain->CachedType___System_Double)
		{
			x86_fld_membase(compMethod, X86_ESP, 0, TRUE);
			x86_adjust_stack(compMethod, 8);
		}
	}
	else
	{
		printf("LoadStaticField: Ref Type = %s @ 0x%x\n", fld->FieldType->TypeDef->Name, (unsigned int)fld->StaticValue);
		x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, 4);
		x86_push_reg(compMethod, X86_EAX);

		// Add 1 RootObject reference for pushing field to the stack
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		unsigned char* skipAdd = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_AddReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipAdd, (unsigned char*)compMethod);
	}

	return compMethod;
}

char* JIT_Compile_Load_Static_Field_Address	(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRField* fld = (IRField*)instr->Arg1;
	compMethod = EmitStaticConstructorCall(compMethod, fld->ParentType);
	uint32_t fieldSize = StackSizeOfType(fld->FieldType);
	Align(&fieldSize);
	if (!fld->StaticValue)
		fld->StaticValue = (void*)calloc(1, fieldSize);
	printf("LoadStaticFieldAddress: Type = %s @ 0x%x\n", fld->FieldType->TypeDef->Name, (unsigned int)fld->StaticValue);
	x86_push_imm(compMethod, (unsigned int)fld->StaticValue);
	return compMethod;
}

char* JIT_Compile_Store_Static_Field		(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRField* fld = (IRField*)instr->Arg1;
	compMethod = EmitStaticConstructorCall(compMethod, fld->ParentType);
	uint32_t fieldSize = StackSizeOfType(fld->FieldType);
	Align(&fieldSize);
	if (!fld->StaticValue)
	{
		fld->StaticValue = (void*)calloc(1, fieldSize);
		printf("Allocating a static field @ 0x%x fieldtype = 0x%x (size %i) at 0x%x\n", (unsigned int)fld, (unsigned int)fld->FieldType, (int)fieldSize, (unsigned int)fld->StaticValue);
	}

	x86_mov_reg_imm(compMethod, X86_EAX, (unsigned int)fld->StaticValue);
	if (fld->FieldType->IsValueType)
	{	
		printf("StoreStaticField: Value Type = %s @ 0x%x\n", fld->FieldType->TypeDef->Name, (unsigned int)fld->StaticValue);
		if (fld->FieldType->TypeDef == fld->ParentAssembly->ParentDomain->CachedType___System_Single)
		{
			x86_fst_membase(compMethod, X86_EAX, 0, FALSE, TRUE);
		}
		else if (fld->FieldType->TypeDef == fld->ParentAssembly->ParentDomain->CachedType___System_Double)
		{
			x86_fst_membase(compMethod, X86_EAX, 0, TRUE, TRUE);
		}
		else 
		{
			uint32_t movCount = fieldSize / global_SizeOfPointerInBytes;
			uint32_t curBas = 0;
			for (uint32_t i = 0; i < movCount; i++)
			{
				x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, curBas, 4);
				x86_mov_membase_reg(compMethod, X86_EAX, i << 2, X86_EBX, 4);
				curBas += 4;
			}
			x86_adjust_stack(compMethod, (int32_t)fieldSize);
		}
	}
	else
	{
		printf("StoreStaticField: Ref Type = %s @ 0x%x\n", fld->FieldType->TypeDef->Name, (unsigned int)fld->StaticValue);
		x86_push_reg(compMethod, X86_EAX); // Save this, need it in the middle of the following code

		// Remove 1 RootObject reference for popping value from the stack
		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, global_SizeOfPointerInBytes, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		unsigned char* skipRemove = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipRemove, (unsigned char*)compMethod);


		// Remove 1 RootObject reference for removing value from the static field
		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, global_SizeOfPointerInBytes);
		x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		skipRemove = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipRemove, (unsigned char*)compMethod);


		x86_pop_reg(compMethod, X86_EAX);
		x86_pop_reg(compMethod, X86_EBX);
		x86_mov_membase_reg(compMethod, X86_EAX, 0, X86_EBX, 4);


		// Add 1 RootObject reference for setting the static field
		x86_test_reg_reg(compMethod, X86_EBX, X86_EBX);
		unsigned char* skipAdd = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EBX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_AddReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipAdd, (unsigned char*)compMethod);

		x86_pop_reg(compMethod, X86_EAX);
	}

	return compMethod;
}


char* JIT_Compile_Load_Object				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType et = *(ElementType*)instr->Arg1;
	IRType* type = (IRType*)instr->Arg2;
	if ((et == ElementType_I || et == ElementType_U || et == ElementType_ManagedPointer) && global_SizeOfPointerInBytes != 4) Panic("This should not happen on x86, pointers can only be 32bit");
	x86_pop_reg(compMethod, X86_EAX);

	if (type->IsValueType)
	{
		uint32_t sizeOfType = StackSizeOfType(type);
		uint32_t movCount = sizeOfType >> 2;
		for (uint32_t index = 0; index < movCount; ++index)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, index << 2, 4);
			x86_push_reg(compMethod, X86_EBX);
		}
		uint32_t remCount = sizeOfType & 0x03;
		if (remCount)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, sizeOfType - remCount, remCount);
			x86_push_reg(compMethod, X86_EBX);
		}
		
		if (type->TypeDef == type->ParentAssembly->ParentDomain->CachedType___System_Single)
		{
			x86_fld_membase(compMethod, X86_ESP, 0, FALSE);
			x86_adjust_stack(compMethod, 4);
		}
		else if (type->TypeDef == type->ParentAssembly->ParentDomain->CachedType___System_Double)
		{
			x86_fld_membase(compMethod, X86_ESP, 0, TRUE);
			x86_adjust_stack(compMethod, 8);
		}
	}
	else
	{
		x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, 4);
		x86_push_reg(compMethod, X86_EAX);

		// Add 1 RootObject reference for pushing the object to the stack
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		unsigned char* skipAdd = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_AddReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipAdd, (unsigned char*)compMethod);
	}

	return compMethod;
}

char* JIT_Compile_Store_Object				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType et = *(ElementType*)instr->Arg1;
	IRType* type = (IRType*)instr->Arg2;
	uint32_t sizeOfType = StackSizeOfType(type);
	uint32_t alignedSizeOfType = sizeOfType;
	Align(&alignedSizeOfType);

	if (et == ElementType_R4)
	{
		x86_adjust_stack(compMethod, -4);
		x86_fst_membase(compMethod, X86_ESP, 0, FALSE, TRUE);
	}
	else if (et == ElementType_R8)
	{
		x86_adjust_stack(compMethod, -8);
		x86_fst_membase(compMethod, X86_ESP, 0, TRUE, TRUE);
	}

	x86_mov_reg_reg(compMethod, X86_EAX, X86_ESP, 4); // Source
	x86_mov_reg_membase(compMethod, X86_ECX, X86_ESP, alignedSizeOfType, 4); // Destination

	if (type->IsValueType)
	{
		uint32_t movCount = sizeOfType >> 2;
		for (uint32_t index = 0; index < movCount; ++index)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, index << 2, 4);
			x86_mov_membase_reg(compMethod, X86_ECX, index << 2, X86_EBX, 4);
		}
		uint32_t remCount = sizeOfType & 0x03;
		if (remCount)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, sizeOfType - remCount, remCount);
			x86_mov_membase_reg(compMethod, X86_ECX, sizeOfType - remCount, X86_EBX, remCount);
		}
	}
	else
	{
		// Remove 1 RootObject reference for popping from the stack
		x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		unsigned char* skipRemove = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipRemove, (unsigned char*)compMethod);

		// Remove 1 RootObject reference if value at destination is not null
		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, global_SizeOfPointerInBytes, global_SizeOfPointerInBytes);
		x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, global_SizeOfPointerInBytes);
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		skipRemove = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipRemove, (unsigned char*)compMethod);

		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, global_SizeOfPointerInBytes); // Source
		x86_mov_reg_membase(compMethod, X86_ECX, X86_ESP, global_SizeOfPointerInBytes, global_SizeOfPointerInBytes); // Destination
		x86_mov_membase_reg(compMethod, X86_ECX, 0, X86_EAX, global_SizeOfPointerInBytes);

		// Add 1 RootObject reference for copying the reference to another location
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		unsigned char* skipAdd = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_AddReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipAdd, (unsigned char*)compMethod);
	}

	return compMethod;
}

char* JIT_Compile_Copy_Object				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType et = *(ElementType*)instr->Arg1;
	IRType* type = (IRType*)instr->Arg2;
	if ((et == ElementType_I || et == ElementType_U || et == ElementType_ManagedPointer) && global_SizeOfPointerInBytes != 4) Panic("This should not happen on x86, pointers can only be 32bit");
	x86_pop_reg(compMethod, X86_EAX); // Source
	x86_pop_reg(compMethod, X86_ECX); // Destination

	if (type->IsValueType)
	{
		uint32_t sizeOfType = StackSizeOfType(type);
		uint32_t movCount = sizeOfType >> 2;
		for (uint32_t index = 0; index < movCount; ++index)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, index << 2, 4);
			x86_mov_membase_reg(compMethod, X86_ECX, index << 2, X86_EBX, 4);
		}
		uint32_t remCount = sizeOfType & 0x03;
		if (remCount)
		{
			x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, sizeOfType - remCount, remCount);
			x86_mov_membase_reg(compMethod, X86_ECX, sizeOfType - remCount, X86_EBX, remCount);
		}
	}
	else
	{
		x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, 4);
		x86_mov_membase_reg(compMethod, X86_ECX, 0, X86_EAX, 4);

		// Add 1 RootObject reference for copying the reference to another location
		x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
		unsigned char* skipAdd = (unsigned char*)compMethod;
		x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

		x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
		x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
		x86_call_code(compMethod, ReferenceTypeObject_AddReference);
		x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

		x86_patch(skipAdd, (unsigned char*)compMethod);
	}

	return compMethod;
}


char* JIT_Compile_Switch					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	uint32_t targetCount = *(uint32_t*)instr->Arg1;
	IRInstruction** targets = (IRInstruction**)instr->Arg2;

	x86_pop_reg(compMethod, X86_EAX);
	x86_alu_reg_imm(compMethod, X86_CMP, X86_EAX, targetCount);
	unsigned char* jumpToDefault = (unsigned char*)compMethod;
	x86_branch32(compMethod, X86_CC_AE, 0, FALSE);
	x86_shift_reg_imm(compMethod, X86_SHL, X86_EAX, 2);
	unsigned char* skipTable = (unsigned char*)compMethod;
	x86_jump32(compMethod, 0);
	uint32_t switchTableLocation = (uint32_t)compMethod;
	for (uint32_t index = 0; index < targetCount; ++index)
	{
		BranchRegistry_RegisterSpecialBranchForLink(branchRegistry, instr->InstructionLocation + (index << 2), targets[index]->InstructionLocation, compMethod);
		x86_imm_emit32(compMethod, targets[index]->InstructionLocation);
	}
	x86_patch(skipTable, (unsigned char*)compMethod);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_EAX, switchTableLocation);
	x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, 4);
	x86_jump_reg(compMethod, X86_EAX);
	// After this is the default case.
	x86_patch(jumpToDefault, (unsigned char*)compMethod);

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
	//IRType* type = (IRType*)instr->Arg1;

	// Remove 1 RootObject reference for popping object from the stack
	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, global_SizeOfPointerInBytes);
	x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
	unsigned char* skipRemove = (unsigned char*)compMethod;
	x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

	x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
	x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
	x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
	x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

	x86_patch(skipRemove, (unsigned char*)compMethod);

	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, 4);
	x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, 4);
	x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_EAX, 4);

	return compMethod;
}

char* JIT_Compile_Unbox_Any					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRType* type = (IRType*)instr->Arg1;
	uint32_t sizeOfType = StackSizeOfType(type);

	// Remove 1 RootObject reference for popping object from the stack
	x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 0, global_SizeOfPointerInBytes);
	x86_test_reg_reg(compMethod, X86_EAX, X86_EAX);
	unsigned char* skipRemove = (unsigned char*)compMethod;
	x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

	x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
	x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EAX, global_SizeOfPointerInBytes);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)mth->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
	x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
	x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

	x86_patch(skipRemove, (unsigned char*)compMethod);

	x86_pop_reg(compMethod, X86_EAX);
	x86_mov_reg_membase(compMethod, X86_EAX, X86_EAX, 0, 4);
	uint32_t alignedSizeOfType = sizeOfType;
	Align(&alignedSizeOfType);
	x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, alignedSizeOfType);
	uint32_t movCount = sizeOfType >> 2;
	for (uint32_t index = 0; index < movCount; ++index)
	{
		x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, index << 2, 4);
		x86_mov_membase_reg(compMethod, X86_ESP, index << 2, X86_EBX, 4);
	}
	uint32_t remCount = sizeOfType & 0x03;
	if (remCount)
	{
		x86_mov_reg_membase(compMethod, X86_EBX, X86_EAX, sizeOfType - remCount, remCount);
		x86_mov_membase_reg(compMethod, X86_ESP, sizeOfType - remCount, X86_EBX, remCount);
	}
	if (type->TypeDef == type->ParentAssembly->ParentDomain->CachedType___System_Single)
	{
		x86_fld_membase(compMethod, X86_ESP, 0, FALSE);
		x86_adjust_stack(compMethod, 4);
	}
	else if (type->TypeDef == type->ParentAssembly->ParentDomain->CachedType___System_Double)
	{
		x86_fld_membase(compMethod, X86_ESP, 0, TRUE);
		x86_adjust_stack(compMethod, 8);
	}

	return compMethod;
}

char* JIT_Compile_Box						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRType* type = (IRType*)instr->Arg1;
	uint32_t sizeOfType = StackSizeOfType(type);

	if (type->TypeDef == type->ParentAssembly->ParentDomain->CachedType___System_Single)
	{
		x86_adjust_stack(compMethod, -4);
		x86_fst_membase(compMethod, X86_ESP, 0, FALSE, TRUE);
	}
	else if (type->TypeDef == type->ParentAssembly->ParentDomain->CachedType___System_Double)
	{
		x86_adjust_stack(compMethod, -8);
		x86_fst_membase(compMethod, X86_ESP, 0, TRUE, TRUE);
	}

	x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 16);
	x86_mov_membase_imm(compMethod, X86_ESP, 12, (uint32_t)type, 4);
	x86_mov_membase_imm(compMethod, X86_ESP, 8, sizeOfType, 4);
	x86_mov_membase_imm(compMethod, X86_ESP, 4, (int)type->ParentAssembly->ParentDomain->RootObject, 4);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (int)type->ParentAssembly->ParentDomain->GarbageCollector, 4);
	x86_call_code(compMethod, GC_AllocateObject);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 16);
	x86_mov_reg_reg(compMethod, X86_ECX, X86_EAX, 4);
	x86_mov_reg_reg(compMethod, X86_EDX, X86_EAX, 4);
	x86_mov_reg_membase(compMethod, X86_ECX, X86_ECX, 0, 4);
	uint32_t movCount = sizeOfType >> 2;
	for (uint32_t index = 0; index < movCount; ++index)
	{
		x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, index << 2, 4);
		x86_mov_membase_reg(compMethod, X86_ECX, index << 2, X86_EBX, 4);
	}
	uint32_t remCount = sizeOfType & 0x03;
	if (remCount)
	{
		x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, sizeOfType - remCount, remCount);
		x86_mov_membase_reg(compMethod, X86_ECX, sizeOfType - remCount, X86_EBX, remCount);
	}
	uint32_t alignedSizeOfType = sizeOfType;
	Align(&alignedSizeOfType);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, alignedSizeOfType);
	x86_push_reg(compMethod, X86_EDX);

	return compMethod;
}


char* JIT_Compile_Compare					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	CompareCondition cond = *(CompareCondition*)instr->Arg1;
	ElementType et1 = *(ElementType*)instr->Arg2;
	ElementType et2 = *(ElementType*)instr->Arg3;
	uint32_t sizeOfET1 = 0;
	uint32_t sizeOfET2 = 0;
	GetSizeOfElementType(sizeOfET1, et1);
	GetSizeOfElementType(sizeOfET2, et2);
	bool_t fCompare = FALSE;
	bool_t is64bit = FALSE;
	printf("We got a Compare Call!\n");
	switch (et1)
	{
		case ElementType_I:
		case ElementType_U:
			switch (et2)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I4:
				case ElementType_U4:
				case ElementType_I2:
				case ElementType_I1:
				case ElementType_U2:
				case ElementType_U1:
				case ElementType_ManagedPointer:
					if (et2 == ElementType_ManagedPointer && cond != CompareCondition_Equal) Panic("Invalid arguments for CompareCondition Equal");
					break;
				default:
					Panic("Invalid arguments for Compare");
					break;
			}
			break;
		case ElementType_I4:
		case ElementType_U4:
		case ElementType_I2:
		case ElementType_I1:
		case ElementType_U2:
		case ElementType_U1:
			switch (et2)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I4:
				case ElementType_U4:
				case ElementType_I2:
				case ElementType_I1:
				case ElementType_U2:
				case ElementType_U1:
					break;
				default:
					Panic("Invalid arguments for Compare");
					break;
			}
			break;
		case ElementType_I8:
		case ElementType_U8:
			if (et2 != ElementType_I8 && et2 != ElementType_U8) Panic("Invalid arguments for Compare");
			is64bit = TRUE;
			break;
		case ElementType_R4:
		case ElementType_R8:
			switch (et2)
			{
				case ElementType_R4:
				case ElementType_R8:
					fCompare = TRUE;
					break;
				default:
					Panic("Invalid arguments for Compare");
					break;
			}
			break;
		case ElementType_ManagedPointer:
			switch (et2)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_ManagedPointer:
					break;
				default:
					Panic("Invalid arguments for Compare");
					break;
			}
			break;
		case ElementType_Ref:
			//if (et2 != ElementType_Ref)
				Panic("Invalid arguments for Compare");
			break;
		default:
			Panic("Invalid arguments for Compare");
			break;
	}
	if (!fCompare && !is64bit)
	{
		x86_pop_reg(compMethod, X86_EAX);
		x86_alu_membase_reg(compMethod, X86_CMP, X86_ESP, 0, X86_EAX);
	}
	else
	{
		if (is64bit)
		{
			x86_fild_membase(compMethod, X86_ESP, 0, TRUE);
			x86_fild_membase(compMethod, X86_ESP, 8, TRUE);
			x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 12);
		}
		x86_fcomip(compMethod, 1);
		x86_fdecstp(compMethod);
	}
	unsigned char* BranchTrue = NULL;
	unsigned char* BranchFalse = NULL;

	switch (cond)
	{
		case CompareCondition_Equal:
			BranchTrue = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_EQ, 0, FALSE);
			BranchFalse = (unsigned char*)compMethod;
			x86_jump32(compMethod, 0);
			break;
		case CompareCondition_Greater_Than:
			BranchTrue = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_GT, 0, TRUE);
			BranchFalse = (unsigned char*)compMethod;
			x86_jump32(compMethod, 0);
			break;
		case CompareCondition_Greater_Than_Unsigned:
			BranchTrue = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_GT, 0, FALSE);
			BranchFalse = (unsigned char*)compMethod;
			x86_jump32(compMethod, 0);
			break;
		case CompareCondition_Less_Than:
			BranchTrue = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_LT, 0, TRUE);
			BranchFalse = (unsigned char*)compMethod;
			x86_jump32(compMethod, 0);
			break;
		case CompareCondition_Less_Than_Unsigned:
			BranchTrue = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_LT, 0, FALSE);
			BranchFalse = (unsigned char*)compMethod;
			x86_jump32(compMethod, 0);
			break;
		default:
			Panic("Unknown CompareCondition");
			break;
	}
	x86_patch(BranchTrue, (unsigned char*)compMethod);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, 1, 4);
	unsigned char* end = (unsigned char*)compMethod;
	x86_jump32(compMethod, 0);
	x86_patch(BranchFalse, (unsigned char*)compMethod);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, 0, 4);
	x86_patch(end, (unsigned char*)compMethod);

	return compMethod;
}


char* JIT_Compile_CheckFinite				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}


char* JIT_Compile_LocalAllocate				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType et = *(ElementType*)instr->Arg1;
	if (et == ElementType_U && global_SizeOfPointerInBytes != 4) Panic("This should not happen on x86, pointers can only be 32bit");

	x86_pop_reg(compMethod, X86_EAX);
	x86_mov_reg_reg(compMethod, X86_ECX, X86_ESP, 4);
	x86_alu_reg_reg(compMethod, X86_SUB, X86_ESP, X86_EAX);
	x86_push_reg(compMethod, X86_ECX);
	return compMethod;
}


char* JIT_Compile_InitObject				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	ElementType et = *(ElementType*)instr->Arg1;
	IRType* type = (IRType*)instr->Arg2;
	if ((et == ElementType_I || et == ElementType_U || et == ElementType_ManagedPointer) && global_SizeOfPointerInBytes != 4) Panic("This should not happen on x86, pointers can only be 32bit");
	x86_pop_reg(compMethod, X86_EAX);

	if (type->IsValueType)
	{
		uint32_t sizeOfType = StackSizeOfType(type);
		uint32_t movCount = sizeOfType >> 2;
		for (uint32_t index = 0; index < movCount; ++index) x86_mov_membase_imm(compMethod, X86_EAX, index << 2, 0, 4);
		uint32_t remCount = sizeOfType & 0x03;
		if (remCount) x86_mov_membase_imm(compMethod, X86_EAX, sizeOfType - remCount, 0, remCount);
	}
	else x86_mov_membase_imm(compMethod, X86_EAX, 0, 0, 4);

	return compMethod;
}


char* JIT_Compile_SizeOf					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_push_imm(compMethod, SizeOfType((IRType*)instr->Arg1));
	return compMethod;
}


char* JIT_Compile_LoadFunction				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRMethod* method = (IRMethod*)instr->Arg1;
	if (!method->AssembledMethod) JIT_CompileMethod(method);
	x86_push_imm(compMethod, method->AssembledMethod);
	return compMethod;
}

char* JIT_Compile_LoadVirtualFunction		(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	return compMethod;
}

char* JIT_Compile_Copy_Block				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_call_code(compMethod, memcpy);
	x86_adjust_stack(compMethod, 12);
	
	return compMethod;
}

char* JIT_Compile_Init_Block				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_call_code(compMethod, memset);
	x86_adjust_stack(compMethod, 12);

	return compMethod;
}



char* JIT_Compile_NewArray					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRType* type = (IRType*)instr->Arg1;
	uint32_t sizeOfType = StackSizeOfType(type);
	x86_pop_reg(compMethod, X86_EAX); // Number of Elements
	x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 24);
	x86_mov_membase_imm(compMethod, X86_ESP, 16, (uint32_t)type, 4);
	x86_mov_membase_imm(compMethod, X86_ESP, 12, sizeOfType, 4);
	x86_mov_membase_reg(compMethod, X86_ESP, 8, X86_EAX, 4);
	x86_mov_membase_imm(compMethod, X86_ESP, 4, (int)mth->ParentAssembly->ParentDomain->RootObject, 4);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (int)mth->ParentAssembly->ParentDomain->GarbageCollector, 4);
	x86_call_code(compMethod, GC_AllocateArray);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 20);
	x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_EAX, 4);
	return compMethod;
}

char* JIT_Compile_NewObject					(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRMethod* method = (IRMethod*)instr->Arg1;
	IRType* type = method->ParentAssembly->Types[method->MethodDefinition->TypeDefinition->TableIndex - 1];
	if (type->TypeDef != method->ParentAssembly->ParentDomain->CachedType___System_String)
	{
		compMethod = JIT_Emit_ParamSwap(compMethod, method, FALSE);

		uint32_t sizeOfType = SizeOfType(type);
		x86_alu_reg_imm(compMethod, X86_SUB, X86_ESP, 20);
		x86_mov_membase_imm(compMethod, X86_ESP, 12, (uint32_t)type, 4);
		x86_mov_membase_imm(compMethod, X86_ESP, 8, sizeOfType, 4);
		x86_mov_membase_imm(compMethod, X86_ESP, 4, (int)method->ParentAssembly->ParentDomain->RootObject, 4);
		x86_mov_membase_imm(compMethod, X86_ESP, 0, (int)method->ParentAssembly->ParentDomain->GarbageCollector, 4);
		x86_call_code(compMethod, GC_AllocateObject);
		x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 16);
		x86_mov_membase_reg(compMethod, X86_ESP, 0, X86_EAX, 4);
		//x86_push_reg(compMethod, X86_EAX);
	
		if (!method->AssembledMethod) JIT_CompileMethod(method);

		x86_push_imm(compMethod, (int)method->ParentAssembly->ParentDomain);
		x86_call_code(compMethod, method->AssembledMethod);
		x86_mov_reg_membase(compMethod, X86_EAX, X86_ESP, 4, 4);

		uint32_t paramsSize = 0;
		if (method->ParameterCount > 0)
		{
			paramsSize += method->Parameters[method->ParameterCount - 1]->Offset  + StackSizeOfType(method->Parameters[method->ParameterCount - 1]->Type);
			paramsSize -= 8; // For stack frame and return pointer
			//printf("NewObj: LastParam Offset %u + %u Size\n", (unsigned int)method->Parameters[method->ParameterCount - 1]->Offset, (unsigned int)method->Parameters[method->ParameterCount - 1]->Size);
		}
		x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, paramsSize + global_SizeOfPointerInBytes);
		x86_push_reg(compMethod, X86_EAX);
	}
	else // Strings return their new obj from constructor
	{
		//printf("String Constructor: %u parameters\n", (unsigned int)method->ParameterCount);
		compMethod = JIT_Emit_ParamSwap(compMethod, method, TRUE);
		x86_push_imm(compMethod, 0);
		x86_push_imm(compMethod, method->ParentAssembly->ParentDomain);

		x86_call_code(compMethod, method->AssembledMethod);
		uint32_t paramsSize = global_SizeOfPointerInBytes;
		if (method->ParameterCount > 0)
		{
			JIT_Layout_Parameters(method);
			paramsSize += method->Parameters[method->ParameterCount - 1]->Offset + StackSizeOfType(method->Parameters[method->ParameterCount - 1]->Type);
			paramsSize -= 8; // For stack frame and return pointer
			//printf("NewObjString: LastParam Offset %u + %u Size\n", (unsigned int)method->Parameters[method->ParameterCount - 1]->Offset, (unsigned int)method->Parameters[method->ParameterCount - 1]->Size);
		}
		x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, paramsSize);
		x86_push_reg(compMethod, X86_EAX);
	}

	return compMethod;
}

char* JIT_Compile_Call_Absolute				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	IRMethod* m = (IRMethod*)instr->Arg1;
	if (!m->AssembledMethod)
	{
		JIT_CompileMethod(m);
	}

	compMethod = JIT_Emit_ParamSwap(compMethod, m, FALSE);
	
	x86_push_imm(compMethod, (unsigned int)m->ParentAssembly->ParentDomain);
	x86_call_code(compMethod, m->AssembledMethod);
	uint32_t paramsSize = global_SizeOfPointerInBytes;
	if (m->ParameterCount > 0)
	{
		JIT_Layout_Parameters(m);
		paramsSize += m->Parameters[m->ParameterCount - 1]->Offset + StackSizeOfType(m->Parameters[m->ParameterCount - 1]->Type);
		paramsSize -= 8; // For stack frame and return pointer
		//printf("CallAbsolute: LastParam index %i Offset %u + %u Size, Type = %s\n", (int)m->ParameterCount - 1, (unsigned int)m->Parameters[m->ParameterCount - 1]->Offset, (unsigned int)StackSizeOfType(m->Parameters[m->ParameterCount - 1]->Type), m->Parameters[m->ParameterCount - 1]->Type->TypeDef->Name);
	}
	
	x86_push_reg(compMethod, X86_EAX);
	x86_push_reg(compMethod, X86_EDX);
	for (uint32_t index = 0; index < m->ParameterCount; ++index)
	{
		if (m->Parameters[index]->Type->IsReferenceType)
		{
			// Remove 1 RootObject reference for removing the parameter reference as we leave scope
			x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, (int)m->Parameters[index]->Offset + 4, global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EBX, X86_EBX);
			unsigned char* skipRemove = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EBX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)m->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipRemove, (unsigned char*)compMethod);
		}
	}
	x86_pop_reg(compMethod, X86_EDX);
	x86_pop_reg(compMethod, X86_EAX);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, paramsSize);


	if (m->Returns)
	{
		if (StackSizeOfType(m->ReturnType) == 8)
		{
			x86_push_reg(compMethod, X86_EDX);
		}
		x86_push_reg(compMethod, X86_EAX);
	}
	return compMethod;
}

char* JIT_Compile_Call_Internal				(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	void* mthd = instr->Arg1; // This is the method we will be calling.
	IRMethod* m = (IRMethod*)instr->Arg2;
	
	compMethod = JIT_Emit_ParamSwap(compMethod, m, FALSE);

	x86_push_imm(compMethod, (unsigned int)m->ParentAssembly->ParentDomain); // Push the domain
	x86_call_code(compMethod, mthd); // Call the method.
	uint32_t paramsSize = global_SizeOfPointerInBytes;
	if (m->ParameterCount > 0)
	{
		JIT_Layout_Parameters(m);
		paramsSize += m->Parameters[m->ParameterCount - 1]->Offset + StackSizeOfType(m->Parameters[m->ParameterCount - 1]->Type);
		paramsSize -= 8; // For stack frame and return pointer
		//printf("CallInternal: LastParam Offset %u + %u Size\n", (unsigned int)m->Parameters[m->ParameterCount - 1]->Offset, (unsigned int)StackSizeOfType(m->Parameters[m->ParameterCount - 1]->Type));
	}

	x86_push_reg(compMethod, X86_EAX);
	x86_push_reg(compMethod, X86_EDX);
	for (uint32_t index = 0; index < m->ParameterCount; ++index)
	{
		if (m->Parameters[index]->Type->IsReferenceType)
		{
			// Remove 1 RootObject reference for removing the parameter reference as we leave scope
			x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, (int)m->Parameters[index]->Offset + 4, global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EBX, X86_EBX);
			unsigned char* skipRemove = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EBX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)m->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipRemove, (unsigned char*)compMethod);
		}
	}
	x86_pop_reg(compMethod, X86_EDX);
	x86_pop_reg(compMethod, X86_EAX);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, paramsSize);


	if (m->Returns)
	{
		if (StackSizeOfType(m->ReturnType) == 8)
		{
			x86_push_reg(compMethod, X86_EDX);
		}
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

	
	compMethod = JIT_Emit_ParamSwap(compMethod, m, FALSE);
	
	x86_push_imm(compMethod, instr->Arg1);
	x86_call_code(compMethod, JIT_Trampoline_DoCall);
	//x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, 4);
	//x86_push_imm(compMethod, (unsigned int)m->ParentAssembly->ParentDomain);
	x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)m->ParentAssembly->ParentDomain, 4);
	x86_call_reg(compMethod, X86_EAX);
	uint32_t paramsSize = global_SizeOfPointerInBytes;
	if (m->ParameterCount > 0)
	{
		JIT_Layout_Parameters(m);
		paramsSize += m->Parameters[m->ParameterCount - 1]->Offset + StackSizeOfType(m->Parameters[m->ParameterCount - 1]->Type);
		paramsSize -= 8; // For stack frame and return pointer
		//printf("Call: LastParam Offset %u + %u Size\n", (unsigned int)m->Parameters[m->ParameterCount - 1]->Offset, (unsigned int)m->Parameters[m->ParameterCount - 1]->Size);
	}

	x86_push_reg(compMethod, X86_EAX);
	x86_push_reg(compMethod, X86_EDX);
	for (uint32_t index = 0; index < m->ParameterCount; ++index)
	{
		if (m->Parameters[index]->Type->IsReferenceType)
		{
			// Remove 1 RootObject reference for removing the parameter reference as we leave scope
			x86_mov_reg_membase(compMethod, X86_EBX, X86_ESP, (int)m->Parameters[index]->Offset + 4, global_SizeOfPointerInBytes);
			x86_test_reg_reg(compMethod, X86_EBX, X86_EBX);
			unsigned char* skipRemove = (unsigned char*)compMethod;
			x86_branch32(compMethod, X86_CC_Z, 0, FALSE);

			x86_adjust_stack(compMethod, -(global_SizeOfPointerInBytes << 1));
			x86_mov_membase_reg(compMethod, X86_ESP, global_SizeOfPointerInBytes, X86_EBX, global_SizeOfPointerInBytes);
			x86_mov_membase_imm(compMethod, X86_ESP, 0, (unsigned int)m->ParentAssembly->ParentDomain->RootObject, global_SizeOfPointerInBytes);
			x86_call_code(compMethod, ReferenceTypeObject_RemoveReference);
			x86_adjust_stack(compMethod, (global_SizeOfPointerInBytes << 1));

			x86_patch(skipRemove, (unsigned char*)compMethod);
		}
	}
	x86_pop_reg(compMethod, X86_EDX);
	x86_pop_reg(compMethod, X86_EAX);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, paramsSize);

	if (m->Returns)
	{
		if (StackSizeOfType(m->ReturnType) == 8)
		{
			x86_push_reg(compMethod, X86_EDX);
		}
		x86_push_reg(compMethod, X86_EAX);
	}

	return compMethod;
}

void* JIT_Trampoline_DoCall(IRMethodSpec* mth, ReferenceTypeObject* obj)
{
	//printf("Arg1 @ 0x%x *@ 0x%x\n", (unsigned int)mth, (unsigned int)&mth);
	//printf("Arg2 @ 0x%x *@ 0x%x\n", (unsigned int)obj, (unsigned int)&obj);
	//printf("Arg3 @ 0x%x *@ 0x%x\n", (unsigned int)arg3, (unsigned int)&arg3);
	//printf("Arg4 @ 0x%x *@ 0x%x\n", (unsigned int)arg4, (unsigned int)&arg4);
	//printf("Arg5 @ 0x%x *@ 0x%x\n", (unsigned int)arg5, (unsigned int)&arg5);
	//printf("Arg6 @ 0x%x *@ 0x%x\n", (unsigned int)arg6, (unsigned int)&arg6);
	//printf("Arg7 @ 0x%x *@ 0x%x\n", (unsigned int)arg7, (unsigned int)&arg7);

	register void* variable = obj->Type;
	if (mth->MethodIndex >= ((IRType*)variable)->MethodCount)
	{
		printf("Object @ 0x%x\n", (unsigned int)obj);
		printf("Method Spec @ 0x%x\n", (unsigned int)mth);
		IRType* tp = (IRType*)variable;
		printf("Type %s has %i methods\n", tp->TypeDef->Name, (int)tp->MethodCount);
		for (uint32_t i = 0; i < tp->MethodCount; i++)
		{
			printf("Method at index %i is %s.%s\n", (int)i, tp->Methods[i]->MethodDefinition->TypeDefinition->Name, tp->Methods[i]->MethodDefinition->Name);
		}
		printf("Mth at 0x%x MethodIndex: %i\n", (unsigned int)mth, (int)mth->MethodIndex);
		printf("IRType at 0x%x MethodCount: %i\n", (unsigned int)variable, (int)((IRType*)variable)->MethodCount);
		Panic(String_Format("Method Index (%i) is too High!", (int)mth->MethodIndex));
	}

	if (mth->ParentType->IsInterface)
	{
		IRInterfaceImpl* impl = NULL;
		for (impl = ((IRType*)variable)->InterfaceTable; impl; impl = (IRInterfaceImpl*)impl->HashHandle.next)
		{
			printf("The type implements interface %s @ 0x%x\n", impl->InterfaceType->TypeDef->Name, (unsigned int)impl->InterfaceType);
		}
		HASH_FIND(HashHandle, ((IRType*)variable)->InterfaceTable, &mth->ParentType, sizeof(void*), impl);
		if (impl)
		{
			if (mth->MethodIndex < impl->MethodCount)
			{
				printf("Top object is %s, new method is at index %i, type has %i methods\n", ((IRType*)variable)->TypeDef->Name, (int)impl->MethodIndexes[mth->MethodIndex], (int)((IRType*)variable)->MethodCount);
				if (impl->MethodIndexes[mth->MethodIndex] < ((IRType*)variable)->MethodCount)
				{
					variable = ((IRType*)variable)->Methods[impl->MethodIndexes[mth->MethodIndex]];
				}
				else
				{
					Panic("Interface mapped method doesn't exist!");
				}
			}
			else
			{
				Panic("Requested method doesn't exist in the interface!");
			}
		}
		else
		{
			Panic("Requested type doesn't implement the requested interface!");
		}
	}
	else
	{
		//IRMethod* m = tp->Methods[mth->MethodIndex];
		variable = ((IRType*)variable)->Methods[mth->MethodIndex];
	}
	if (!((IRMethod*)variable)->AssembledMethod)
	{
		JIT_CompileMethod(((IRMethod*)variable));
	}
	return ((IRMethod*)variable)->AssembledMethod;
}

char* JIT_Compile_Jump						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	Panic("JIT_Compile_Jump not yet implemented!");
	return compMethod;
}

char* JIT_Compile_Throw						(IRInstruction* instr, char* compMethod, IRMethod* mth, BranchRegistry* branchRegistry)
{
	x86_adjust_stack(compMethod, global_SizeOfPointerInBytes);
	//Panic("JIT_Compile_Throw not yet implemented!");
	return compMethod;
}
