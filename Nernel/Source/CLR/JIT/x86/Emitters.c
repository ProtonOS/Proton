#include <CLR/JIT.h>
#include <CLR/JIT/x86/Layout.h>
#include <CLR/JIT/x86/x86-codegen.h>

char* JIT_Emit_Load(char* pCompiledCode, IRMethod* pMethod, SourceTypeData* pSource, X86_Reg_No pRegister1, X86_Reg_No pRegister2, X86_Reg_No pRegister3, size_t* pSize)
{
	uint32_t sizeOfSource = 0;
	switch (pSource->Type)
	{
		case SourceType_Local:
		{
			sizeOfSource = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->VariableType));
			switch (sizeOfSource)
			{
				case 4:
					x86_mov_reg_membase(pCompiledCode, pRegister1, X86_EBP, -pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset, 4);
					break;
				case 8:
					x86_mov_reg_membase(pCompiledCode, pRegister1, X86_EBP, -pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset, 4);
					x86_mov_reg_membase(pCompiledCode, pRegister2, X86_EBP, -(pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset - 4), 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, -((int32_t)sizeOfSource));
					uint32_t count = sizeOfSource >> gPointerDivideShift;
					uint32_t offset = 0;
					for (uint32_t index = 0; index < count; index++, offset += gSizeOfPointerInBytes)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_EBP, -(pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset - (index << gPointerDivideShift)), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, offset, pRegister2, gSizeOfPointerInBytes);
					}
					x86_mov_reg_reg(pCompiledCode, pRegister1, X86_ESP, gSizeOfPointerInBytes);
					break;
				}
			}
			break;
		}
		case SourceType_LocalAddress:
		{
			// TODO: Fix this, need to be loading value at address, not loading the address
			sizeOfSource = gSizeOfPointerInBytes;
			x86_mov_reg_reg(pCompiledCode, pRegister1, X86_EBP, gSizeOfPointerInBytes);
			x86_alu_reg_imm(pCompiledCode, X86_SUB, pRegister1, pMethod->LocalVariables[pSource->Data.LocalVariableAddress.LocalVariableIndex]->Offset);
			break;
		}
		case SourceType_Parameter:
		{
			sizeOfSource = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Type));
			switch (sizeOfSource)
			{
				case 4:
					x86_mov_reg_membase(pCompiledCode, pRegister1, X86_EBP, pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset, 4);
					break;
				case 8:
					x86_mov_reg_membase(pCompiledCode, pRegister1, X86_EBP, pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset, 4);
					x86_mov_reg_membase(pCompiledCode, pRegister2, X86_EBP, (pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset + 4), 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, -((int32_t)sizeOfSource));
					uint32_t count = sizeOfSource >> gPointerDivideShift;
					uint32_t offset = 0;
					for (uint32_t index = 0; index < count; index++, offset += gSizeOfPointerInBytes)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_EBP, (pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset + (index << gPointerDivideShift)), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, offset, pRegister2, gSizeOfPointerInBytes);
					}
					x86_mov_reg_reg(pCompiledCode, pRegister1, X86_ESP, gSizeOfPointerInBytes);
					break;
				}
			}
			break;
		}
		case SourceType_ParameterAddress:
		{
			// TODO: Fix this, need to be loading value at address, not loading the address
			sizeOfSource = gSizeOfPointerInBytes;
			x86_mov_reg_reg(pCompiledCode, pRegister1, X86_EBP, gSizeOfPointerInBytes);
			x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, pMethod->Parameters[pSource->Data.ParameterAddress.ParameterIndex]->Offset);
			break;
		}
		case SourceType_ConstantI4:
		{
			sizeOfSource = 4;
			x86_mov_reg_imm(pCompiledCode, pRegister1, pSource->Data.ConstantI4.Value);
			break;
		}
		case SourceType_ConstantI8:
		{
			sizeOfSource = 8;
			x86_mov_reg_imm(pCompiledCode, pRegister1, pSource->Data.ConstantI8.Value);
			x86_mov_reg_imm(pCompiledCode, pRegister2, pSource->Data.ConstantI8.Value >> 32);
			break;
		}
		case SourceType_ConstantR4:
		{
			sizeOfSource = 4;
			x86_mov_reg_imm(pCompiledCode, pRegister1, pSource->Data.ConstantR4.Value);
			break;
		}
		case SourceType_ConstantR8:
		{
			sizeOfSource = 8;
			x86_mov_reg_imm(pCompiledCode, pRegister1, pSource->Data.ConstantR8.Value);
			x86_mov_reg_imm(pCompiledCode, pRegister2, pSource->Data.ConstantR8.Value >> 32);
			break;
		}
		case SourceType_Field:
		{
			// Note: pRegister3 must contain a pointer to the object the field is being loaded from
			JIT_CalculateFieldLayout(pSource->Data.Field.ParentType);
			IRField* field = pSource->Data.Field.ParentType->Fields[pSource->Data.Field.FieldIndex];
			sizeOfSource = JIT_GetStackSizeOfType(field->FieldType);
			switch (sizeOfSource)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister3, field->Offset, sizeOfSource);
					break;
				case 5:
				case 6:
				case 7:
				case 8:
					x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, (field->Offset + 4), sizeOfSource - 4);
					x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister3, field->Offset, 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, -((int32_t)JIT_StackAlign(sizeOfSource)));
					uint32_t count = sizeOfSource >> gPointerDivideShift;
					uint32_t offset = 0;
					for (uint32_t index = 0; index < count; index++, offset += gSizeOfPointerInBytes)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, (field->Offset + (index << gPointerDivideShift)), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, offset, pRegister2, gSizeOfPointerInBytes);
					}
					uint32_t remainder = sizeOfSource % gSizeOfPointerInBytes;
					if (remainder)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, (field->Offset + (count << gPointerDivideShift)), remainder);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, offset, pRegister2, remainder);
					}
					x86_mov_reg_reg(pCompiledCode, pRegister1, X86_ESP, gSizeOfPointerInBytes);
					break;
				}
			}
			break;
		}
		case SourceType_FieldAddress:
		{
			// Note: pRegister3 must contain a pointer to the object the field is being loaded from
			JIT_CalculateFieldLayout(pSource->Data.FieldAddress.ParentType);
			sizeOfSource = gSizeOfPointerInBytes;
			x86_mov_reg_reg(pCompiledCode, pRegister1, pRegister3, gSizeOfPointerInBytes);
			x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, pSource->Data.FieldAddress.ParentType->Fields[pSource->Data.FieldAddress.FieldIndex]->Offset);
			break;
		}
		case SourceType_StaticField:
		{
			IRField* field = pSource->Data.StaticField.Field;
			sizeOfSource = JIT_GetStackSizeOfType(field->FieldType);
			x86_mov_reg_imm(pCompiledCode, pRegister3, (size_t)((uint8_t*)field->ParentAssembly->ParentDomain->StaticValues[field->ParentAssembly->AssemblyIndex] + field->Offset));
			switch (sizeOfSource)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister3, 0, sizeOfSource);
					break;
				case 5:
				case 6:
				case 7:
				case 8:
					x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, 4, sizeOfSource - 4);
					x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister3, 0, 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, -((int32_t)JIT_StackAlign(sizeOfSource)));
					uint32_t count = sizeOfSource >> gPointerDivideShift;
					uint32_t offset = 0;
					for (uint32_t index = 0; index < count; index++, offset += gSizeOfPointerInBytes)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, (index << gPointerDivideShift), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, offset, pRegister2, gSizeOfPointerInBytes);
					}
					uint32_t remainder = sizeOfSource % gSizeOfPointerInBytes;
					if (remainder)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, (field->Offset + (count << gPointerDivideShift)), remainder);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, offset, pRegister2, remainder);
					}
					x86_mov_reg_reg(pCompiledCode, pRegister1, X86_ESP, gSizeOfPointerInBytes);
					break;
				}
			}
			break;
		}
		case SourceType_StaticFieldAddress:
		{
			IRField* field = pSource->Data.StaticFieldAddress.Field;
			sizeOfSource = gSizeOfPointerInBytes;
			x86_mov_reg_imm(pCompiledCode, pRegister1, (size_t)((uint8_t*)field->ParentAssembly->ParentDomain->StaticValues[field->ParentAssembly->AssemblyIndex] + field->Offset));
			break;
		}
		case SourceType_SizeOf:
		{
			sizeOfSource = 4;
			x86_mov_reg_imm(pCompiledCode, pRegister1, JIT_GetSizeOfType(pSource->Data.SizeOf.Type));
			break;
		}
		default:
			Panic("Unknown source type for JIT_Emit_Load");
			break;
	}
	*pSize = sizeOfSource;
	return pCompiledCode;
}

