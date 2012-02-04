#include <CLR/JIT/x86/x86-codegen.h>
#include <CLR/JIT/JIT.h>
#include <CLR/JIT/JIT-x86.h>
#include <CLR/Architecture.h>
#include <CLR/SyntheticStack.h>
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
	// We don't have to move the
	// stack back from local variables,
	// because we're restoring all registers
	// here from when the stack hadn't been 
	// touched yet.
	x86_mov_reg_reg(compMethod, X86_ESP, X86_EBP, global_SizeOfPointerInBytes);


	// // We don't care about the assembly
	// // or method indexes anymore.
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
	//x86_ret(compMethod);
	return compMethod;
}


char* JIT_Compile_LoadInt32_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	x86_push_imm(compMethod, *((int32_t*)instr->Arg1));
	return compMethod;
}


char* JIT_Compile_LoadInt64_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	int64_t value = *((int64_t*)instr->Arg1);
	x86_push_imm(compMethod, (value & 0xFFFFFFFF));
	x86_push_imm(compMethod, (value >> 32));
	return compMethod;
}


char* JIT_Compile_LoadFloat32_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	x86_push_imm(compMethod, *((int32_t*)instr->Arg1));
	return compMethod;
}


char* JIT_Compile_LoadFloat64_Val			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	int64_t value = *((int64_t*)instr->Arg1);
	x86_push_imm(compMethod, (value & 0xFFFFFFFF));
	x86_push_imm(compMethod, (value >> 32));
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
	uint32_t localIndex = *(uint32_t*)instr->Arg1;
	uint32_t size = global_SizeOfPointerInBytes;
	if (mth->LocalVariables[localIndex]->VariableType->IsValueType)
		size = mth->LocalVariables[localIndex]->VariableType->Size;
	uint32_t movCount = size / global_SizeOfPointerInBytes;
	if ((size % global_SizeOfPointerInBytes) != 0) ++movCount;
	for (uint32_t movIndex = 0; movIndex < movCount; ++movIndex)
	{
		x86_pop_reg(compMethod, X86_EAX);
		x86_mov_membase_reg(compMethod, X86_EBP, mth->LocalVariables[localIndex]->Offset + (movIndex * global_SizeOfPointerInBytes), X86_EAX, global_SizeOfPointerInBytes);
	}
	return compMethod;
}


char* JIT_Compile_Load_LocalVar				(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	uint32_t localIndex = *(uint32_t*)instr->Arg1;
	uint32_t size = global_SizeOfPointerInBytes;
	if (mth->LocalVariables[localIndex]->VariableType->IsValueType)
		size = mth->LocalVariables[localIndex]->VariableType->Size;
	uint32_t movCount = size / global_SizeOfPointerInBytes;
	if ((size % global_SizeOfPointerInBytes) != 0) ++movCount;
	for (uint32_t movIndex = 0; movIndex < movCount; ++movIndex)
	{
		x86_mov_reg_membase(compMethod, X86_EAX, X86_EBP, mth->LocalVariables[localIndex]->Offset + (movIndex * global_SizeOfPointerInBytes), global_SizeOfPointerInBytes);
		x86_push_reg(compMethod, X86_EAX);
	}
	return compMethod;
}


char* JIT_Compile_Load_LocalVar_Address		(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	uint32_t localIndex = *(uint32_t*)instr->Arg1;
	x86_mov_reg_reg(compMethod, X86_EAX, X86_EBP, global_SizeOfPointerInBytes);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_EAX, mth->LocalVariables[localIndex]->Offset);
	x86_push_reg(compMethod, X86_EAX);
	return compMethod;
}


char* JIT_Compile_Convert_OverflowCheck		(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	return compMethod;
}


char* JIT_Compile_Convert_Unchecked			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	char buf[128];
	ConversionArgumentType fromType = *(ConversionArgumentType*)instr->Arg1;
	ConversionArgumentType toType = *(ConversionArgumentType*)instr->Arg2;
	switch (fromType)
	{
		case ConversionArgumentType_I:
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
						snprintf(buf, 128, "Convert Unchecked, Invalid Arguments: From = 0x%x, To = 0x%x", (unsigned int)fromType, (unsigned int)toType);
						Panic(buf);
					}
					break;
			}
			break;
		case ConversionArgumentType_I8:
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
				case ConversionArgumentType_U8: break;
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
						snprintf(buf, 128, "Convert Unchecked, Invalid Arguments: From = 0x%x, To = 0x%x", (unsigned int)fromType, (unsigned int)toType);
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
						snprintf(buf, 128, "Convert Unchecked, Invalid Arguments: From = 0x%x, To = 0x%x", (unsigned int)fromType, (unsigned int)toType);
						Panic(buf);
					}
					break;
			}
			break;
		default:
			{
				snprintf(buf, 128, "Convert Unchecked, Invalid Arguments: From = 0x%x, To = 0x%x", (unsigned int)fromType, (unsigned int)toType);
				Panic(buf);
			}
			break;
	}
	return compMethod;
}