char* JIT_Emit_Store(char* pCompiledCode, IRMethod* pMethod, SourceTypeData* pDestination, X86_Reg_No pRegister1, X86_Reg_No pRegister2, X86_Reg_No pRegister3, size_t* pSize)
{
	uint32_t sizeOfDestination = 0;
	switch (pDestination->Type)
	{
		case SourceType_Local:
		{
			sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->VariableType));
			switch (sizeOfDestination)
			{
				case 4:
					x86_mov_membase_reg(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister1, 4);
					break;
				case 8:
					x86_mov_membase_reg(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister1, 4);
					x86_mov_membase_reg(pCompiledCode, X86_EBP, -(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset - 4), pRegister2, 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, -((int32_t)sizeOfDestination));
					uint32_t count = sizeOfDestination >> gPointerDivideShift;
					uint32_t offset = 0;
					for (uint32_t index = 0; index < count; index++, offset += gSizeOfPointerInBytes)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, offset, gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_EBP, -(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset - (index << gPointerDivideShift)), pRegister2, gSizeOfPointerInBytes);
					}
					break;
				}
			}
			break;
		}
		case SourceType_LocalAddress:
		{
			Panic("This should not be happening!");
			break;
		}
		case SourceType_Parameter:
		{
			sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Type));
			switch (sizeOfDestination)
			{
				case 4:
					x86_mov_membase_reg(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister1, 4);
					break;
				case 8:
					x86_mov_membase_reg(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister1, 4);
					x86_mov_membase_reg(pCompiledCode, X86_EBP, (pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset + 4), pRegister2, 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, -((int32_t)sizeOfDestination));
					uint32_t count = sizeOfDestination >> gPointerDivideShift;
					uint32_t offset = 0;
					for (uint32_t index = 0; index < count; index++, offset += gSizeOfPointerInBytes)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, offset, gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_EBP, (pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset + (index << gPointerDivideShift)), pRegister2, gSizeOfPointerInBytes);
					}
					break;
				}
			}
			break;
		}
		case SourceType_ParameterAddress:
		{
			Panic("This should not be happening!");
			break;
		}
		case SourceType_ConstantI4:
		{
			Panic("This should not be happening!");
			break;
		}
		case SourceType_ConstantI8:
		{
			Panic("This should not be happening!");
			break;
		}
		case SourceType_ConstantR4:
		{
			Panic("This should not be happening!");
			break;
		}
		case SourceType_ConstantR8:
		{
			Panic("This should not be happening!");
			break;
		}
		case SourceType_Field:
		{
			// Note: pRegister3 must contain a pointer to the object the field is being stored to
			JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
			IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
			sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
			switch (sizeOfDestination)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, sizeOfDestination);
					break;
				case 5:
				case 6:
				case 7:
				case 8:
					x86_mov_membase_reg(pCompiledCode, pRegister3, (field->Offset + 4), pRegister2, sizeOfDestination - 4);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, -((int32_t)JIT_StackAlign(sizeOfDestination)));
					uint32_t count = sizeOfDestination >> gPointerDivideShift;
					uint32_t offset = 0;
					for (uint32_t index = 0; index < count; index++, offset += gSizeOfPointerInBytes)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, offset, gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, pRegister3, (field->Offset + (index << gPointerDivideShift)), pRegister2, gSizeOfPointerInBytes);
					}
					uint32_t remainder = sizeOfDestination % gSizeOfPointerInBytes;
					if (remainder)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, offset, remainder);
						x86_mov_membase_reg(pCompiledCode, pRegister3, (field->Offset + (count << gPointerDivideShift)), pRegister2, remainder);
					}
					break;
				}
			}
			break;
		}
		case SourceType_FieldAddress:
		{
			Panic("This should not be happening!");
			break;
		}
		case SourceType_StaticField:
		{
			IRField* field = pDestination->Data.StaticField.Field;
			sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
			x86_mov_reg_imm(pCompiledCode, pRegister3, (size_t)((uint8_t*)field->ParentAssembly->ParentDomain->StaticValues[field->ParentAssembly->AssemblyIndex] + field->Offset));
			switch (sizeOfDestination)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, sizeOfDestination);
					break;
				case 5:
				case 6:
				case 7:
				case 8:
					x86_mov_membase_reg(pCompiledCode, pRegister3, 4, pRegister2, sizeOfDestination - 4);
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, -((int32_t)JIT_StackAlign(sizeOfDestination)));
					uint32_t count = sizeOfDestination >> gPointerDivideShift;
					uint32_t offset = 0;
					for (uint32_t index = 0; index < count; index++, offset += gSizeOfPointerInBytes)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, offset, gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, pRegister3, (index << gPointerDivideShift), pRegister2, gSizeOfPointerInBytes);
					}
					uint32_t remainder = sizeOfDestination % gSizeOfPointerInBytes;
					if (remainder)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, offset, remainder);
						x86_mov_membase_reg(pCompiledCode, pRegister3, (field->Offset + (count << gPointerDivideShift)), pRegister2, remainder);
					}
					break;
				}
			}
			break;
		}
		case SourceType_StaticFieldAddress:
		{
			Panic("This should not be happening!");
			break;
		}
		case SourceType_SizeOf:
		{
			Panic("This should not be happening!");
			break;
		}
		default:
			Panic("Unknown source type for JIT_Emit_LoadSource");
			break;
	}
	*pSize = sizeOfDestination;
	return pCompiledCode;
}

void JIT_BranchLinker(BranchRegistry* pBranchRegistry)
{
	for (uint32_t index = 0; index < pBranchRegistry->InstructionCount; ++index)
	{
		if (pBranchRegistry->BranchLocations[index])
		{
			Log_WriteLine(LOGLEVEL__JIT_Link, "Linking %sBranch from 0x%x to 0x%x", pBranchRegistry->SpecialBranches[index] ? "Special " : "", (unsigned int)pBranchRegistry->BranchLocations[index], (unsigned int)pBranchRegistry->InstructionLocations[pBranchRegistry->TargetLocations[index]]);
			if (pBranchRegistry->SpecialBranches[index])
			{
				*(size_t*)pBranchRegistry->BranchLocations[index] = pBranchRegistry->InstructionLocations[pBranchRegistry->TargetLocations[index]];
			}
			else
			{
				x86_patch((unsigned char*)pBranchRegistry->BranchLocations[index], (unsigned char*)pBranchRegistry->InstructionLocations[pBranchRegistry->TargetLocations[index]]);
			}
		}
	}
}

char* JIT_Emit_Prologue(char* pCompiledCode, IRMethod* pMethod)
{
	// Save old stack frame
	x86_push_reg(pCompiledCode, X86_EBP);
	// Load new stack frame
	x86_mov_reg_reg(pCompiledCode, X86_EBP, X86_ESP, gSizeOfPointerInBytes);

	uint32_t localsSize = gSizeOfPointerInBytes;
	if (pMethod->LocalVariableCount)
	{
		localsSize = pMethod->LocalVariables[pMethod->LocalVariableCount - 1]->Offset;
	}

	// Create stack space for called IRMethod* and additional locals
	x86_adjust_stack(pCompiledCode, -((int32_t)localsSize));

	if (localsSize > gSizeOfPointerInBytes &&
		pMethod->MethodDefinition->Body.Flags & MethodDefinitionBody_Fat_Flags_InitializeLocals)
	{
		for (uint32_t offset = 0; offset < localsSize; offset += 4)
		{
			// Initialize memory to zero for all of the local space
			x86_mov_membase_imm(pCompiledCode, X86_ESP, offset, 0, 4);
		}
	}

	// Assign the called IRMethod* into the first local variable
	x86_mov_membase_imm(pCompiledCode, X86_EBP, -gSizeOfPointerInBytes, (size_t)pMethod, gSizeOfPointerInBytes);

	return pCompiledCode;
}