char* JIT_Compile_Load_Parameter			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	uint32_t paramIndex = *(uint32_t*)instr->Arg1;
	uint32_t size = global_SizeOfPointerInBytes;
	if (mth->Parameters[paramIndex]->Type->IsValueType)
		size = mth->Parameters[paramIndex]->Type->Size;
	uint32_t movCount = size / global_SizeOfPointerInBytes;
	if ((size % global_SizeOfPointerInBytes) != 0) ++movCount;
	for (uint32_t movIndex = 0; movIndex < movCount; ++movIndex)
	{
		x86_mov_reg_membase(compMethod, X86_EAX, X86_EBP, (mth->Parameters[paramIndex]->Offset + (movIndex * global_SizeOfPointerInBytes)) * -1, global_SizeOfPointerInBytes);
		x86_push_reg(compMethod, X86_EAX);
	}
	return compMethod;
}


char* JIT_Compile_Load_Parameter_Address	(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	uint32_t paramIndex = *(uint32_t*)instr->Arg1;
	x86_mov_reg_reg(compMethod, X86_EAX, X86_EBP, global_SizeOfPointerInBytes);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_EAX, mth->Parameters[paramIndex]->Offset * -1);
	x86_push_reg(compMethod, X86_EAX);
	return compMethod;
}


char* JIT_Compile_Store_Parameter			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	uint32_t paramIndex = *(uint32_t*)instr->Arg1;
	uint32_t size = global_SizeOfPointerInBytes;
	if (mth->Parameters[paramIndex]->Type->IsValueType)
		size = mth->Parameters[paramIndex]->Type->Size;
	uint32_t movCount = size / global_SizeOfPointerInBytes;
	if ((size % global_SizeOfPointerInBytes) != 0) ++movCount;
	for (uint32_t movIndex = 0; movIndex < movCount; ++movIndex)
	{
		x86_pop_reg(compMethod, X86_EAX);
		x86_mov_membase_reg(compMethod, X86_EBP, (mth->Parameters[paramIndex]->Offset + (movIndex * global_SizeOfPointerInBytes)) * -1, X86_EAX, global_SizeOfPointerInBytes);
	}
	return compMethod;
}


char* JIT_Compile_Load_Element				(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	ElementType elementType = *(ElementType*)instr->Arg1;
	//StackObjectType indexType = *(StackObjectType*)instr->Arg2;
	uint32_t sizeOfElementType = 0;
	GetSizeOfElementType(sizeOfElementType, elementType);

	x86_pop_reg(compMethod, X86_EAX);
	x86_pop_reg(compMethod, X86_ECX);
	x86_mov_reg_mem(compMethod, X86_ECX, X86_ECX, global_SizeOfPointerInBytes);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ECX, sizeof(x86ArrayHeader));
	x86_imul_reg_reg_imm(compMethod, X86_EAX, X86_EAX, sizeOfElementType);
	x86_alu_reg_reg(compMethod, X86_ADD, X86_ECX, X86_EAX);
	x86_mov_reg_mem(compMethod, X86_EAX, X86_ECX, sizeOfElementType);
	x86_push_reg(compMethod, X86_EAX);
	return compMethod;
}


char* JIT_Compile_Store_Element				(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	ElementType elementType = *(ElementType*)instr->Arg1;
	//StackObjectType indexType = *(StackObjectType*)instr->Arg2;
	uint32_t sizeOfElementType = 0;
	GetSizeOfElementType(sizeOfElementType, elementType);

	x86_pop_reg(compMethod, X86_EDX);
	x86_pop_reg(compMethod, X86_ECX);
	x86_pop_reg(compMethod, X86_EAX);
	x86_mov_reg_mem(compMethod, X86_ECX, X86_ECX, global_SizeOfPointerInBytes);
	x86_alu_reg_imm(compMethod, X86_ADD, X86_ECX, sizeof(x86ArrayHeader));
	x86_imul_reg_reg_imm(compMethod, X86_EAX, X86_EAX, sizeOfElementType);
	x86_alu_reg_reg(compMethod, X86_ADD, X86_ECX, X86_EAX);
	x86_mov_mem_reg(compMethod, X86_ECX, X86_EDX, sizeOfElementType);
	return compMethod;
}