char* JIT_Emit_Epilogue(char* pCompiledCode, IRMethod* pMethod)
{
	if (pMethod->Returns)
	{
		// Pop the value into EAX, as per cdecl call convention
		x86_pop_reg(pCompiledCode, X86_EAX);
		uint32_t sizeOfReturnType = JIT_GetStackSizeOfType(pMethod->ReturnType);
		if (sizeOfReturnType > 4 && sizeOfReturnType <= 8)
		{
			// If the size of the return type is 5-8 bytes, pop the rest into EDX as per cdecl call convention
			x86_pop_reg(pCompiledCode, X86_EDX);
		}
		else
		{
			// TODO: Deal with return parameters larger than 8 bytes
			Panic("Don't know how to support large return types yet");
		}
	}
	// Adjust the stack for local variables, and restore the previous stack frame
	x86_leave(pCompiledCode);
	// Pop EIP and return back to caller method
	x86_ret(pCompiledCode);
	return pCompiledCode;
}

char* JIT_Emit_Nop(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	if ((uint32_t)pInstruction->Arg1)
	{
		// Emit a nop if it came from IL (probably for debugging)
		x86_nop(pCompiledCode);
	}
	return pCompiledCode;
}

char* JIT_Emit_Break(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Return(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	// Register a branch target for return to immediately jump into epilogue
	BranchRegistry_RegisterBranch(pBranchRegistry, pInstruction->ILLocation, pBranchRegistry->InstructionCount + 1, pCompiledCode);
	x86_jump32(pCompiledCode, 0);
	return pCompiledCode;
}

char* JIT_Emit_Load_Parameter(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_ParameterAddress(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Store_Parameter(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_Local(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_LocalAddress(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Store_Local(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_Null(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_String(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_Int32(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_Int64(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_Float32(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_Float64(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Dup(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Pop(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_Indirect(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Store_Indirect(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Add(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Sub(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Mul(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Div(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Rem(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_And(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Or(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Xor(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Neg(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Not(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Shift(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Convert_Unchecked(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Convert_Checked(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_CastClass(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_IsInst(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Unbox(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_UnboxAny(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Box(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Throw(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_Object(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Store_Object(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Copy_Object(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_New_Object(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_ArrayLength(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_New_Array(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_CheckFinite(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_Field(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_FieldAddress(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Store_Field(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_StaticField(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_StaticFieldAddress(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Store_StaticField(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_Element(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_ElementAddress(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Store_Element(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Allocate_Local(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Initialize_Object(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Copy_Block(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Initialize_Block(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_SizeOf(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Jump(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Call_Virtual(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Call_Constrained(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Call_Absolute(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Call_Internal(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Branch(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Switch(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_Function(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_VirtualFunction(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Compare(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_Load_Token(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_MkRefAny(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_RefAnyVal(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}

char* JIT_Emit_RefAnyType(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	return pCompiledCode;
}