char* JIT_Compile_Load_Array_Length			(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	x86_pop_reg(compMethod, X86_ECX);
	x86_mov_reg_mem(compMethod, X86_ECX, X86_ECX, global_SizeOfPointerInBytes);
	x86_mov_reg_mem(compMethod, X86_ECX, X86_ECX, 4);
	x86_push_reg(compMethod, X86_ECX);
	return compMethod;
}


char* JIT_Compile_Pop						(IRInstruction* instr, char* compMethod, IRMethod* mth)
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
				x86_alu_reg_imm(compMethod, X86_ADD, X86_ESP, (unsigned int)pCount);
			}
			break;
	}
	return compMethod;
}


char* JIT_Compile_Shift						(IRInstruction* instr, char* compMethod, IRMethod* mth)
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
		default: Panic("Invalid shift direction"); break;
	}
	return compMethod;
}


char* JIT_Compile_Add						(IRInstruction* instr, char* compMethod, IRMethod* mth)
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
					x86_alu_mem_reg(compMethod, X86_ADD, X86_ESP, X86_EAX);
					break;
				case ElementType_I8:
				case ElementType_U8:
					x86_pop_reg(compMethod, X86_EAX);
					x86_pop_reg(compMethod, X86_EBX);
					x86_alu_mem_reg(compMethod, X86_ADD, X86_ESP, X86_EAX);
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


char* JIT_Compile_Sub						(IRInstruction* instr, char* compMethod, IRMethod* mth)
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
					x86_alu_mem_reg(compMethod, X86_SUB, X86_ESP, X86_EAX);
					break;
				case ElementType_I8:
				case ElementType_U8:
					x86_pop_reg(compMethod, X86_EAX);
					x86_pop_reg(compMethod, X86_EBX);
					x86_alu_mem_reg(compMethod, X86_SUB, X86_ESP, X86_EAX);
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


char* JIT_Compile_Mul						(IRInstruction* instr, char* compMethod, IRMethod* mth)
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


char* JIT_Compile_Div						(IRInstruction* instr, char* compMethod, IRMethod* mth)
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
		case OverflowType_Unsigned:
		default: Panic("Unsupported OverflowType!"); break;
	}
	return compMethod;
}


char* JIT_Compile_Rem						(IRInstruction* instr, char* compMethod, IRMethod* mth)
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
					x86_mov_mem_reg(compMethod, X86_ESP, X86_EDX, 4);
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
		case OverflowType_Signed:
		case OverflowType_Unsigned:
		default: Panic("Unsupported OverflowType!"); break;
	}
	return compMethod;
}


char* JIT_Compile_LoadIndirect				(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	ElementType elementType = *(ElementType*)instr->Arg1;
	uint32_t sizeOfElementType = 0;
	GetSizeOfElementType(sizeOfElementType, elementType);
	x86_pop_reg(compMethod, X86_ECX);
	x86_mov_reg_mem(compMethod, X86_ECX, X86_ECX, sizeOfElementType);
	x86_push_reg(compMethod, X86_ECX);
	return compMethod;
}


char* JIT_Compile_StoreIndirect				(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	ElementType elementType = *(ElementType*)instr->Arg1;
	uint32_t sizeOfElementType = 0;
	GetSizeOfElementType(sizeOfElementType, elementType);
	x86_pop_reg(compMethod, X86_EAX);
	x86_pop_reg(compMethod, X86_ECX);
	x86_mov_mem_reg(compMethod, X86_ECX, X86_EAX, sizeOfElementType);
	return compMethod;
}


char* JIT_Compile_Call						(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}


char* JIT_Compile_LoadNull					(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	x86_push_imm(compMethod, (unsigned int)0);
	return compMethod;
}


char* JIT_Compile_NewObj					(IRInstruction* instr, char* compMethod, IRMethod* mth)
{
	
	return compMethod;
}



