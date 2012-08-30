#include <CLR/GC.h>
#include <CLR/JIT.h>
#include <CLR/JIT/Layout.h>
#include <CLR/JIT/x86/x86-codegen.h>
#include <CLR/Optimizations/IntrinsicSubstitution.h>

#define PRIMARY_REG X86_EAX
#define SECONDARY_REG X86_EBX
#define THIRD_REG X86_ECX
#define FOURTH_REG X86_EDX

#define DOMAIN_REG X86_EDI

//#define MemoryCorruptionChecks

uint64_t __udivdi3(uint64_t pValue, uint64_t pDivisor);
uint64_t __umoddi3(uint64_t pValue, uint64_t pDivisor);

#ifdef MemoryCorruptionChecks

#define Move_Destination_Default(parentFunc, type) \
default: \
	Panic(#parentFunc ": Unknown destination type for " #type "!"); \
	break;

#define Destination_Default(parentFunc) \
default: \
	Panic(#parentFunc ": Unknown destination type!"); \
	break;

#define Source_Default(parentFunc) \
default: \
	Panic(#parentFunc ": Unknown source type!"); \
	break; 
#else

#define Move_Destination_Default(parentFunc, type)
#define Destination_Default(parentFunc)
#define Source_Default(parentFunc)

#endif

// These source types aren't valid destinations
#define Define_Bad_Destinations() \
	case SourceType_Null: \
	case SourceType_ParameterAddress: \
	case SourceType_ConstantI4: \
	case SourceType_ConstantI8: \
	case SourceType_ConstantR4: \
	case SourceType_ConstantR8: \
	case SourceType_LocalAddress: \
	case SourceType_FieldAddress: \
	case SourceType_StaticFieldAddress: \
	case SourceType_SizeOf: \
	case SourceType_ArrayElementAddress: \
	case SourceType_ArrayLength: \
	{ \
		Panic("This should not be happening!"); \
		break; \
	}

#define Define_Move_To_Destination(sourceBaseReg, sourceOffset, destinationBaseReg, destinationOffset, intermediateReg, sourceAligned, destinationAligned) \
	switch (sizeOfDestination) \
	{ \
		case 1: \
		case 2: \
		case 3: \
			if (!sourceAligned && destinationAligned) \
				x86_alu_reg_reg(pCompiledCode, X86_XOR, intermediateReg, intermediateReg); \
		case 4: \
			if (sourceAligned) \
				x86_mov_reg_membase(pCompiledCode, intermediateReg, sourceBaseReg, sourceOffset, gSizeOfPointerInBytes); \
			else \
				x86_mov_reg_membase(pCompiledCode, intermediateReg, sourceBaseReg, sourceOffset, sizeOfSource); \
			\
			if (destinationAligned) \
				x86_mov_membase_reg(pCompiledCode, destinationBaseReg, destinationOffset, intermediateReg, gSizeOfPointerInBytes); \
			else \
				x86_mov_membase_reg(pCompiledCode, destinationBaseReg, destinationOffset, intermediateReg, sizeOfDestination); \
			break; \
		default: \
		{ \
			uint32_t count = sizeOfDestination >> gPointerDivideShift; \
			for (uint32_t index = 0; index < count; index++) \
			{ \
				x86_mov_reg_membase(pCompiledCode, intermediateReg, sourceBaseReg, (sourceOffset) + (index << gPointerDivideShift), gSizeOfPointerInBytes); \
				x86_mov_membase_reg(pCompiledCode, destinationBaseReg, (destinationOffset) + (index << gPointerDivideShift), intermediateReg, gSizeOfPointerInBytes); \
			} \
			if (!destinationAligned) \
			{ \
				uint32_t remainder = sizeOfDestination & (gSizeOfPointerInBytes - 1); \
				if (remainder) \
				{ \
					x86_mov_reg_membase(pCompiledCode, intermediateReg, sourceBaseReg, (sourceOffset) + (count << gPointerDivideShift), remainder); \
					x86_mov_membase_reg(pCompiledCode, destinationBaseReg, (destinationOffset) + (count << gPointerDivideShift), intermediateReg, remainder); \
				} \
			} \
			break; \
		} \
	}


#define EMIT_Static_Constructor_Call(reg1, reg2, field, code) \
	if (field->ParentType->HasStaticConstructor) \
	{ \
		x86_mov_reg_membase(code, reg1, DOMAIN_REG, offsetof(AppDomain, StaticConstructorsCalled), gSizeOfPointerInBytes); \
		x86_mov_reg_membase(code, reg1, reg1, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes); \
		x86_mov_reg_membase(code, reg2, reg1, (field->ParentType->TypeIndex << gPointerDivideShift), sizeof(bool_t)); \
		x86_test_reg_reg(code, reg2, reg2); \
		unsigned char* eBranch = (unsigned char*)code; \
		x86_branch32(code, X86_CC_NZ, 0, FALSE); \
		x86_mov_membase_imm(code, reg1, (field->ParentType->TypeIndex << gPointerDivideShift), TRUE, sizeof(bool_t)); \
		printf("Static constructor at 0x%x\n", (unsigned int)field->ParentType->StaticConstructor); \
		JIT_CompileMethod(field->ParentType->StaticConstructor); \
		x86_call_code(code, field->ParentType->StaticConstructor->AssembledMethod); \
		x86_patch(eBranch, (unsigned char*)code); \
	}




char* JIT_Emit_Load(char* pCompiledCode, IRMethod* pMethod, SourceTypeData* pSource, X86_Reg_No pRegister1, X86_Reg_No pRegister2, X86_Reg_No pRegister3, bool_t loadToStack, uint32_t stackOffset, size_t* pSize)
{
	uint32_t sizeOfSource = 0;
	switch (pSource->Type)
	{
		case SourceType_Null:
		{
			if (loadToStack)
				x86_mov_membase_imm(pCompiledCode, X86_ESP, stackOffset, 0, 4);
			else
				x86_mov_reg_imm(pCompiledCode, pRegister1, 0);
			sizeOfSource = gSizeOfPointerInBytes;
			break;
		}
		case SourceType_Local:
		{
			sizeOfSource = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->VariableType));
			switch (sizeOfSource)
			{
				case 4:
					x86_mov_reg_membase(pCompiledCode, pRegister1, X86_EBP, -pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset, 4);
					if (loadToStack)
						x86_mov_membase_reg(pCompiledCode, X86_ESP, stackOffset, pRegister1, 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, sizeOfSource);
					uint32_t count = sizeOfSource >> gPointerDivideShift;
					for (uint32_t index = 0; index < count; index++)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_EBP, -(pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset - (index << gPointerDivideShift)), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, (index << gPointerDivideShift) + stackOffset, pRegister2, gSizeOfPointerInBytes);
					}
					break;
				}
			}
			break;
		}
		case SourceType_LocalAddress:
		{
			sizeOfSource = gSizeOfPointerInBytes;
			if (loadToStack)
			{
				x86_mov_membase_reg(pCompiledCode, X86_ESP, stackOffset, X86_EBP, gSizeOfPointerInBytes);
				x86_alu_membase_imm(pCompiledCode, X86_SUB, X86_ESP, stackOffset, pMethod->LocalVariables[pSource->Data.LocalVariableAddress.LocalVariableIndex]->Offset);
			}
			else
			{
				x86_mov_reg_reg(pCompiledCode, pRegister1, X86_EBP, gSizeOfPointerInBytes);
				x86_alu_reg_imm(pCompiledCode, X86_SUB, pRegister1, pMethod->LocalVariables[pSource->Data.LocalVariableAddress.LocalVariableIndex]->Offset);
			}
			break;
		}
		case SourceType_Parameter:
		{
			sizeOfSource = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Type));
			switch (sizeOfSource)
			{
				case 4:
					x86_mov_reg_membase(pCompiledCode, pRegister1, X86_EBP, pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset, 4);
					if (loadToStack)
						x86_mov_membase_reg(pCompiledCode, X86_ESP, stackOffset, pRegister1, 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, sizeOfSource);
					uint32_t count = sizeOfSource >> gPointerDivideShift;
					for (uint32_t index = 0; index < count; index++)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_EBP, (pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset + (index << gPointerDivideShift)), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, (index << gPointerDivideShift) + stackOffset, pRegister2, gSizeOfPointerInBytes);
					}
					break;
				}
			}
			break;
		}
		case SourceType_ParameterAddress:
		{
			sizeOfSource = gSizeOfPointerInBytes;
			if (loadToStack)
			{
				x86_mov_membase_reg(pCompiledCode, X86_ESP, stackOffset, X86_EBP, gSizeOfPointerInBytes);
				x86_alu_membase_imm(pCompiledCode, X86_ADD, X86_ESP, stackOffset, pMethod->Parameters[pSource->Data.ParameterAddress.ParameterIndex]->Offset);
			}
			else
			{
				x86_mov_reg_reg(pCompiledCode, pRegister1, X86_EBP, gSizeOfPointerInBytes);
				x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, pMethod->Parameters[pSource->Data.ParameterAddress.ParameterIndex]->Offset);
			}
			break;
		}
		case SourceType_ConstantI4:
		{
			sizeOfSource = 4;
			if (loadToStack)
				x86_mov_membase_imm(pCompiledCode, X86_ESP, stackOffset, pSource->Data.ConstantI4.Value, 4);
			else
				x86_mov_reg_imm(pCompiledCode, pRegister1, pSource->Data.ConstantI4.Value);
			break;
		}
		case SourceType_ConstantI8:
		{
			sizeOfSource = 8;
			if (loadToStack)
			{
				x86_mov_membase_imm(pCompiledCode, X86_ESP, stackOffset, pSource->Data.ConstantI8.Value >> 32, 4);
				x86_mov_membase_imm(pCompiledCode, X86_ESP, stackOffset + 4, pSource->Data.ConstantI8.Value, 4);
			}
			else
			{
				x86_push_imm(pCompiledCode, pSource->Data.ConstantI8.Value >> 32);
				x86_push_imm(pCompiledCode, pSource->Data.ConstantI8.Value);
			}
			break;
		}
		case SourceType_ConstantR4:
		{
			sizeOfSource = 4;
			if (loadToStack)
				x86_mov_membase_imm(pCompiledCode, X86_ESP, stackOffset, pSource->Data.ConstantR4.Value, 4);
			else
				x86_mov_reg_imm(pCompiledCode, pRegister1, pSource->Data.ConstantR4.Value);
			break;
		}
		case SourceType_ConstantR8:
		{
			sizeOfSource = 8;
			if (loadToStack)
			{
				x86_mov_membase_imm(pCompiledCode, X86_ESP, stackOffset, pSource->Data.ConstantR8.Value >> 32, 4);
				x86_mov_membase_imm(pCompiledCode, X86_ESP, stackOffset + 4, pSource->Data.ConstantR8.Value, 4);
			}
			else
			{
				x86_push_imm(pCompiledCode, pSource->Data.ConstantR8.Value >> 32);
				x86_push_imm(pCompiledCode, pSource->Data.ConstantR8.Value);
			}
			break;
		}
		case SourceType_Field:
		{
			JIT_CalculateFieldLayout(pSource->Data.Field.ParentType);
			IRField* sourceField = pSource->Data.Field.ParentType->Fields[pSource->Data.Field.FieldIndex];
			sizeOfSource = JIT_GetStackSizeOfType(sourceField->FieldType);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
			switch (sizeOfSource)
			{
				case 1:
				case 2:
				case 3:
					x86_alu_reg_reg(pCompiledCode, X86_XOR, pRegister1, pRegister1);
				case 4:
					x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister3, sourceField->Offset, sizeOfSource);
					if (loadToStack)
						x86_mov_membase_reg(pCompiledCode, X86_ESP, stackOffset, pRegister1, 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, JIT_StackAlign(sizeOfSource));
					uint32_t count = sizeOfSource >> gPointerDivideShift;
					for (uint32_t index = 0; index < count; index++)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, (index << gPointerDivideShift) + sourceField->Offset, gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, (index << gPointerDivideShift) + stackOffset, pRegister2, gSizeOfPointerInBytes);
					}
					uint32_t remainder = sizeOfSource & (gSizeOfPointerInBytes - 1);
					if (remainder)
					{
						x86_alu_reg_reg(pCompiledCode, X86_XOR, pRegister2, pRegister2);
						x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, (count << gPointerDivideShift) + sourceField->Offset, remainder);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, (count << gPointerDivideShift) + stackOffset, pRegister2, remainder);
					}
					break;
				}
			}
			break;
		}
		case SourceType_FieldAddress:
		{
			JIT_CalculateFieldLayout(pSource->Data.FieldAddress.ParentType);
			sizeOfSource = gSizeOfPointerInBytes;
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.FieldAddress.FieldSource, pRegister1, pRegister2, pRegister3, FALSE, 0, NULL);
			x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, pSource->Data.FieldAddress.ParentType->Fields[pSource->Data.FieldAddress.FieldIndex]->Offset);
			if (loadToStack)
				x86_mov_membase_reg(pCompiledCode, X86_ESP, stackOffset, pRegister1, 4);
			break;
		}
		case SourceType_StaticField:
		{
			IRField* field = pSource->Data.StaticField.Field;
			sizeOfSource = JIT_GetStackSizeOfType(field->FieldType);

			EMIT_Static_Constructor_Call(pRegister3, pRegister1, field, pCompiledCode);

			x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
			x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
			switch (sizeOfSource)
			{
				case 1:
				case 2:
				case 3:
					x86_alu_reg_reg(pCompiledCode, X86_XOR, pRegister1, pRegister1);
				case 4:
					x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister3, field->Offset, sizeOfSource);
					if (loadToStack)
						x86_mov_membase_reg(pCompiledCode, X86_ESP, stackOffset, pRegister1, 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, JIT_StackAlign(sizeOfSource));
					uint32_t count = sizeOfSource >> gPointerDivideShift;
					for (uint32_t index = 0; index < count; index++)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, field->Offset + (index << gPointerDivideShift), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, (index << gPointerDivideShift) + stackOffset, pRegister2, gSizeOfPointerInBytes);
					}
					uint32_t remainder = sizeOfSource & (gSizeOfPointerInBytes - 1);
					if (remainder)
					{
						x86_alu_reg_reg(pCompiledCode, X86_XOR, pRegister2, pRegister2);
						x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, field->Offset + (count << gPointerDivideShift), remainder);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, (count << gPointerDivideShift) + stackOffset, pRegister2, remainder);
					}
					break;
				}
			}
			break;
		}
		case SourceType_StaticFieldAddress:
		{
			IRField* field = pSource->Data.StaticFieldAddress.Field;
			sizeOfSource = gSizeOfPointerInBytes;

			EMIT_Static_Constructor_Call(pRegister3, pRegister1, field, pCompiledCode);

			x86_mov_reg_membase(pCompiledCode, pRegister1, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
			x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister1, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
			x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, field->Offset);
			if (loadToStack)
				x86_mov_membase_reg(pCompiledCode, X86_ESP, stackOffset, pRegister1, 4);
			break;
		}
		case SourceType_Indirect:
		{
			sizeOfSource = JIT_GetStackSizeOfType(pSource->Data.Indirect.Type);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
			switch (sizeOfSource)
			{
				case 1:
				case 2:
				case 3:
					x86_alu_reg_reg(pCompiledCode, X86_XOR, pRegister1, pRegister1);
				case 4:
					x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister3, 0, sizeOfSource);
					if (loadToStack)
						x86_mov_membase_reg(pCompiledCode, X86_ESP, stackOffset, pRegister1, 4);
					break;
				default:
				{
					x86_adjust_stack(pCompiledCode, JIT_StackAlign(sizeOfSource));
					uint32_t count = sizeOfSource >> gPointerDivideShift;
					for (uint32_t index = 0; index < count; index++)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, (index << gPointerDivideShift), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, (index << gPointerDivideShift) + stackOffset, pRegister2, gSizeOfPointerInBytes);
					}
					uint32_t remainder = sizeOfSource & (gSizeOfPointerInBytes - 1);
					if (remainder)
					{
						x86_alu_reg_reg(pCompiledCode, X86_XOR, pRegister2, pRegister2);
						x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, (count << gPointerDivideShift), remainder);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, (count << gPointerDivideShift) + stackOffset, pRegister2, remainder);
					}
					break;
				}
			}
			break;
		}
		case SourceType_SizeOf:
		{
			sizeOfSource = 4;
			if (loadToStack)
				x86_mov_membase_imm(pCompiledCode, X86_ESP, stackOffset, JIT_GetSizeOfType(pSource->Data.SizeOf.Type), 4);
			else
				x86_mov_reg_imm(pCompiledCode, pRegister1, JIT_GetSizeOfType(pSource->Data.SizeOf.Type));
			break;
		}
		case SourceType_ArrayElement:
		{
			sizeOfSource = JIT_GetStackSizeOfType(pSource->Data.ArrayElement.ElementType);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
			switch (sizeOfSource)
			{
				case 1:
				case 2:
				case 3:
					x86_alu_reg_reg(pCompiledCode, X86_XOR, pRegister1, pRegister1);
				case 4:
					if (pSource->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister3, sizeOfSource * pSource->Data.ArrayElement.IndexSource->Data.ConstantI4.Value, sizeOfSource);
					}
					else  if (sizeOfSource == 3)
					{
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfSource);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
						x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister3, 0, sizeOfSource);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_mov_reg_memindex(pCompiledCode, pRegister1, pRegister3, 0, pRegister2, sizeOfSource >> 1, sizeOfSource);
					}
					if (loadToStack)
						x86_mov_membase_reg(pCompiledCode, X86_ESP, stackOffset, pRegister1, 4);
					break;
				default:
				{
					if (pSource->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfSource * pSource->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfSource);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
					}
					x86_adjust_stack(pCompiledCode, JIT_StackAlign(sizeOfSource));
					uint32_t count = sizeOfSource >> gPointerDivideShift;
					for (uint32_t index = 0; index < count; index++)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, (index << gPointerDivideShift), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, (index << gPointerDivideShift) + stackOffset, pRegister2, gSizeOfPointerInBytes);
					}
					uint32_t remainder = sizeOfSource & (gSizeOfPointerInBytes - 1);
					if (remainder)
					{
						x86_alu_reg_reg(pCompiledCode, X86_XOR, pRegister2, pRegister2);
						x86_mov_reg_membase(pCompiledCode, pRegister2, pRegister3, (count << gPointerDivideShift), remainder);
						x86_mov_membase_reg(pCompiledCode, X86_ESP, (count << gPointerDivideShift) + stackOffset, pRegister2, remainder);
					}
					break;
				}
			}
			break;
		}
		case SourceType_ArrayElementAddress:
		{
			sizeOfSource = gSizeOfPointerInBytes;
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.ArrayElementAddress.ArraySource, pRegister1, pRegister2, pRegister3, FALSE, 0, NULL);
			if (pSource->Data.ArrayElementAddress.IndexSource->Type == SourceType_ConstantI4)
			{
				x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, sizeOfSource * pSource->Data.ArrayElementAddress.IndexSource->Data.ConstantI4.Value);
			}
			else
			{
				x86_push_reg(pCompiledCode, pRegister1);
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.ArrayElementAddress.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
				x86_pop_reg(pCompiledCode, pRegister1);
				if (sizeOfSource != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfSource);
				x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister1, pRegister2);
			}
			if (loadToStack)
				x86_mov_membase_reg(pCompiledCode, X86_ESP, stackOffset, pRegister1, 4);
			break;
		}
		case SourceType_ArrayLength:
		{
			sizeOfSource = 4;
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.ArrayLength.ArraySource, pRegister1, pRegister2, pRegister3, FALSE, 0, NULL);
			x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister1, -gSizeOfPointerInBytes, gSizeOfPointerInBytes);
			x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister1, offsetof(GCObject, Array.Length), 4);
			if (loadToStack)
				x86_mov_membase_reg(pCompiledCode, X86_ESP, stackOffset, pRegister1, 4);
			break;
		}
		Source_Default(JIT_Emit_Load);
	}
	if (pSize) *pSize = sizeOfSource;
	return pCompiledCode;
}

char* JIT_Emit_LoadDestinationAddress(char* pCompiledCode, IRMethod* pMethod, SourceTypeData* pDestination, X86_Reg_No pRegister1, X86_Reg_No pRegister2, X86_Reg_No pRegister3)
{
	switch (pDestination->Type)
	{
		case SourceType_Local:
			x86_mov_reg_reg(pCompiledCode, pRegister1, X86_EBP, gSizeOfPointerInBytes);
			if(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset)
			{
				x86_alu_reg_imm(pCompiledCode, X86_SUB, pRegister1, pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset);
			}
			break;
		case SourceType_Parameter:
			x86_mov_reg_reg(pCompiledCode, pRegister1, X86_EBP, gSizeOfPointerInBytes);
			if(pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset)
			{
				x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset);
			}
			break;
		case SourceType_Field:
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister1, pRegister2, pRegister3, FALSE, 0, NULL);
			if (pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex]->Offset)
			{
				x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex]->Offset);
			}
			break;
		case SourceType_StaticField:
			{
				IRField* field = pDestination->Data.StaticField.Field;
				EMIT_Static_Constructor_Call(pRegister3, pRegister1, field, pCompiledCode);

				x86_mov_reg_membase(pCompiledCode, pRegister1, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
				x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister1, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
				x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, field->Offset);
				break;
			}
		case SourceType_Indirect:
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister1, pRegister2, pRegister3, FALSE, 0, NULL);
			break;
		case SourceType_ArrayElement:
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister1, pRegister2, pRegister3, FALSE, 0, NULL);
			x86_push_reg(pCompiledCode, pRegister1);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister3, pRegister1, FALSE, 0, NULL);
			x86_pop_reg(pCompiledCode, pRegister1);
			if (pDestination->Data.ArrayElement.ElementType->Size != 1)
			{
				x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, pDestination->Data.ArrayElement.ElementType->Size);
			}
			x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister1, pRegister2);
			break;
		Define_Bad_Destinations();
	}
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
				default:
				{
					uint32_t count = sizeOfDestination >> gPointerDivideShift;
					for (uint32_t index = 0; index < count; index++)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, (index << gPointerDivideShift), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_EBP, -(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset - (index << gPointerDivideShift)), pRegister2, gSizeOfPointerInBytes);
					}
					x86_adjust_stack(pCompiledCode, -((int32_t)sizeOfDestination));
					break;
				}
			}
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
				default:
				{
					uint32_t count = sizeOfDestination >> gPointerDivideShift;
					for (uint32_t index = 0; index < count; index++)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, (index << gPointerDivideShift), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, X86_EBP, (pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset + (index << gPointerDivideShift)), pRegister2, gSizeOfPointerInBytes);
					}
					x86_adjust_stack(pCompiledCode, -((int32_t)sizeOfDestination));
					break;
				}
			}
			break;
		}
		case SourceType_Field:
		{
			JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
			IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
			sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
			if (sizeOfDestination <= 8)
			{
				x86_push_reg(pCompiledCode, pRegister1);
				if (sizeOfDestination > 4)
				{
					x86_push_reg(pCompiledCode, pRegister2);
				}
			}
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
			if (sizeOfDestination <= 8)
			{
				if (sizeOfDestination > 4)
				{
					x86_pop_reg(pCompiledCode, pRegister2);
				}
				x86_pop_reg(pCompiledCode, pRegister1);
			}
			x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, field->Offset);
			switch (sizeOfDestination)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, sizeOfDestination);
					break;
				default:
				{
					uint32_t count = sizeOfDestination >> gPointerDivideShift;
					for (uint32_t index = 0; index < count; index++)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, (index << gPointerDivideShift), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, pRegister3, (index << gPointerDivideShift), pRegister2, gSizeOfPointerInBytes);
					}
					uint32_t remainder = sizeOfDestination & (gSizeOfPointerInBytes - 1);
					if (remainder)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, (count << gPointerDivideShift), remainder);
						x86_mov_membase_reg(pCompiledCode, pRegister3, (count << gPointerDivideShift), pRegister2, remainder);
					}
					x86_adjust_stack(pCompiledCode, -((int32_t)JIT_StackAlign(sizeOfDestination)));
					break;
				}
			}
			break;
		}
		case SourceType_StaticField:
		{
			IRField* field = pDestination->Data.StaticField.Field;
			sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
			
			EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

			x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
			x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
			switch (sizeOfDestination)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, sizeOfDestination);
					break;
				default:
				{
					uint32_t count = sizeOfDestination >> gPointerDivideShift;
					for (uint32_t index = 0; index < count; index++)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, (index << gPointerDivideShift), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset + (index << gPointerDivideShift), pRegister2, gSizeOfPointerInBytes);
					}
					uint32_t remainder = sizeOfDestination & (gSizeOfPointerInBytes - 1);
					if (remainder)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, (count << gPointerDivideShift), remainder);
						x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset + (count << gPointerDivideShift), pRegister2, remainder);
					}
					x86_adjust_stack(pCompiledCode, -((int32_t)JIT_StackAlign(sizeOfDestination)));
					break;
				}
			}
			break;
		}
		case SourceType_Indirect:
		{
			sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.Indirect.Type);
			if (sizeOfDestination <= 8)
			{
				x86_push_reg(pCompiledCode, pRegister1);
				if (sizeOfDestination > 4)
				{
					x86_push_reg(pCompiledCode, pRegister2);
				}
			}
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
			if (sizeOfDestination <= 8)
			{
				if (sizeOfDestination > 4)
				{
					x86_pop_reg(pCompiledCode, pRegister2);
				}
				x86_pop_reg(pCompiledCode, pRegister1);
			}
			switch (sizeOfDestination)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, sizeOfDestination);
					break;
				default:
				{
					uint32_t count = sizeOfDestination >> gPointerDivideShift;
					for (uint32_t index = 0; index < count; index++)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, (index << gPointerDivideShift), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, pRegister3, (index << gPointerDivideShift), pRegister2, gSizeOfPointerInBytes);
					}
					uint32_t remainder = sizeOfDestination & (gSizeOfPointerInBytes - 1);
					if (remainder)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, (count << gPointerDivideShift), remainder);
						x86_mov_membase_reg(pCompiledCode, pRegister3, (count << gPointerDivideShift), pRegister2, remainder);
					}
					x86_adjust_stack(pCompiledCode, -((int32_t)JIT_StackAlign(sizeOfDestination)));
					break;
				}
			}
			break;
		}
		case SourceType_ArrayElement:
		{
			sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
			if (sizeOfDestination <= 8)
			{
				x86_push_reg(pCompiledCode, pRegister1);
				if (sizeOfDestination > 4)
				{
					x86_push_reg(pCompiledCode, pRegister2);
				}
			}
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
			if (sizeOfDestination <= 8)
			{
				if (sizeOfDestination > 4)
				{
					x86_pop_reg(pCompiledCode, pRegister2);
				}
				x86_pop_reg(pCompiledCode, pRegister1);
			}
			if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
			{
				x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
			}
			else
			{
				if (sizeOfDestination <= 8)
				{
					x86_push_reg(pCompiledCode, pRegister1);
					if (sizeOfDestination > 4)
					{
						x86_push_reg(pCompiledCode, pRegister2);
					}
				}
				x86_push_reg(pCompiledCode, pRegister3);
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
				x86_pop_reg(pCompiledCode, pRegister3);

				if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
				x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);

				if (sizeOfDestination <= 8)
				{
					if (sizeOfDestination > 4)
					{
						x86_pop_reg(pCompiledCode, pRegister2);
					}
					x86_pop_reg(pCompiledCode, pRegister1);
				}
			}
			switch (sizeOfDestination)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, sizeOfDestination);
					break;
				default:
				{
					uint32_t count = sizeOfDestination >> gPointerDivideShift;
					for (uint32_t index = 0; index < count; index++)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, (index << gPointerDivideShift), gSizeOfPointerInBytes);
						x86_mov_membase_reg(pCompiledCode, pRegister3, (index << gPointerDivideShift), pRegister2, gSizeOfPointerInBytes);
					}
					uint32_t remainder = sizeOfDestination & (gSizeOfPointerInBytes - 1);
					if (remainder)
					{
						x86_mov_reg_membase(pCompiledCode, pRegister2, X86_ESP, (count << gPointerDivideShift), remainder);
						x86_mov_membase_reg(pCompiledCode, pRegister3, (count << gPointerDivideShift), pRegister2, remainder);
					}
					x86_adjust_stack(pCompiledCode, -((int32_t)JIT_StackAlign(sizeOfDestination)));
					break;
				}
			}
			break;
		}
		Define_Bad_Destinations();
		Destination_Default(JIT_Emit_Store);
	}
	if (pSize) *pSize = sizeOfDestination;
	return pCompiledCode;
}

char* JIT_Emit_Move(char* pCompiledCode, IRMethod* pMethod, SourceTypeData* pSource, SourceTypeData* pDestination, X86_Reg_No pRegister1, X86_Reg_No pRegister2, X86_Reg_No pRegister3, size_t* pSize)
{
	uint32_t sizeOfSource = 0;
	uint32_t sizeOfDestination = 0;
	switch (pSource->Type)
	{
		case SourceType_Null:
		{
			sizeOfSource = gSizeOfPointerInBytes;
			sizeOfDestination = gSizeOfPointerInBytes;
			switch (pDestination->Type)
			{
				// Null to Local (both aligned)
				case SourceType_Local:
				{
					x86_mov_membase_imm(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, 0, gSizeOfPointerInBytes);
					break;
				}
				// Null to Parameter (both aligned)
				case SourceType_Parameter:
				{
					x86_mov_membase_imm(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, 0, gSizeOfPointerInBytes);
					break;
				}
				// Null to Field (both aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset, 0, gSizeOfPointerInBytes);
					break;
				}
				// Null to Static Field (both aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset, 0, gSizeOfPointerInBytes);
					break;
				}
				// Null to Indirect (both aligned)
				case SourceType_Indirect:
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_mov_membase_imm(pCompiledCode, pRegister3, 0, 0, gSizeOfPointerInBytes);
					break;
				}
				// Null to ArrayElement (both aligned)
				case SourceType_ArrayElement:
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
					}
					x86_mov_membase_imm(pCompiledCode, pRegister3, 0, 0, gSizeOfPointerInBytes);
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Null);
			}
			break;
		}
		// From Local Address
		case SourceType_LocalAddress:
		{
			sizeOfSource = gSizeOfPointerInBytes;
			sizeOfDestination = gSizeOfPointerInBytes;
			x86_mov_reg_reg(pCompiledCode, pRegister1, X86_EBP, gSizeOfPointerInBytes);
			x86_alu_reg_imm(pCompiledCode, X86_SUB, pRegister1, pMethod->LocalVariables[pSource->Data.LocalVariableAddress.LocalVariableIndex]->Offset);
			switch (pDestination->Type)
			{
				// Local Address to Local (both aligned)
				case SourceType_Local:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Local Address to Parameter (both aligned)
				case SourceType_Parameter:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Local Address to Field (both aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Local Address to Static Field (both aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Local Address to Indirect (both aligned)
				case SourceType_Indirect:
				{
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Local Address to ArrayElement (both aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister1);
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_pop_reg(pCompiledCode, pRegister1);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
					}
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Local Address);
			}
			break;
		}
		// From Parameter Address
		case SourceType_ParameterAddress:
		{
			sizeOfSource = gSizeOfPointerInBytes;
			sizeOfDestination = gSizeOfPointerInBytes;
			x86_mov_reg_reg(pCompiledCode, pRegister1, X86_EBP, gSizeOfPointerInBytes);
			x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, pMethod->Parameters[pSource->Data.ParameterAddress.ParameterIndex]->Offset);
			switch (pDestination->Type)
			{
				// Parameter Address to Local (both aligned)
				case SourceType_Local:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Parameter Address to Parameter (both aligned)
				case SourceType_Parameter:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Parameter Address to Field (both aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Parameter Address to Static Field (both aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Parameter Address to Indirect (both aligned)
				case SourceType_Indirect:
				{
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Parameter Address to ArrayElement (both aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister1);
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_pop_reg(pCompiledCode, pRegister1);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
					}
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Parameter Address);
			}
			break;
		}
		// From SizeOf
		case SourceType_SizeOf:
		{
			sizeOfSource = 4;
			uint32_t sizeOfType = JIT_GetSizeOfType(pSource->Data.SizeOf.Type);
			switch (pDestination->Type)
			{
				// SizeOf to Local (both aligned)
				case SourceType_Local:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->VariableType));
					x86_mov_membase_imm(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, sizeOfType, 4);
					break;
				}
				// SizeOf to Parameter (both aligned)
				case SourceType_Parameter:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Type));
					x86_mov_membase_imm(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, sizeOfType, 4);
					break;
				}
				// SizeOf to Field (both aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset, sizeOfType, 4);
					break;
				}
				// SizeOf to Static Field (both aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset, sizeOfType, 4);
					break;
				}
				// SizeOf to Indirect (both aligned)
				case SourceType_Indirect:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.Indirect.Type);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_mov_membase_imm(pCompiledCode, pRegister3, 0, sizeOfType, 4);
					break;
				}
				// SizeOf to ArrayElement (both aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
					}
					x86_mov_membase_imm(pCompiledCode, pRegister3, 0, sizeOfType, 4);
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, SizeOf);
			}
			break;
		}
		// From ConstantI4
		case SourceType_ConstantI4:
		{
			sizeOfSource = 4;
			switch (pDestination->Type)
			{
				// ConstantI4 to Local (both aligned)
				case SourceType_Local:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->VariableType));
					x86_mov_membase_imm(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pSource->Data.ConstantI4.Value, 4);
					break;
				}
				// ConstantI4 to Parameter (both aligned)
				case SourceType_Parameter:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Type));
					x86_mov_membase_imm(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pSource->Data.ConstantI4.Value, 4);
					break;
				}
				// ConstantI4 to Field (both aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset, pSource->Data.ConstantI4.Value, 4);
					break;
				}
				// ConstantI4 to Static Field (both aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset, pSource->Data.ConstantI4.Value, 4);
					break;
				}
				// ConstantI4 to Indirect (both aligned)
				case SourceType_Indirect:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.Indirect.Type);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_mov_membase_imm(pCompiledCode, pRegister3, 0, pSource->Data.ConstantI4.Value, 4);
					break;
				}
				// ConstantI4 to ArrayElement (both aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_mov_membase_imm(pCompiledCode, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value, pSource->Data.ConstantI4.Value, sizeOfDestination);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
						x86_mov_membase_imm(pCompiledCode, pRegister3, 0, pSource->Data.ConstantI4.Value, sizeOfDestination);
					}
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, ConstantI4);
			}
			break;
		}
		// From ConstantI8
		case SourceType_ConstantI8:
		{
			sizeOfSource = 8;
			sizeOfDestination = 8;
			switch (pDestination->Type)
			{
				// ConstantI8 to Local (both aligned)
				case SourceType_Local:
				{
					x86_mov_membase_imm(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pSource->Data.ConstantI8.Value, 4);
					x86_mov_membase_imm(pCompiledCode, X86_EBP, -(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset - 4), pSource->Data.ConstantI8.Value >> 32, 4);
					break;
				}
				// ConstantI8 to Parameter (both aligned)
				case SourceType_Parameter:
				{
					x86_mov_membase_imm(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pSource->Data.ConstantI8.Value, 4);
					x86_mov_membase_imm(pCompiledCode, X86_EBP, (pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset + 4), pSource->Data.ConstantI8.Value >> 32, 4);
					break;
				}
				// ConstantI8 to Field (both aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset, pSource->Data.ConstantI8.Value, 4);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset + 4, pSource->Data.ConstantI8.Value >> 32, 4);
					break;
				}
				// ConstantI8 to Static Field (both aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset, (pSource->Data.ConstantI8.Value & 0xFFFFFFFF), 4);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset + 4, (pSource->Data.ConstantI8.Value >> 32), 4);
					break;
				}
				// ConstantI8 to Indirect (both aligned)
				case SourceType_Indirect:
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_mov_membase_imm(pCompiledCode, pRegister3, 0, pSource->Data.ConstantI8.Value, 4);
					x86_mov_membase_imm(pCompiledCode, pRegister3, 4, pSource->Data.ConstantI8.Value >> 32, 4);
					break;
				}
				// ConstantI8 to ArrayElement (both aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
					}
					x86_mov_membase_imm(pCompiledCode, pRegister3, 0, pSource->Data.ConstantI8.Value, 4);
					x86_mov_membase_imm(pCompiledCode, pRegister3, 4, pSource->Data.ConstantI8.Value >> 32, 4);
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, ConstantI8);
			}
			break;
		}
		// From ConstantR4
		case SourceType_ConstantR4:
		{
			sizeOfSource = 4;
			switch (pDestination->Type)
			{
				// ConstantR4 to Local (both aligned)
				case SourceType_Local:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->VariableType));
					x86_mov_membase_imm(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pSource->Data.ConstantR4.Value, 4);
					break;
				}
				// ConstantR4 to Parameter (both aligned)
				case SourceType_Parameter:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Type));
					x86_mov_membase_imm(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pSource->Data.ConstantR4.Value, 4);
					break;
				}
				// ConstantR4 to Field (both aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset, pSource->Data.ConstantR4.Value, 4);
					break;
				}
				// ConstantR4 to Static Field (both aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset, pSource->Data.ConstantR4.Value, 4);
					break;
				}
				// ConstantR4 to Indirect (both aligned)
				case SourceType_Indirect:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.Indirect.Type);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_mov_membase_imm(pCompiledCode, pRegister3, 0, pSource->Data.ConstantR4.Value, 4);
					break;
				}
				// ConstantR4 to ArrayElement (both aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
					}
					x86_mov_membase_imm(pCompiledCode, pRegister3, 0, pSource->Data.ConstantR4.Value, 4);
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, ConstantR4);
			}
			break;
		}
		// From ConstantR8
		case SourceType_ConstantR8:
		{
			sizeOfSource = 8;
			sizeOfDestination = 8;
			switch (pDestination->Type)
			{
				// ConstantR8 to Local (both aligned)
				case SourceType_Local:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pSource->Data.ConstantR8.Value, 4);
					x86_mov_membase_reg(pCompiledCode, X86_EBP, -(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset - 4), pSource->Data.ConstantR8.Value >> 32, 4);
					break;
				}
				// ConstantR8 to Parameter (both aligned)
				case SourceType_Parameter:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pSource->Data.ConstantR8.Value, 4);
					x86_mov_membase_reg(pCompiledCode, X86_EBP, (pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset + 4), pSource->Data.ConstantR8.Value >> 32, 4);
					break;
				}
				// ConstantR8 to Field (both aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pSource->Data.ConstantR8.Value, 4);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset + 4, pSource->Data.ConstantR8.Value >> 32, 4);
					break;
				}
				// ConstantR8 to Static Field (both aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset, (pSource->Data.ConstantR8.Value & 0xFFFFFFFF), 4);
					x86_mov_membase_imm(pCompiledCode, pRegister3, field->Offset + 4, (pSource->Data.ConstantR8.Value >> 32), 4);
					break;
				}
				// ConstantR8 to Indirect (both aligned)
				case SourceType_Indirect:
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pSource->Data.ConstantR8.Value, 4);
					x86_mov_membase_reg(pCompiledCode, pRegister3, 4, pSource->Data.ConstantR8.Value >> 32, 4);
					break;
				}
				// ConstantR8 to ArrayElement (both aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
					}
					x86_mov_membase_imm(pCompiledCode, pRegister3, 0, pSource->Data.ConstantR8.Value, 4);
					x86_mov_membase_imm(pCompiledCode, pRegister3, 4, pSource->Data.ConstantR8.Value >> 32, 4);
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, ConstantR8);
			}
			break;
		}
		// From Field Address
		case SourceType_FieldAddress:
		{
			JIT_CalculateFieldLayout(pSource->Data.FieldAddress.ParentType);
			sizeOfSource = gSizeOfPointerInBytes;
			sizeOfDestination = gSizeOfPointerInBytes;
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.FieldAddress.FieldSource, pRegister1, pRegister2, pRegister3, FALSE, 0, NULL);
			x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, pSource->Data.FieldAddress.ParentType->Fields[pSource->Data.FieldAddress.FieldIndex]->Offset);
			switch (pDestination->Type)
			{
				// Field Address to Local (both aligned)
				case SourceType_Local:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Field Address to Parameter (both aligned)
				case SourceType_Parameter:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Field Address to Field (both aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Field Address to Static Field (both aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Field Address to Indirect (both aligned)
				case SourceType_Indirect:
				{
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Field Address to ArrayElement (both aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister1);
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_pop_reg(pCompiledCode, pRegister1);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
					}
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Field Address);
			}
			break;
		}
		// From Static Field Address
		case SourceType_StaticFieldAddress:
		{
			IRField* field = pSource->Data.StaticFieldAddress.Field;
			sizeOfSource = gSizeOfPointerInBytes;
			sizeOfDestination = gSizeOfPointerInBytes;
			x86_mov_reg_membase(pCompiledCode, pRegister1, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
			x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister1, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
			x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, field->Offset);
			switch (pDestination->Type)
			{
				// Static Field Address to Local (both aligned)
				case SourceType_Local:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Static Field Address to Parameter (both aligned)
				case SourceType_Parameter:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Static Field Address to Field (both aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Static Field Address to Static Field (both aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Static Field Address to Indirect (both aligned)
				case SourceType_Indirect:
				{
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Static Field Address to ArrayElement (both aligned)
				case SourceType_ArrayElement:
				{
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister1);
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_pop_reg(pCompiledCode, pRegister1);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
					}
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Static Field Address);
			}
			break;
		}
		// From Array Element Address
		case SourceType_ArrayElementAddress:
		{
			uint32_t sizeOfType = JIT_GetStackSizeOfType(pSource->Data.ArrayElementAddress.ElementType);
			sizeOfSource = gSizeOfPointerInBytes;
			sizeOfDestination = gSizeOfPointerInBytes;
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.ArrayElementAddress.ArraySource, pRegister1, pRegister2, pRegister3, FALSE, 0, NULL);
			if (pSource->Data.ArrayElementAddress.IndexSource->Type == SourceType_ConstantI4)
			{
				x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister1, sizeOfType * pSource->Data.ArrayElementAddress.IndexSource->Data.ConstantI4.Value);
			}
			else
			{
				x86_push_reg(pCompiledCode, pRegister1);
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.ArrayElementAddress.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
				x86_pop_reg(pCompiledCode, pRegister1);

				if (sizeOfType != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfType);
				x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister1, pRegister2);
			}
			switch (pDestination->Type)
			{
				// Array Element Address to Local (both aligned)
				case SourceType_Local:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Array Element Address to Parameter (both aligned)
				case SourceType_Parameter:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Array Element Address to Field (both aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Array Element Address to Static Field (both aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Array Element Address to Indirect (both aligned)
				case SourceType_Indirect:
				{
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				// Array Element Address to ArrayElement (both aligned)
				case SourceType_ArrayElement:
				{
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister1);
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_pop_reg(pCompiledCode, pRegister1);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
					}
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, gSizeOfPointerInBytes);
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Array Element Address);
			}
			break;
		}
		// From Array Length
		case SourceType_ArrayLength:
		{
			sizeOfSource = 4;
			sizeOfDestination = 4;
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.ArrayLength.ArraySource, pRegister1, pRegister2, pRegister3, FALSE, 0, NULL);
			x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister1, -gSizeOfPointerInBytes, gSizeOfPointerInBytes);
			x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister1, offsetof(GCObject, Array.Length), 4);
			switch (pDestination->Type)
			{
				// Array Length to Local (both aligned)
				case SourceType_Local:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister1, 4);
					break;
				}
				// Array Length to Parameter (both aligned)
				case SourceType_Parameter:
				{
					x86_mov_membase_reg(pCompiledCode, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister1, 4);
					break;
				}
				// Array Length to Field (both aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, 4);
					break;
				}
				// Array Length to Static Field (both aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					x86_mov_membase_reg(pCompiledCode, pRegister3, field->Offset, pRegister1, 4);
					break;
				}
				// Array Length to Indirect (both aligned)
				case SourceType_Indirect:
				{
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, 4);
					break;
				}
				// Array Length to ArrayElement (both aligned)
				case SourceType_ArrayElement:
				{
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						x86_alu_reg_imm(pCompiledCode, X86_ADD, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister1);
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_pop_reg(pCompiledCode, pRegister1);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
					}
					x86_mov_membase_reg(pCompiledCode, pRegister3, 0, pRegister1, 4);
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Array Length);
			}
			break;
		}
		// From Local
		case SourceType_Local:
		{
			sizeOfSource = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->VariableType));
			switch (pDestination->Type)
			{
				// Local to Local (both aligned)
				case SourceType_Local:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->VariableType));
					if (pSource->Data.LocalVariable.LocalVariableIndex != pDestination->Data.LocalVariable.LocalVariableIndex)
					{
						Define_Move_To_Destination(X86_EBP, -pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister2, TRUE, TRUE);
					}
					break;
				}
				// Local to Parameter (both aligned)
				case SourceType_Parameter:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Type));
					Define_Move_To_Destination(X86_EBP, -pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister2, TRUE, TRUE);
					break;
				}
				// Local to Field (source aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					Define_Move_To_Destination(X86_EBP, -pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister3, field->Offset, pRegister2, TRUE, FALSE);
					break;
				}
				// Local to Static Field (source aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					Define_Move_To_Destination(X86_EBP, -pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister3, field->Offset, pRegister2, TRUE, FALSE);
					break;
				}
				// Local to Indirect (source aligned)
				case SourceType_Indirect:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.Indirect.Type);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					Define_Move_To_Destination(X86_EBP, -pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister3, 0, pRegister2, TRUE, FALSE);
					break;
				}
				// Local to ArrayElement (source aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						Define_Move_To_Destination(X86_EBP, -pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value, pRegister2, TRUE, FALSE);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
						Define_Move_To_Destination(X86_EBP, -pMethod->LocalVariables[pSource->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister3, 0, pRegister2, TRUE, FALSE);
					}
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Local);
			}
			break;
		}
		// From Parameter
		case SourceType_Parameter:
		{
			sizeOfSource = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Type));
			switch (pDestination->Type)
			{
				// Parameter to Local (both aligned)
				case SourceType_Local:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->VariableType));
					Define_Move_To_Destination(X86_EBP, pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister2, TRUE, TRUE);
					break;
				}
				// Parameter to Parameter (both aligned)
				case SourceType_Parameter:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Type));
					Define_Move_To_Destination(X86_EBP, pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister2, TRUE, TRUE);
					break;
				}
				// Parameter to Field (source aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					Define_Move_To_Destination(X86_EBP, pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset, pRegister3, field->Offset, pRegister2, TRUE, FALSE);
					break;
				}
				// Parameter to Static Field (source aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
					Define_Move_To_Destination(X86_EBP, pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset, pRegister3, field->Offset, pRegister2, TRUE, FALSE);
					break;
				}
				// Parameter to Indirect (source aligned)
				case SourceType_Indirect:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.Indirect.Type);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					Define_Move_To_Destination(X86_EBP, pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset, pRegister3, 0, pRegister2, TRUE, FALSE);
					break;
				}
				// Parameter to ArrayElement (source aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						Define_Move_To_Destination(X86_EBP, pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value, pRegister2, TRUE, FALSE);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
						Define_Move_To_Destination(X86_EBP, pMethod->Parameters[pSource->Data.Parameter.ParameterIndex]->Offset, pRegister3, 0, pRegister2, TRUE, FALSE);
					}
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Parameter);
			}
			break;
		}
		// From Field
		case SourceType_Field:
		{
			JIT_CalculateFieldLayout(pSource->Data.Field.ParentType);
			IRField* sourceField = pSource->Data.Field.ParentType->Fields[pSource->Data.Field.FieldIndex];
			sizeOfSource = JIT_GetStackSizeOfType(sourceField->FieldType);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.Field.FieldSource, pRegister1, pRegister2, pRegister3, FALSE, 0, NULL);
			switch (pDestination->Type)
			{
				// Field to Local (destination aligned)
				case SourceType_Local:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->VariableType));
					Define_Move_To_Destination(pRegister1, sourceField->Offset, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister2, FALSE, TRUE);
					break;
				}
				// Field to Parameter (destination aligned)
				case SourceType_Parameter:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Type));
					Define_Move_To_Destination(pRegister1, sourceField->Offset, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister2, FALSE, TRUE);
					break;
				}
				// Field to Field (neither aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					Define_Move_To_Destination(pRegister1, sourceField->Offset, pRegister3, field->Offset, pRegister2, FALSE, FALSE);
					break;
				}
				// Field to Static Field (neither aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);

					Define_Move_To_Destination(pRegister1, sourceField->Offset, pRegister3, field->Offset, pRegister2, FALSE, FALSE);
					break;
				}
				// Field to Indirect (neither aligned)
				case SourceType_Indirect:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.Indirect.Type);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					Define_Move_To_Destination(pRegister1, sourceField->Offset, pRegister3, 0, pRegister2, FALSE, FALSE);
					break;
				}
				// Field to ArrayElement (neither aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						Define_Move_To_Destination(pRegister1, sourceField->Offset, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value, pRegister2, FALSE, FALSE);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister1);
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_pop_reg(pCompiledCode, pRegister1);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
						Define_Move_To_Destination(pRegister1, sourceField->Offset, pRegister3, 0, pRegister2, FALSE, FALSE);
					}
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Field);
			}
			break;
		}
		// From Static Field
		case SourceType_StaticField:
		{
			IRField* staticfield = pSource->Data.StaticField.Field;
			sizeOfSource = JIT_GetStackSizeOfType(staticfield->FieldType);
			EMIT_Static_Constructor_Call(pRegister1, pRegister2, staticfield, pCompiledCode);

			x86_mov_reg_membase(pCompiledCode, pRegister1, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
			x86_mov_reg_membase(pCompiledCode, pRegister1, pRegister1, (staticfield->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);
			switch (pDestination->Type)
			{
				// Static Field to Local (destination aligned)
				case SourceType_Local:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->VariableType));
					Define_Move_To_Destination(pRegister1, staticfield->Offset, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister2, FALSE, TRUE);
					break;
				}
				// Static Field to Parameter (destination aligned)
				case SourceType_Parameter:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Type));
					Define_Move_To_Destination(pRegister1, staticfield->Offset, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister2, FALSE, TRUE);
					break;
				}
				// Static Field to Field (neither aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					Define_Move_To_Destination(pRegister1, staticfield->Offset, pRegister3, field->Offset, pRegister2, FALSE, FALSE);
					break;
				}
				// Static Field to Static Field (neither aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					if (field->FieldType != staticfield->FieldType)
					{
						EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);
					}

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);

					Define_Move_To_Destination(pRegister1, staticfield->Offset, pRegister3, field->Offset, pRegister2, FALSE, FALSE);
					break;
				}
				// Static Field to Indirect (neither aligned)
				case SourceType_Indirect:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.Indirect.Type);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					Define_Move_To_Destination(pRegister1, staticfield->Offset, pRegister3, 0, pRegister2, FALSE, FALSE);
					break;
				}
				// Static Field to ArrayElement (neither aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						Define_Move_To_Destination(pRegister1, staticfield->Offset, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value, pRegister2, FALSE, FALSE);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister1);
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_pop_reg(pCompiledCode, pRegister1);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
						Define_Move_To_Destination(pRegister1, staticfield->Offset, pRegister3, 0, pRegister2, FALSE, FALSE);
					}
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Static Field);
			}
			break;
		}
		// From Indirect
		case SourceType_Indirect:
		{
			sizeOfSource = JIT_GetStackSizeOfType(pSource->Data.Indirect.Type);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.Indirect.AddressSource, pRegister1, pRegister2, pRegister3, FALSE, 0, NULL);
			switch (pDestination->Type)
			{
				// Indirect to Local (destination aligned)
				case SourceType_Local:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->VariableType));
					Define_Move_To_Destination(pRegister1, 0, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister2, FALSE, TRUE);
					break;
				}
				// Indirect to Parameter (destination aligned)
				case SourceType_Parameter:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Type));
					Define_Move_To_Destination(pRegister1, 0, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister2, FALSE, TRUE);
					break;
				}
				// Indirect to Field (neither aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					Define_Move_To_Destination(pRegister1, 0, pRegister3, field->Offset, pRegister2, FALSE, FALSE);
					break;
				}
				// Indirect to Static Field (neither aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);

					Define_Move_To_Destination(pRegister1, 0, pRegister3, field->Offset, pRegister2, FALSE, FALSE);
					break;
				}
				// Indirect to Indirect (neither aligned)
				case SourceType_Indirect:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.Indirect.Type);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					Define_Move_To_Destination(pRegister1, 0, pRegister3, 0, pRegister2, FALSE, FALSE);
					break;
				}
				// Indirect to ArrayElement (neither aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						Define_Move_To_Destination(pRegister1, 0, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value, pRegister2, FALSE, FALSE);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister1);
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_pop_reg(pCompiledCode, pRegister1);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
						Define_Move_To_Destination(pRegister1, 0, pRegister3, 0, pRegister2, FALSE, FALSE);
					}
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Indirect);
			}			
			break;
		}
		// From Array Element
		case SourceType_ArrayElement:
		{
			sizeOfSource = JIT_GetStackSizeOfType(pSource->Data.ArrayElement.ElementType);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.ArrayElement.ArraySource, pRegister1, pRegister2, pRegister3, FALSE, 0, NULL);
			uint32_t sourceBase = 0;
			if (pSource->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
			{
				sourceBase = sizeOfSource * pSource->Data.ArrayElement.IndexSource->Data.ConstantI4.Value;
			}
			else
			{
				x86_push_reg(pCompiledCode, pRegister1);
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pSource->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
				x86_pop_reg(pCompiledCode, pRegister1);

				if (sizeOfSource != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfSource);
				x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister1, pRegister2);
			}
			switch (pDestination->Type)
			{
				// Array Element to Local (destination aligned)
				case SourceType_Local:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->VariableType));
					Define_Move_To_Destination(pRegister1, sourceBase, X86_EBP, -pMethod->LocalVariables[pDestination->Data.LocalVariable.LocalVariableIndex]->Offset, pRegister2, FALSE, TRUE);
					break;
				}
				// Array Element to Parameter (destination aligned)
				case SourceType_Parameter:
				{
					sizeOfDestination = JIT_StackAlign(JIT_GetStackSizeOfType(pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Type));
					Define_Move_To_Destination(pRegister1, sourceBase, X86_EBP, pMethod->Parameters[pDestination->Data.Parameter.ParameterIndex]->Offset, pRegister2, FALSE, TRUE);
					break;
				}
				// Array Element to Field (neither aligned)
				case SourceType_Field:
				{
					JIT_CalculateFieldLayout(pDestination->Data.Field.ParentType);
					IRField* field = pDestination->Data.Field.ParentType->Fields[pDestination->Data.Field.FieldIndex];
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Field.FieldSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					Define_Move_To_Destination(pRegister1, sourceBase, pRegister3, field->Offset, pRegister2, FALSE, FALSE);
					break;
				}
				// Array Element to Static Field (neither aligned)
				case SourceType_StaticField:
				{
					IRField* field = pDestination->Data.StaticField.Field;
					sizeOfDestination = JIT_GetStackSizeOfType(field->FieldType);
					EMIT_Static_Constructor_Call(pRegister3, pRegister2, field, pCompiledCode);

					x86_mov_reg_membase(pCompiledCode, pRegister3, DOMAIN_REG, offsetof(AppDomain, StaticValues), gSizeOfPointerInBytes);
					x86_mov_reg_membase(pCompiledCode, pRegister3, pRegister3, (field->ParentAssembly->AssemblyIndex << gPointerDivideShift), gSizeOfPointerInBytes);

					Define_Move_To_Destination(pRegister1, sourceBase, pRegister3, field->Offset, pRegister2, FALSE, FALSE);
					break;
				}
				// Array Element to Indirect (neither aligned)
				case SourceType_Indirect:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.Indirect.Type);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.Indirect.AddressSource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					Define_Move_To_Destination(pRegister1, sourceBase, pRegister3, 0, pRegister2, FALSE, FALSE);
					break;
				}
				// Array Element to ArrayElement (neither aligned)
				case SourceType_ArrayElement:
				{
					sizeOfDestination = JIT_GetStackSizeOfType(pDestination->Data.ArrayElement.ElementType);
					x86_push_reg(pCompiledCode, pRegister1);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.ArraySource, pRegister3, pRegister2, pRegister1, FALSE, 0, NULL);
					x86_pop_reg(pCompiledCode, pRegister1);
					if (pDestination->Data.ArrayElement.IndexSource->Type == SourceType_ConstantI4)
					{
						Define_Move_To_Destination(pRegister1, sourceBase, pRegister3, sizeOfDestination * pDestination->Data.ArrayElement.IndexSource->Data.ConstantI4.Value, pRegister2, FALSE, FALSE);
					}
					else
					{
						x86_push_reg(pCompiledCode, pRegister1);
						x86_push_reg(pCompiledCode, pRegister3);
						pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, pDestination->Data.ArrayElement.IndexSource, pRegister2, pRegister1, pRegister3, FALSE, 0, NULL);
						x86_pop_reg(pCompiledCode, pRegister3);
						x86_pop_reg(pCompiledCode, pRegister1);

						if (sizeOfDestination != 1) x86_imul_reg_reg_imm(pCompiledCode, pRegister2, pRegister2, sizeOfDestination);
						x86_alu_reg_reg(pCompiledCode, X86_ADD, pRegister3, pRegister2);
						Define_Move_To_Destination(pRegister1, sourceBase, pRegister3, 0, pRegister2, FALSE, FALSE);
					}
					break;
				}
				Define_Bad_Destinations();
				Move_Destination_Default(JIT_Emit_Move, Array Element);
			}
			break;
		}
		Source_Default(JIT_Emit_Move);
	}
	if (pSize) *pSize = sizeOfDestination;
	return pCompiledCode;
}


void JIT_BranchLinker(BranchRegistry* pBranchRegistry)
{
	for (uint32_t index = 0; index <= pBranchRegistry->InstructionCount; ++index)
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

	uint32_t localsSize = pMethod->LocalsSize;

	// Create stack space for locals
	x86_adjust_stack(pCompiledCode, localsSize);

	if (localsSize > 0) // &&
//		(pMethod->MethodDefinition->Body.Flags & MethodDefinitionBody_Fat_Flags_InitializeLocals))
	{
		for (uint32_t offset = 0; offset < localsSize; offset += 4)
		{
			// Initialize memory to zero for all of the local space
			x86_mov_membase_imm(pCompiledCode, X86_ESP, offset, 0, 4);
		}
	}

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
	x86_breakpoint(pCompiledCode);
	return pCompiledCode;
}

char* JIT_Emit_Move_OpCode(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source1, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
	return pCompiledCode;
}

char* JIT_Emit_Return(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	// Register a branch target for return to immediately jump into epilogue
	//BranchRegistry_RegisterBranch(pBranchRegistry, pInstruction->ILLocation, pBranchRegistry->InstructionCount + 1, pCompiledCode);
	//x86_jump32(pCompiledCode, 0);

	if (pMethod->Returns)
	{
		// Pop the value into EAX, as per cdecl call convention
		pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, X86_EAX, X86_EDX, X86_ECX, FALSE, 0, NULL);
		uint32_t sizeOfReturnType = JIT_GetStackSizeOfType(pMethod->ReturnType);
		if (sizeOfReturnType > 4 && sizeOfReturnType <= 8)
		{
			// If the size of the return type is 5-8 bytes, pop the rest into EDX as per cdecl call convention
			x86_pop_reg(pCompiledCode, X86_EDX);
		}
		else if (sizeOfReturnType > 8)
		{
			uint32_t sizeOfDestination = JIT_StackAlign(sizeOfReturnType);
			uint32_t sizeOfSource = sizeOfDestination;
			Define_Move_To_Destination(X86_ESP, 0, X86_EBP, (pMethod->Parameters[pMethod->ParameterCount - 1]->Offset + JIT_StackAlign(pMethod->Parameters[pMethod->ParameterCount - 1]->Size)), THIRD_REG, TRUE, TRUE);
		}
	}
	// Adjust the stack for local variables, and restore the previous stack frame
	x86_leave(pCompiledCode);
	// Pop EIP and return back to caller method
	x86_ret(pCompiledCode);
	return pCompiledCode;
}

char* JIT_Emit_Load_String(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	// No need to adjust stack stream, AllocateString does not call back into managed code
	// and maintains GC lock until it has written to the void** in ESP+8, which points to
	// the destination local variable directly, ensuring GC won't fail to mark it as alive
	// No harm is done if GC interrupts after adjusting stack but before call completes as
	// long as stack streams are cleared to zero when managed calls return so that the bits
	// are zero for these values if the GC does interrupt
	pCompiledCode = JIT_Emit_LoadDestinationAddress(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG);
	x86_adjust_stack(pCompiledCode, (gSizeOfPointerInBytes * 3) + 4);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, (gSizeOfPointerInBytes << 1) + 4, PRIMARY_REG, gSizeOfPointerInBytes);
	x86_mov_membase_imm(pCompiledCode, X86_ESP, gSizeOfPointerInBytes << 1, (uint32_t)pInstruction->Arg1, 4);
	x86_mov_membase_imm(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, (size_t)pInstruction->Arg2, gSizeOfPointerInBytes);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, DOMAIN_REG, gSizeOfPointerInBytes);
	x86_call_code(pCompiledCode, GC_AllocateInternedStringFromUnicode);
	x86_mov_reg_membase(pCompiledCode, DOMAIN_REG, X86_ESP, 0, gSizeOfPointerInBytes);
	x86_adjust_stack(pCompiledCode, -((gSizeOfPointerInBytes * 3) + 4));
	
	return pCompiledCode;
}

char* JIT_Emit_Dup(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source1, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
	pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source1, &pInstruction->Source3, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
	return pCompiledCode;
}

char* JIT_Emit_Add(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	switch((OverflowType)(uint32_t)pInstruction->Arg1)
	{
		case OverflowType_Signed:
		case OverflowType_Unsigned:
			Panic("Unsupported overflow type!");
			break;
		case OverflowType_None: break;
	}
	ElementType sAType = (ElementType)(uint32_t)pInstruction->Arg2;
	ElementType sBType = (ElementType)(uint32_t)pInstruction->Arg3;
	ElementType destType = (ElementType)0;
	switch(sAType)
	{
		case ElementType_I:
		case ElementType_U:
		case ElementType_I1:
		case ElementType_I2:
		case ElementType_I4:
		case ElementType_U1:
		case ElementType_U2:
		case ElementType_U4:
			switch(sBType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
					destType = ElementType_I4;
					break;
				case ElementType_I8:
				case ElementType_U8:
					destType = ElementType_I8;
					break;
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid parameter!");
					break;
			}
			break;
		case ElementType_I8:
		case ElementType_U8:
			destType = ElementType_I8;
			break;
		case ElementType_R4:
		case ElementType_R8:
			destType = ElementType_R8;
			break;
	}

	if (SourceTypeData_Equal(pInstruction->Source1, pInstruction->Destination))
	{
		if (pInstruction->Source2.Type == SourceType_ConstantI4)
		{
			if (pInstruction->Source1.Type == SourceType_Local)
			{
				x86_alu_membase_imm(pCompiledCode, X86_ADD, X86_EBP, -pMethod->LocalVariables[pInstruction->Source1.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source2.Data.ConstantI4.Value);
			}
			else if (pInstruction->Source1.Type == SourceType_Parameter)
			{
				x86_alu_membase_imm(pCompiledCode, X86_ADD, X86_EBP, pMethod->Parameters[pInstruction->Source1.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source2.Data.ConstantI4.Value);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_reg_imm(pCompiledCode, X86_ADD, PRIMARY_REG, pInstruction->Source2.Data.ConstantI4.Value);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			goto EmitFinished;
		}
	}
	else if (SourceTypeData_Equal(pInstruction->Source2, pInstruction->Destination))
	{
		if (pInstruction->Source1.Type == SourceType_ConstantI4)
		{
			if (pInstruction->Source2.Type == SourceType_Local)
			{
				x86_alu_membase_imm(pCompiledCode, X86_ADD, X86_EBP, -pMethod->LocalVariables[pInstruction->Source2.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
			}
			else if (pInstruction->Source2.Type == SourceType_Parameter)
			{
				x86_alu_membase_imm(pCompiledCode, X86_ADD, X86_EBP, pMethod->Parameters[pInstruction->Source2.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_reg_imm(pCompiledCode, X86_ADD, PRIMARY_REG, pInstruction->Source1.Data.ConstantI4.Value);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			goto EmitFinished;
		}
	}

	switch(destType)
	{
		case ElementType_I4:
		{
			if (pInstruction->Destination.Type == SourceType_Local)
			{
				pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source2, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
				if (pInstruction->Source1.Type == SourceType_ConstantI4)
				{
					x86_alu_membase_imm(pCompiledCode, X86_ADD, X86_EBP, -pMethod->LocalVariables[pInstruction->Destination.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
				}
				else
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_alu_membase_reg(pCompiledCode, X86_ADD, X86_EBP, -pMethod->LocalVariables[pInstruction->Destination.Data.LocalVariable.LocalVariableIndex]->Offset, PRIMARY_REG);
				}
			}
			else if (pInstruction->Destination.Type == SourceType_Parameter)
			{
				pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source2, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
				if (pInstruction->Source1.Type == SourceType_ConstantI4)
				{
					x86_alu_membase_imm(pCompiledCode, X86_ADD, X86_EBP, pMethod->Parameters[pInstruction->Destination.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
				}
				else
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_alu_membase_reg(pCompiledCode, X86_ADD, X86_EBP, pMethod->Parameters[pInstruction->Destination.Data.Parameter.ParameterIndex]->Offset, PRIMARY_REG);
				}
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				if (pInstruction->Source1.Type == SourceType_ConstantI4)
				{
					x86_alu_reg_imm(pCompiledCode, X86_ADD, PRIMARY_REG, pInstruction->Source1.Data.ConstantI4.Value);
				}
				else
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, FOURTH_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_alu_reg_reg(pCompiledCode, X86_ADD, FOURTH_REG, PRIMARY_REG);
				}
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, FOURTH_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			break;
		}
		case ElementType_I8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
					// This means that operand 2 is 64-bit.
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					x86_fp_int_op_membase(pCompiledCode, X86_FADD, X86_ESP, 0, TRUE);
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I8:
				case ElementType_U8:
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					size_t arg2Size = 0;
					x86_adjust_stack(pCompiledCode, 8);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &arg2Size);
					if (arg2Size <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fp_int_op_membase(pCompiledCode, X86_FADD, X86_ESP, 0, TRUE);
						x86_adjust_stack(pCompiledCode, -4);
					}
					else
					{
						x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
						x86_fp_op(pCompiledCode, X86_FADD, 1);
					}
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid operand type!");
					break;
			}

			break;
		}
		case ElementType_R8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
				case ElementType_I8:
				case ElementType_U8:
					Panic("Invalid operand type!");
					break;
				case ElementType_R4:
				case ElementType_R8:
				{
					size_t argSize = 0;
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &argSize);
					if (argSize <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
						x86_adjust_stack(pCompiledCode, 4);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
						x86_adjust_stack(pCompiledCode, 8);
					}
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &argSize);
					if (argSize <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
						x86_adjust_stack(pCompiledCode, -4);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					}
					x86_fp_op(pCompiledCode, X86_FADD, 1);
					x86_fst_membase(pCompiledCode, X86_ESP, 0, TRUE, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
			}
			break;
		}
		default:
			Panic("Invalid destination type!");
			break;
	}

EmitFinished:
	return pCompiledCode;
}

char* JIT_Emit_Sub(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	switch((OverflowType)(uint32_t)pInstruction->Arg1)
	{
		case OverflowType_Signed:
		case OverflowType_Unsigned:
			Panic("Unsupported overflow type!");
			break;
		case OverflowType_None: break;
	}
	ElementType sAType = (ElementType)(uint32_t)pInstruction->Arg2;
	ElementType sBType = (ElementType)(uint32_t)pInstruction->Arg3;
	ElementType destType = (ElementType)0;
	switch(sAType)
	{
		case ElementType_I:
		case ElementType_U:
		case ElementType_I1:
		case ElementType_I2:
		case ElementType_I4:
		case ElementType_U1:
		case ElementType_U2:
		case ElementType_U4:
			switch(sBType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
					destType = ElementType_I4;
					break;
				case ElementType_I8:
				case ElementType_U8:
					destType = ElementType_I8;
					break;
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid parameter!");
					break;
			}
			break;
		case ElementType_I8:
		case ElementType_U8:
			destType = ElementType_I8;
			break;
		case ElementType_R4:
		case ElementType_R8:
			destType = ElementType_R8;
			break;
	}

	if (SourceTypeData_Equal(pInstruction->Source1, pInstruction->Destination))
	{
		if (pInstruction->Source2.Type == SourceType_ConstantI4)
		{
			if (pInstruction->Source1.Type == SourceType_Local)
			{
				x86_alu_membase_imm(pCompiledCode, X86_SUB, X86_EBP, -pMethod->LocalVariables[pInstruction->Source1.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source2.Data.ConstantI4.Value);
			}
			else if (pInstruction->Source1.Type == SourceType_Parameter)
			{
				x86_alu_membase_imm(pCompiledCode, X86_SUB, X86_EBP, pMethod->Parameters[pInstruction->Source1.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source2.Data.ConstantI4.Value);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_reg_imm(pCompiledCode, X86_SUB, PRIMARY_REG, pInstruction->Source2.Data.ConstantI4.Value);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			goto EmitFinished;
		}
	}
	else if (SourceTypeData_Equal(pInstruction->Source2, pInstruction->Destination))
	{
		if (pInstruction->Source1.Type == SourceType_ConstantI4)
		{
			if (pInstruction->Source2.Type == SourceType_Local)
			{
				x86_alu_membase_imm(pCompiledCode, X86_SUB, X86_EBP, -pMethod->LocalVariables[pInstruction->Source2.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
			}
			else if (pInstruction->Source2.Type == SourceType_Parameter)
			{
				x86_alu_membase_imm(pCompiledCode, X86_SUB, X86_EBP, pMethod->Parameters[pInstruction->Source2.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_reg_imm(pCompiledCode, X86_SUB, PRIMARY_REG, pInstruction->Source1.Data.ConstantI4.Value);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			goto EmitFinished;
		}
	}

	switch(destType)
	{
		case ElementType_I4:
		{
			if (pInstruction->Destination.Type == SourceType_Local)
			{
				pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source2, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_membase_reg(pCompiledCode, X86_SUB, X86_EBP, -pMethod->LocalVariables[pInstruction->Destination.Data.LocalVariable.LocalVariableIndex]->Offset, PRIMARY_REG);
			}
			else if (pInstruction->Destination.Type == SourceType_Parameter)
			{
				pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source2, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_membase_reg(pCompiledCode, X86_SUB, X86_EBP, pMethod->Parameters[pInstruction->Destination.Data.Parameter.ParameterIndex]->Offset, PRIMARY_REG);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, FOURTH_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_reg_reg(pCompiledCode, X86_SUB, FOURTH_REG, PRIMARY_REG);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, FOURTH_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			break;
		}
		case ElementType_I8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
					// This means that operand 2 is 64-bit.
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					x86_fp_int_op_membase(pCompiledCode, X86_FSUB, X86_ESP, 0, TRUE);
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I8:
				case ElementType_U8:
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					size_t arg2Size = 0;
					x86_adjust_stack(pCompiledCode, 8);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &arg2Size);
					if (arg2Size <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fp_int_op_membase(pCompiledCode, X86_FSUB, X86_ESP, 0, TRUE);
						x86_adjust_stack(pCompiledCode, -4);
					}
					else
					{
						x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
						x86_fp_op(pCompiledCode, X86_FADD, 1);
					}
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid operand type!");
					break;
			}

			break;
		}
		case ElementType_R8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
				case ElementType_I8:
				case ElementType_U8:
					Panic("Invalid operand type!");
					break;
				case ElementType_R4:
				case ElementType_R8:
				{
					size_t argSize = 0;
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &argSize);
					if (argSize <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
						x86_adjust_stack(pCompiledCode, 4);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
						x86_adjust_stack(pCompiledCode, 8);
					}
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &argSize);
					if (argSize <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
						x86_adjust_stack(pCompiledCode, -4);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					}
					x86_fp_op(pCompiledCode, X86_FSUB, 1);
					x86_fst_membase(pCompiledCode, X86_ESP, 0, TRUE, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
			}
			break;
		}
		default:
			Panic("Invalid destination type!");
			break;
	}

EmitFinished:
	return pCompiledCode;
}

char* JIT_Emit_Mul(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	switch((OverflowType)(uint32_t)pInstruction->Arg1)
	{
		case OverflowType_Signed:
		case OverflowType_Unsigned:
			Panic("Unsupported overflow type!");
			break;
		case OverflowType_None: break;
	}
	ElementType sAType = (ElementType)(uint32_t)pInstruction->Arg2;
	ElementType sBType = (ElementType)(uint32_t)pInstruction->Arg3;
	ElementType destType = (ElementType)0;
	switch(sAType)
	{
		case ElementType_I:
		case ElementType_U:
		case ElementType_I1:
		case ElementType_I2:
		case ElementType_I4:
		case ElementType_U1:
		case ElementType_U2:
		case ElementType_U4:
			switch(sBType)
			{
				case ElementType_I:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
					destType = ElementType_I4;
					break;
				case ElementType_U:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
					destType = ElementType_U4;
					break;
				case ElementType_I8:
					destType = ElementType_I8;
					break;
				case ElementType_U8:
					destType = ElementType_U8;
					break;
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid parameter!");
					break;
			}
			break;
		case ElementType_I8:
			destType = ElementType_I8;
			break;
		case ElementType_U8:
			if (sBType == ElementType_I8) destType = ElementType_I8;
			else destType = ElementType_U8;
			break;
		case ElementType_R4:
		case ElementType_R8:
			destType = ElementType_R8;
			break;
	}

	if (SourceTypeData_Equal(pInstruction->Source1, pInstruction->Destination))
	{
		if (pInstruction->Source2.Type == SourceType_ConstantI4)
		{
			if (pInstruction->Source1.Type == SourceType_Local)
			{
				x86_imul_reg_membase_imm(pCompiledCode, PRIMARY_REG, X86_EBP, -pMethod->LocalVariables[pInstruction->Source1.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source2.Data.ConstantI4.Value);
				x86_mov_membase_reg(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pInstruction->Source1.Data.LocalVariable.LocalVariableIndex]->Offset, PRIMARY_REG, 4);
			}
			else if (pInstruction->Source1.Type == SourceType_Parameter)
			{
				x86_imul_reg_membase_imm(pCompiledCode, PRIMARY_REG, X86_EBP, pMethod->Parameters[pInstruction->Source1.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source2.Data.ConstantI4.Value);
				x86_mov_membase_reg(pCompiledCode, X86_EBP, pMethod->Parameters[pInstruction->Source1.Data.Parameter.ParameterIndex]->Offset, PRIMARY_REG, 4);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_imul_reg_reg_imm(pCompiledCode, PRIMARY_REG, PRIMARY_REG, pInstruction->Source2.Data.ConstantI4.Value);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			goto EmitFinished;
		}
	}
	else if (SourceTypeData_Equal(pInstruction->Source2, pInstruction->Destination))
	{
		if (pInstruction->Source1.Type == SourceType_ConstantI4)
		{
			if (pInstruction->Source2.Type == SourceType_Local)
			{
				x86_imul_reg_membase_imm(pCompiledCode, PRIMARY_REG, X86_EBP, -pMethod->LocalVariables[pInstruction->Source2.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
				x86_mov_membase_reg(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pInstruction->Source2.Data.LocalVariable.LocalVariableIndex]->Offset, PRIMARY_REG, 4);
			}
			else if (pInstruction->Source2.Type == SourceType_Parameter)
			{
				x86_imul_reg_membase_imm(pCompiledCode, PRIMARY_REG, X86_EBP, pMethod->Parameters[pInstruction->Source2.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
				x86_mov_membase_reg(pCompiledCode, X86_EBP, pMethod->Parameters[pInstruction->Source2.Data.Parameter.ParameterIndex]->Offset, PRIMARY_REG, 4);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_imul_reg_reg_imm(pCompiledCode, PRIMARY_REG, PRIMARY_REG, pInstruction->Source1.Data.ConstantI4.Value);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			goto EmitFinished;
		}
	}

	switch(destType)
	{
		case ElementType_I4:
		{
			if (pInstruction->Destination.Type == SourceType_Local)
			{
				pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source2, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_imul_reg_membase(pCompiledCode, PRIMARY_REG, X86_EBP, -pMethod->LocalVariables[pInstruction->Destination.Data.LocalVariable.LocalVariableIndex]->Offset);
				x86_mov_membase_reg(pCompiledCode, X86_EBP, -pMethod->LocalVariables[pInstruction->Destination.Data.LocalVariable.LocalVariableIndex]->Offset, PRIMARY_REG, 4);
			}
			else if (pInstruction->Destination.Type == SourceType_Parameter)
			{
				pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source2, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_imul_reg_membase(pCompiledCode, PRIMARY_REG, X86_EBP, pMethod->Parameters[pInstruction->Destination.Data.Parameter.ParameterIndex]->Offset);
				x86_mov_membase_reg(pCompiledCode, X86_EBP, pMethod->Parameters[pInstruction->Destination.Data.Parameter.ParameterIndex]->Offset, PRIMARY_REG, 4);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, FOURTH_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_imul_reg_reg(pCompiledCode, FOURTH_REG, PRIMARY_REG);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, FOURTH_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			break;
		}
		case ElementType_U4:
		{
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, FOURTH_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			if (PRIMARY_REG != X86_EAX) Panic("DIE EVIL DOEER!");
			x86_mul_reg(pCompiledCode, FOURTH_REG, FALSE);
			pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, FOURTH_REG, SECONDARY_REG, THIRD_REG, NULL);
			break;
		}
		case ElementType_I8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
					// This means that operand 2 is 64-bit.
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					x86_fp_int_op_membase(pCompiledCode, X86_FMUL, X86_ESP, 0, TRUE);
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I8:
				case ElementType_U8:
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					size_t arg2Size = 0;
					x86_adjust_stack(pCompiledCode, 8);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &arg2Size);
					if (arg2Size <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fp_int_op_membase(pCompiledCode, X86_FMUL, X86_ESP, 0, TRUE);
						x86_adjust_stack(pCompiledCode, -4);
					}
					else
					{
						x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
						x86_fp_op(pCompiledCode, X86_FMUL, 1);
					}
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid operand type!");
					break;
			}
			break;
		}
		case ElementType_U8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
				{
					// This means that operand 2 is 64-bit.
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_adjust_stack(pCompiledCode, 8);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, FALSE);
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_adjust_stack(pCompiledCode, 8);
					x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 16, 4);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 4, PRIMARY_REG, 4);
					x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 20, 4);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					x86_mov_reg_membase(pCompiledCode, SECONDARY_REG, X86_ESP, 8, 4);
					x86_mul_reg(pCompiledCode, SECONDARY_REG, FALSE);
					x86_xchg_reg_reg(pCompiledCode, PRIMARY_REG, SECONDARY_REG, 4);
					x86_mul_membase(pCompiledCode, X86_ESP, 4, FALSE);
					x86_xchg_membase_reg(pCompiledCode, X86_ESP, 4, PRIMARY_REG, 4);
					x86_alu_reg_reg(pCompiledCode, X86_ADD, SECONDARY_REG, X86_EDX);
					x86_mul_membase(pCompiledCode, X86_ESP, 12, FALSE);
					x86_alu_reg_reg(pCompiledCode, X86_ADD, SECONDARY_REG, PRIMARY_REG);
					x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 4, 4);
					x86_adjust_stack(pCompiledCode, -16);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 4, SECONDARY_REG, 4);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
				case ElementType_I8:
				case ElementType_U8:
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_adjust_stack(pCompiledCode, 8);
					x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 16, 4);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 4, PRIMARY_REG, 4);
					x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 20, 4);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					x86_mov_reg_membase(pCompiledCode, SECONDARY_REG, X86_ESP, 8, 4);
					x86_mul_reg(pCompiledCode, SECONDARY_REG, FALSE);
					x86_xchg_reg_reg(pCompiledCode, PRIMARY_REG, SECONDARY_REG, 4);
					x86_mul_membase(pCompiledCode, X86_ESP, 4, FALSE);
					x86_xchg_membase_reg(pCompiledCode, X86_ESP, 4, PRIMARY_REG, 4);
					x86_alu_reg_reg(pCompiledCode, X86_ADD, SECONDARY_REG, X86_EDX);
					x86_mul_membase(pCompiledCode, X86_ESP, 12, FALSE);
					x86_alu_reg_reg(pCompiledCode, X86_ADD, SECONDARY_REG, PRIMARY_REG);
					x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 4, 4);
					x86_adjust_stack(pCompiledCode, -16);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 4, SECONDARY_REG, 4);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid operand type!");
					break;
			}

			break;
		}
		case ElementType_R8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
				case ElementType_I8:
				case ElementType_U8:
					Panic("Invalid operand type!");
					break;
				case ElementType_R4:
				case ElementType_R8:
				{
					size_t argSize = 0;
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &argSize);
					if (argSize <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
						x86_adjust_stack(pCompiledCode, 4);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
						x86_adjust_stack(pCompiledCode, 8);
					}
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &argSize);
					if (argSize <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
						x86_adjust_stack(pCompiledCode, -4);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					}
					x86_fp_op(pCompiledCode, X86_FMUL, 1);
					x86_fst_membase(pCompiledCode, X86_ESP, 0, TRUE, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
			}
			break;
		}
		default:
			Panic("Invalid destination type!");
			break;
	}

EmitFinished:
	return pCompiledCode;
}

char* JIT_Emit_Div(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	switch((OverflowType)(uint32_t)pInstruction->Arg1)
	{
		case OverflowType_Signed:
		case OverflowType_Unsigned:
			Panic("Unsupported overflow type!");
			break;
		case OverflowType_None: break;
	}
	ElementType sAType = (ElementType)(uint32_t)pInstruction->Arg2;
	ElementType sBType = (ElementType)(uint32_t)pInstruction->Arg3;
	ElementType destType = (ElementType)0;
	switch(sAType)
	{
		case ElementType_I:
		case ElementType_U:
		case ElementType_I1:
		case ElementType_I2:
		case ElementType_I4:
		case ElementType_U1:
		case ElementType_U2:
		case ElementType_U4:
			switch(sBType)
			{
				case ElementType_I:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
					destType = ElementType_I4;
					break;
				case ElementType_U:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
					destType = ElementType_U4;
					break;
				case ElementType_I8:
					destType = ElementType_I8;
					break;
				case ElementType_U8:
					destType = ElementType_U8;
					break;
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid parameter!");
					break;
			}
			break;
		case ElementType_I8:
			destType = ElementType_I8;
			break;
		case ElementType_U8:
			if (sBType == ElementType_I8) destType = ElementType_I8;
			else destType = ElementType_U8;
			break;
		case ElementType_R4:
		case ElementType_R8:
			destType = ElementType_R8;
			break;
	}

	switch(destType)
	{
		case ElementType_I4:
		{
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, FOURTH_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			x86_div_reg(pCompiledCode, FOURTH_REG, TRUE);
			pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			break;
		}
		case ElementType_U4:
		{
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, FOURTH_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			if (PRIMARY_REG != X86_EAX) Panic("DIE A VERY PAINFUL DEATH EVIL DOEER!");
			x86_div_reg(pCompiledCode, FOURTH_REG, FALSE);
			pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			break;
		}
		case ElementType_I8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
					// This means that operand 2 is 64-bit.
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					x86_fp_int_op_membase(pCompiledCode, X86_FDIV, X86_ESP, 0, TRUE);
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I8:
				case ElementType_U8:
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					size_t arg2Size = 0;
					x86_adjust_stack(pCompiledCode, 8);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &arg2Size);
					if (arg2Size <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fp_int_op_membase(pCompiledCode, X86_FDIV, X86_ESP, 0, TRUE);
						x86_adjust_stack(pCompiledCode, -4);
					}
					else
					{
						x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
						x86_fp_op(pCompiledCode, X86_FDIV, 1);
					}
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid operand type!");
					break;
			}
			break;
		}
		case ElementType_U8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
				{
					// This means that operand 2 is 64-bit.
					x86_push_reg(pCompiledCode, DOMAIN_REG);

					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_adjust_stack(pCompiledCode, 8);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, FALSE);
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_call_code(pCompiledCode, __udivdi3);
					x86_adjust_stack(pCompiledCode, -8);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 4, X86_EDX, 4);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, X86_EAX, 4);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);

					x86_pop_reg(pCompiledCode, DOMAIN_REG);
					break;
				}
				case ElementType_I8:
				case ElementType_U8:
				{
					x86_push_reg(pCompiledCode, DOMAIN_REG);

					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_call_code(pCompiledCode, __udivdi3);
					x86_adjust_stack(pCompiledCode, -8);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 4, X86_EDX, 4);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, X86_EAX, 4);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);

					x86_pop_reg(pCompiledCode, DOMAIN_REG);
					break;
				}
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid operand type!");
					break;
			}

			break;
		}
		case ElementType_R8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
				case ElementType_I8:
				case ElementType_U8:
					Panic("Invalid operand type!");
					break;
				case ElementType_R4:
				case ElementType_R8:
				{
					size_t argSize = 0;
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &argSize);
					if (argSize <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
						x86_adjust_stack(pCompiledCode, 4);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
						x86_adjust_stack(pCompiledCode, 8);
					}
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &argSize);
					if (argSize <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
						x86_adjust_stack(pCompiledCode, -4);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					}
					x86_fp_op(pCompiledCode, X86_FDIV, 1);
					x86_fst_membase(pCompiledCode, X86_ESP, 0, TRUE, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
			}
			break;
		}
		default:
			Panic("Invalid destination type!");
			break;
	}

	return pCompiledCode;
}

char* JIT_Emit_Rem(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	switch((OverflowType)(uint32_t)pInstruction->Arg1)
	{
		case OverflowType_Signed:
		case OverflowType_Unsigned:
			Panic("Unsupported overflow type!");
			break;
		case OverflowType_None: break;
	}
	ElementType sAType = (ElementType)(uint32_t)pInstruction->Arg2;
	ElementType sBType = (ElementType)(uint32_t)pInstruction->Arg3;
	ElementType destType = (ElementType)0;
	switch(sAType)
	{
		case ElementType_I:
		case ElementType_U:
		case ElementType_I1:
		case ElementType_I2:
		case ElementType_I4:
		case ElementType_U1:
		case ElementType_U2:
		case ElementType_U4:
			switch(sBType)
			{
				case ElementType_I:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
					destType = ElementType_I4;
					break;
				case ElementType_U:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
					destType = ElementType_U4;
					break;
				case ElementType_I8:
					destType = ElementType_I8;
					break;
				case ElementType_U8:
					destType = ElementType_U8;
					break;
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid parameter!");
					break;
			}
			break;
		case ElementType_I8:
			destType = ElementType_I8;
			break;
		case ElementType_U8:
			if (sBType == ElementType_I8) destType = ElementType_I8;
			else destType = ElementType_U8;
			break;
		case ElementType_R4:
		case ElementType_R8:
			destType = ElementType_R8;
			break;
	}

	switch(destType)
	{
		case ElementType_I4:
		{
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, FOURTH_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			x86_div_reg(pCompiledCode, FOURTH_REG, TRUE);
			pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, X86_EDX, SECONDARY_REG, THIRD_REG, NULL);
			break;
		}
		case ElementType_U4:
		{
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, FOURTH_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			if (PRIMARY_REG != X86_EAX) Panic("DIE A VERY PAINFUL DEATH EVIL DOEER!");
			x86_div_reg(pCompiledCode, FOURTH_REG, FALSE);
			pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, X86_EDX, SECONDARY_REG, THIRD_REG, NULL);
			break;
		}
		case ElementType_I8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
					// This means that operand 2 is 64-bit.
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_adjust_stack(pCompiledCode, 4);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);     
					x86_fild_membase(pCompiledCode, X86_ESP, 0, FALSE);
					x86_fprem1(pCompiledCode);
					x86_adjust_stack(pCompiledCode, -4);
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I8:
				case ElementType_U8:
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					x86_fprem1(pCompiledCode);
					x86_adjust_stack(pCompiledCode, -8);
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid operand type!");
					break;
			}
			break;
		}
		case ElementType_U8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
				{
					x86_push_reg(pCompiledCode, DOMAIN_REG);

					// This means that operand 2 is 64-bit.
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_adjust_stack(pCompiledCode, 8);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, FALSE);
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_call_code(pCompiledCode, __umoddi3);
					x86_adjust_stack(pCompiledCode, -8);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 4, X86_EDX, 4);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, X86_EAX, 4);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);

					x86_pop_reg(pCompiledCode, DOMAIN_REG);
					break;
				}
				case ElementType_I8:
				case ElementType_U8:
				{
					x86_push_reg(pCompiledCode, DOMAIN_REG);

					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_call_code(pCompiledCode, __umoddi3);
					x86_adjust_stack(pCompiledCode, -8);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 4, X86_EDX, 4);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, X86_EAX, 4);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);

					x86_pop_reg(pCompiledCode, DOMAIN_REG);
					break;
				}
				case ElementType_R4:
				case ElementType_R8:
					Panic("Invalid operand type!");
					break;
			}

			break;
		}
		case ElementType_R8:
		{
			switch(sAType)
			{
				case ElementType_I:
				case ElementType_U:
				case ElementType_I1:
				case ElementType_I2:
				case ElementType_I4:
				case ElementType_U1:
				case ElementType_U2:
				case ElementType_U4:
				case ElementType_I8:
				case ElementType_U8:
					Panic("Invalid operand type!");
					break;
				case ElementType_R4:
				case ElementType_R8:
				{
					size_t argSize = 0;
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &argSize);
					if (argSize <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
						x86_adjust_stack(pCompiledCode, 4);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
						x86_adjust_stack(pCompiledCode, 8);
					}
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &argSize);
					if (argSize <= 4)
					{
						x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
						x86_adjust_stack(pCompiledCode, -4);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					}
					x86_fprem1(pCompiledCode);
					x86_fst_membase(pCompiledCode, X86_ESP, 0, TRUE, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
			}
			break;
		}
		default:
			Panic("Invalid destination type!");
			break;
	}

	return pCompiledCode;
}

char* JIT_Emit_And(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	if (SourceTypeData_Equal(pInstruction->Source1, pInstruction->Destination))
	{
		if (pInstruction->Source2.Type == SourceType_ConstantI4)
		{
			if (pInstruction->Source1.Type == SourceType_Local)
			{
				x86_alu_membase_imm(pCompiledCode, X86_AND, X86_EBP, -pMethod->LocalVariables[pInstruction->Source1.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source2.Data.ConstantI4.Value);
			}
			else if (pInstruction->Source1.Type == SourceType_Parameter)
			{
				x86_alu_membase_imm(pCompiledCode, X86_AND, X86_EBP, pMethod->Parameters[pInstruction->Source1.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source2.Data.ConstantI4.Value);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_reg_imm(pCompiledCode, X86_AND, PRIMARY_REG, pInstruction->Source2.Data.ConstantI4.Value);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			goto EmitFinished;
		}
	}
	else if (SourceTypeData_Equal(pInstruction->Source2, pInstruction->Destination))
	{
		if (pInstruction->Source1.Type == SourceType_ConstantI4)
		{
			if (pInstruction->Source2.Type == SourceType_Local)
			{
				x86_alu_membase_imm(pCompiledCode, X86_AND, X86_EBP, -pMethod->LocalVariables[pInstruction->Source2.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
			}
			else if (pInstruction->Source2.Type == SourceType_Parameter)
			{
				x86_alu_membase_imm(pCompiledCode, X86_AND, X86_EBP, pMethod->Parameters[pInstruction->Source2.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_reg_imm(pCompiledCode, X86_AND, PRIMARY_REG, pInstruction->Source1.Data.ConstantI4.Value);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			goto EmitFinished;
		}
	}

	uint32_t aAS = 0;
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, (size_t*)&aAS);
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, FOURTH_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	switch(aAS)
	{
		case 4:
			x86_alu_reg_reg(pCompiledCode, X86_AND, PRIMARY_REG, FOURTH_REG);
			break;
		case 8:
			x86_mov_reg_membase(pCompiledCode, SECONDARY_REG, X86_ESP, 0, 4);
			x86_alu_membase_reg(pCompiledCode, X86_AND, X86_ESP, 8, SECONDARY_REG);
			x86_mov_reg_membase(pCompiledCode, SECONDARY_REG, X86_ESP, 4, 4);
			x86_alu_membase_reg(pCompiledCode, X86_AND, X86_ESP, 12, SECONDARY_REG);
			x86_adjust_stack(pCompiledCode, -8);
			break;
	}
	pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
EmitFinished:
	return pCompiledCode;
}

char* JIT_Emit_Or(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	if (SourceTypeData_Equal(pInstruction->Source1, pInstruction->Destination))
	{
		if (pInstruction->Source2.Type == SourceType_ConstantI4)
		{
			if (pInstruction->Source1.Type == SourceType_Local)
			{
				x86_alu_membase_imm(pCompiledCode, X86_OR, X86_EBP, -pMethod->LocalVariables[pInstruction->Source1.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source2.Data.ConstantI4.Value);
			}
			else if (pInstruction->Source1.Type == SourceType_Parameter)
			{
				x86_alu_membase_imm(pCompiledCode, X86_OR, X86_EBP, pMethod->Parameters[pInstruction->Source1.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source2.Data.ConstantI4.Value);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_reg_imm(pCompiledCode, X86_OR, PRIMARY_REG, pInstruction->Source2.Data.ConstantI4.Value);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			goto EmitFinished;
		}
	}
	else if (SourceTypeData_Equal(pInstruction->Source2, pInstruction->Destination))
	{
		if (pInstruction->Source1.Type == SourceType_ConstantI4)
		{
			if (pInstruction->Source2.Type == SourceType_Local)
			{
				x86_alu_membase_imm(pCompiledCode, X86_OR, X86_EBP, -pMethod->LocalVariables[pInstruction->Source2.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
			}
			else if (pInstruction->Source2.Type == SourceType_Parameter)
			{
				x86_alu_membase_imm(pCompiledCode, X86_OR, X86_EBP, pMethod->Parameters[pInstruction->Source2.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_reg_imm(pCompiledCode, X86_OR, PRIMARY_REG, pInstruction->Source1.Data.ConstantI4.Value);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			goto EmitFinished;
		}
	}

	uint32_t aAS = 0;
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, (size_t*)&aAS);
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, FOURTH_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	switch(aAS)
	{
		case 4:
			x86_alu_reg_reg(pCompiledCode, X86_OR, PRIMARY_REG, FOURTH_REG);
			break;
		case 8:
			x86_mov_reg_membase(pCompiledCode, SECONDARY_REG, X86_ESP, 0, 4);
			x86_alu_membase_reg(pCompiledCode, X86_OR, X86_ESP, 8, SECONDARY_REG);
			x86_mov_reg_membase(pCompiledCode, SECONDARY_REG, X86_ESP, 4, 4);
			x86_alu_membase_reg(pCompiledCode, X86_OR, X86_ESP, 12, SECONDARY_REG);
			x86_adjust_stack(pCompiledCode, -8);
			break;
	}
	pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
EmitFinished:
	return pCompiledCode;
}

char* JIT_Emit_Xor(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	if (SourceTypeData_Equal(pInstruction->Source1, pInstruction->Destination))
	{
		if (pInstruction->Source2.Type == SourceType_ConstantI4)
		{
			if (pInstruction->Source1.Type == SourceType_Local)
			{
				x86_alu_membase_imm(pCompiledCode, X86_XOR, X86_EBP, -pMethod->LocalVariables[pInstruction->Source1.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source2.Data.ConstantI4.Value);
			}
			else if (pInstruction->Source1.Type == SourceType_Parameter)
			{
				x86_alu_membase_imm(pCompiledCode, X86_XOR, X86_EBP, pMethod->Parameters[pInstruction->Source1.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source2.Data.ConstantI4.Value);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_reg_imm(pCompiledCode, X86_XOR, PRIMARY_REG, pInstruction->Source2.Data.ConstantI4.Value);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			goto EmitFinished;
		}
	}
	else if (SourceTypeData_Equal(pInstruction->Source2, pInstruction->Destination))
	{
		if (pInstruction->Source1.Type == SourceType_ConstantI4)
		{
			if (pInstruction->Source2.Type == SourceType_Local)
			{
				x86_alu_membase_imm(pCompiledCode, X86_XOR, X86_EBP, -pMethod->LocalVariables[pInstruction->Source2.Data.LocalVariable.LocalVariableIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
			}
			else if (pInstruction->Source2.Type == SourceType_Parameter)
			{
				x86_alu_membase_imm(pCompiledCode, X86_XOR, X86_EBP, pMethod->Parameters[pInstruction->Source2.Data.Parameter.ParameterIndex]->Offset, pInstruction->Source1.Data.ConstantI4.Value);
			}
			else
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_alu_reg_imm(pCompiledCode, X86_XOR, PRIMARY_REG, pInstruction->Source1.Data.ConstantI4.Value);
				pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
			}
			goto EmitFinished;
		}
	}

	uint32_t aAS = 0;
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, (size_t*)&aAS);
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, FOURTH_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	switch(aAS)
	{
		case 4:
			x86_alu_reg_reg(pCompiledCode, X86_XOR, PRIMARY_REG, FOURTH_REG);
			break;
		case 8:
			x86_mov_reg_membase(pCompiledCode, SECONDARY_REG, X86_ESP, 0, 4);
			x86_alu_membase_reg(pCompiledCode, X86_XOR, X86_ESP, 8, SECONDARY_REG);
			x86_mov_reg_membase(pCompiledCode, SECONDARY_REG, X86_ESP, 4, 4);
			x86_alu_membase_reg(pCompiledCode, X86_XOR, X86_ESP, 12, SECONDARY_REG);
			x86_adjust_stack(pCompiledCode, -8);
			break;
	}
	pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
EmitFinished:
	return pCompiledCode;
}

char* JIT_Emit_Neg(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	if (SourceTypeData_Equal(pInstruction->Source1, pInstruction->Destination))
	{
		if (pInstruction->Source1.Type == SourceType_Local)
		{
			IRLocalVariable* local = pMethod->LocalVariables[pInstruction->Source1.Data.LocalVariable.LocalVariableIndex];
			switch(local->Size)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					x86_neg_membase(pCompiledCode, X86_EBP, -local->Offset);
					break;
				case 5:
				case 6:
				case 7:
				case 8:
					switch((ElementType)(int)pInstruction->Arg1)
					{
						case ElementType_I8:
						case ElementType_U8:
							x86_fild_membase(pCompiledCode, X86_EBP, -local->Offset, TRUE);
							x86_fchs(pCompiledCode);
							x86_fist_pop_membase(pCompiledCode, X86_EBP, -local->Offset, TRUE);
							break;
						case ElementType_R4:
							x86_fld_membase(pCompiledCode, X86_EBP, -local->Offset, FALSE);
							x86_fchs(pCompiledCode);
							x86_fst_membase(pCompiledCode, X86_EBP, -local->Offset, FALSE, TRUE);
							break;
						case ElementType_R8:
							x86_fld_membase(pCompiledCode, X86_EBP, -local->Offset, TRUE);
							x86_fchs(pCompiledCode);
							x86_fst_membase(pCompiledCode, X86_EBP, -local->Offset, TRUE, TRUE);
							break;
						default:
							Panic("Invalid large operand for Negate!");
							break;
					}
					break;
			}
			goto EmitFinished;
		}
		else if (pInstruction->Source1.Type == SourceType_Parameter)
		{
			IRParameter* param = pMethod->Parameters[pInstruction->Source1.Data.Parameter.ParameterIndex];
			switch(param->Size)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					x86_neg_membase(pCompiledCode, X86_EBP, param->Offset);
					break;
				case 5:
				case 6:
				case 7:
				case 8:
					switch((ElementType)(int)pInstruction->Arg1)
					{
						case ElementType_I8:
						case ElementType_U8:
							x86_fild_membase(pCompiledCode, X86_EBP, param->Offset, TRUE);
							x86_fchs(pCompiledCode);
							x86_fist_pop_membase(pCompiledCode, X86_EBP, param->Offset, TRUE);
							break;
						case ElementType_R4:
							x86_fld_membase(pCompiledCode, X86_EBP, param->Offset, FALSE);
							x86_fchs(pCompiledCode);
							x86_fst_membase(pCompiledCode, X86_EBP, param->Offset, FALSE, TRUE);
							break;
						case ElementType_R8:
							x86_fld_membase(pCompiledCode, X86_EBP, param->Offset, TRUE);
							x86_fchs(pCompiledCode);
							x86_fst_membase(pCompiledCode, X86_EBP, param->Offset, TRUE, TRUE);
							break;
						default:
							Panic("Invalid large operand for Negate!");
							break;
					}
					break;
			}
			goto EmitFinished;
		}
	}
	
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	switch((ElementType)(int)pInstruction->Arg1)
	{
		case ElementType_I:
		case ElementType_I1:
		case ElementType_I2:
		case ElementType_I4:
		case ElementType_U:
		case ElementType_U1:
		case ElementType_U2:
		case ElementType_U4:
			x86_neg_reg(pCompiledCode, PRIMARY_REG);
			break;
		case ElementType_I8:
		case ElementType_U8:
			x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
			x86_fchs(pCompiledCode);
			x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
			break;
		case ElementType_R4:
			x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
			x86_fchs(pCompiledCode);
			x86_fst_membase(pCompiledCode, X86_ESP, 0, FALSE, TRUE);
			break;
		case ElementType_R8:
			x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
			x86_fchs(pCompiledCode);
			x86_fst_membase(pCompiledCode, X86_ESP, 0, TRUE, TRUE);
			break;
	}
	pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
EmitFinished:
	return pCompiledCode;
}

char* JIT_Emit_Not(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	if (SourceTypeData_Equal(pInstruction->Source1, pInstruction->Destination))
	{
		if (pInstruction->Source1.Type == SourceType_Local)
		{
			IRLocalVariable* local = pMethod->LocalVariables[pInstruction->Source1.Data.LocalVariable.LocalVariableIndex];
			switch(local->Size)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					x86_not_membase(pCompiledCode, X86_EBP, -local->Offset);
					break;
				case 5:
				case 6:
				case 7:
				case 8:
					x86_not_membase(pCompiledCode, X86_EBP, -local->Offset);
					x86_not_membase(pCompiledCode, X86_EBP, -local->Offset + 4);
					break;
			}
			goto EmitFinished;
		}
		else if (pInstruction->Source1.Type == SourceType_Parameter)
		{
			IRParameter* param = pMethod->Parameters[pInstruction->Source1.Data.Parameter.ParameterIndex];
			switch(param->Size)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					x86_not_membase(pCompiledCode, X86_EBP, param->Offset);
					break;
				case 5:
				case 6:
				case 7:
				case 8:
					x86_not_membase(pCompiledCode, X86_EBP, param->Offset);
					x86_not_membase(pCompiledCode, X86_EBP, param->Offset + 4);
					break;
			}
			goto EmitFinished;
		}
	}
	
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	switch((ElementType)(int)pInstruction->Arg1)
	{
		case ElementType_I:
		case ElementType_I1:
		case ElementType_I2:
		case ElementType_I4:
		case ElementType_U:
		case ElementType_U1:
		case ElementType_U2:
		case ElementType_U4:
			x86_not_reg(pCompiledCode, PRIMARY_REG);
			break;
		case ElementType_I8:
		case ElementType_U8:
			x86_not_membase(pCompiledCode, X86_ESP, 0);
			x86_not_membase(pCompiledCode, X86_ESP, 4);
			break;
		default:
			Panic("Invalid type for Not instruction!");
			break;
	}
	pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
EmitFinished:
	return pCompiledCode;
}

char* JIT_Emit_Shift(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	//some code written
	ShiftNumericOperation sType = (ShiftNumericOperation)(uint32_t)pInstruction->Arg1;
	ElementType sValType;
	ElementType sbValType;
	switch ((ElementType)(uint32_t)pInstruction->Arg3)
	{
		case ElementType_I1:
		case ElementType_U1:
		case ElementType_I2:
		case ElementType_U2:
		case ElementType_I4:
		case ElementType_U4:
		case ElementType_I:
		case ElementType_U:
			sValType = ElementType_I4;
			break;
			
		case ElementType_I8:
		case ElementType_U8:
			sValType = ElementType_I8;
			break;

		case ElementType_R4:
		case ElementType_R8:
			Panic("DIE EVIL FLOAT SHIFTER!");
			break;
	}
	switch ((ElementType)(uint32_t)pInstruction->Arg2)
	{
		case ElementType_I4:
		case ElementType_U4:
		case ElementType_I:
		case ElementType_U:
			sbValType = ElementType_I4;
			break;
		case ElementType_I1:
		case ElementType_I2:
		case ElementType_U1:
		case ElementType_U2:
		case ElementType_I8:
		case ElementType_U8:
		case ElementType_R4:
		case ElementType_R8:
			Panic("WTF DO YOU THINK YOUR DOING?????");
			break;
	}
	switch(sValType)
	{
		case ElementType_I4:
		{
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			switch(sbValType)
			{
				case ElementType_I4:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, NULL);
					switch(sType)
					{
						case ShiftNumericOperation_Left:
							x86_alu_reg_reg(pCompiledCode, X86_SHL, PRIMARY_REG, SECONDARY_REG);
							break;
						case ShiftNumericOperation_Right:
							x86_alu_reg_reg(pCompiledCode, X86_SHR, PRIMARY_REG, SECONDARY_REG);
							break;
						case ShiftNumericOperation_Right_Sign_Extended:
							x86_alu_reg_reg(pCompiledCode, X86_SAR, PRIMARY_REG, SECONDARY_REG);
							break;
					}
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				default:
					Panic("Why must you be a pain?");
					break;
			}
			break;
		}
		case ElementType_I8:
		{
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			switch (sbValType)
			{
				case ElementType_I4:
				{
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, X86_ECX, PRIMARY_REG, SECONDARY_REG, FALSE, 0, NULL);
					x86_mov_reg_membase(pCompiledCode, X86_EBX, X86_ESP, 4, 4);
					x86_test_reg_imm(pCompiledCode, X86_ECX, 32);
					unsigned char* jumpHighPartIsZero = (unsigned char *)pCompiledCode;
					x86_branch8(pCompiledCode, X86_CC_AE, 0, FALSE);
					// 32bit or less shift
					x86_mov_reg_membase(pCompiledCode, X86_EAX, X86_ESP, 0, 4);
					switch (sType)
					{
						case ShiftNumericOperation_Left:
						{
							x86_shld_reg(pCompiledCode, X86_EAX, X86_EAX);
							break;
						}
						case ShiftNumericOperation_Right:
						case ShiftNumericOperation_Right_Sign_Extended:
						{
							x86_shrd_reg(pCompiledCode, X86_EAX, X86_EAX);
							break;
						}
					}
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, X86_EAX, 4);
					switch (sType)
					{
						case ShiftNumericOperation_Left:
							x86_shift_reg(pCompiledCode, X86_SHL, X86_EBX);
							break;
						case ShiftNumericOperation_Right:
							x86_shift_reg(pCompiledCode, X86_SHR, X86_EBX);
							break;
						case ShiftNumericOperation_Right_Sign_Extended:
							x86_shift_reg(pCompiledCode, X86_SAR, X86_EBX);
							break;
					}
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 4, X86_EBX, 4);
					unsigned char* jumpEnd = (unsigned char *)pCompiledCode;
					x86_jump8(pCompiledCode, 0);
					x86_patch(jumpHighPartIsZero, (unsigned char *)pCompiledCode);
					// 33-64bit shift
					x86_alu_reg_imm(pCompiledCode, X86_AND, X86_ECX, 0x1F);
					switch (sType)
					{
						case ShiftNumericOperation_Left:
							x86_shift_reg(pCompiledCode, X86_SHL, X86_EBX);
							break;
						case ShiftNumericOperation_Right:
							x86_shift_reg(pCompiledCode, X86_SHR, X86_EBX);
							break;
						case ShiftNumericOperation_Right_Sign_Extended:
							x86_shift_reg(pCompiledCode, X86_SAR, X86_EBX);
							break;
					}
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 4, X86_EBX, 4);
					x86_mov_membase_imm(pCompiledCode, X86_ESP, 0, 0, 4);
					x86_patch(jumpEnd, (unsigned char *)pCompiledCode);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				}
				default:
					Panic("bad");
					break;
			}
		}
		default:
			Panic("moar bad");
			break;
	}
	return pCompiledCode;
}

char* JIT_Emit_Convert_Unchecked(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	ElementType fromType = (ElementType)(uint32_t)pInstruction->Arg1;
	ElementType toType = (ElementType)(uint32_t)pInstruction->Arg2;
	//bool_t IsDouble = TRUE;
	switch (fromType)
	{
		case ElementType_I:
		case ElementType_U:
		case ElementType_I4:
		case ElementType_I1:
		case ElementType_I2:
		case ElementType_U1:
		case ElementType_U2:
		case ElementType_U4:
			switch (toType)
			{
				case ElementType_I1:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_widen_reg(pCompiledCode, PRIMARY_REG, PRIMARY_REG, TRUE, FALSE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_U1:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_widen_reg(pCompiledCode, PRIMARY_REG, PRIMARY_REG, FALSE, FALSE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I2:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_widen_reg(pCompiledCode, PRIMARY_REG, PRIMARY_REG, TRUE, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_U2:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_widen_reg(pCompiledCode, PRIMARY_REG, PRIMARY_REG, FALSE, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I:
				case ElementType_I4:
				case ElementType_U:
				case ElementType_U4:
					pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source1, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I8:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, X86_EAX, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_cdq(pCompiledCode);
					x86_push_reg(pCompiledCode, X86_EDX);
					x86_push_reg(pCompiledCode, X86_EAX);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_U8:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_push_imm(pCompiledCode, 0);
					x86_push_reg(pCompiledCode, PRIMARY_REG);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_R4:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_push_reg(pCompiledCode, PRIMARY_REG);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, FALSE);
					x86_fst_membase(pCompiledCode, X86_ESP, 0, FALSE, TRUE);
					x86_pop_reg(pCompiledCode, PRIMARY_REG);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_R8:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_adjust_stack(pCompiledCode, 8);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, FALSE);
					x86_fst_membase(pCompiledCode, X86_ESP, 0, TRUE, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				default:
					//Panic(String_Format("Convert Unchecked (Branch 1), Invalid Arguments: From = 0x%x, To = 0x%x", (unsigned int)fromType, (unsigned int)toType));
					Panic("Convert Unchecked (Branch 1), Invalid Arguments");
					break;
			}
			break;
		case ElementType_I8:
		case ElementType_U8:
			switch (toType)
			{
				case ElementType_I1:
				case ElementType_U1:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 0, 4);
					x86_adjust_stack(pCompiledCode, -8);
					x86_alu_reg_imm(pCompiledCode, X86_AND, PRIMARY_REG, 0xFF);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I2:
				case ElementType_U2:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 0, 4);
					x86_adjust_stack(pCompiledCode, -8);
					x86_alu_reg_imm(pCompiledCode, X86_AND, PRIMARY_REG, 0xFFFF);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I:
				case ElementType_I4:
				case ElementType_U:
				case ElementType_U4:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 0, 4);
					x86_adjust_stack(pCompiledCode, -8);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I8:
				case ElementType_U8: 
					break;
				case ElementType_R4:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					x86_fst_membase(pCompiledCode, X86_ESP, 0, FALSE, TRUE);
					x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 0, 4);
					x86_adjust_stack(pCompiledCode, -8);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_R8:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
					x86_fst_membase(pCompiledCode, X86_ESP, 0, TRUE, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				default:
					{
						//snprintf(buf, 128, "Convert Unchecked (Branch 2), Invalid Arguments: From = 0x%x, To = 0x%x", (unsigned int)fromType, (unsigned int)toType);
						Panic("Convert Unchecked (Branch 2), Invalid Arguments");
					}
					break;
			}
			break;
		case ElementType_R4:
		case ElementType_R8:
			switch (toType)
			{
				case ElementType_I1:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					if (fromType == ElementType_R4)
					{
						x86_push_reg(pCompiledCode, PRIMARY_REG);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					}
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					if (fromType == ElementType_R4)
					{
						x86_pop_reg(pCompiledCode, PRIMARY_REG);
					}
					else
					{
						x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 0, 4);
						x86_adjust_stack(pCompiledCode, -8);
					}
					x86_widen_reg(pCompiledCode, PRIMARY_REG, PRIMARY_REG, TRUE, FALSE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_U1:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					if (fromType == ElementType_R4)
					{
						x86_push_reg(pCompiledCode, PRIMARY_REG);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					}
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					if (fromType == ElementType_R4)
					{
						x86_pop_reg(pCompiledCode, PRIMARY_REG);
					}
					else
					{
						x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 0, 4);
						x86_adjust_stack(pCompiledCode, -8);
					}
					x86_widen_reg(pCompiledCode, PRIMARY_REG, PRIMARY_REG, FALSE, FALSE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I2:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					if (fromType == ElementType_R4)
					{
						x86_push_reg(pCompiledCode, PRIMARY_REG);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					}
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					if (fromType == ElementType_R4)
					{
						x86_pop_reg(pCompiledCode, PRIMARY_REG);
					}
					else
					{
						x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 0, 4);
						x86_adjust_stack(pCompiledCode, -8);
					}
					x86_widen_reg(pCompiledCode, PRIMARY_REG, PRIMARY_REG, TRUE, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_U2:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					if (fromType == ElementType_R4)
					{
						x86_push_reg(pCompiledCode, PRIMARY_REG);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					}
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					if (fromType == ElementType_R4)
					{
						x86_pop_reg(pCompiledCode, PRIMARY_REG);
					}
					else
					{
						x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 0, 4);
						x86_adjust_stack(pCompiledCode, -8);
					}
					x86_widen_reg(pCompiledCode, PRIMARY_REG, PRIMARY_REG, FALSE, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I:
				case ElementType_I4:
				case ElementType_U:
				case ElementType_U4:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					if (fromType == ElementType_R4)
					{
						x86_push_reg(pCompiledCode, PRIMARY_REG);
						x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
					}
					else
					{
						x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					}
					x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					if (fromType == ElementType_R4)
					{
						x86_pop_reg(pCompiledCode, PRIMARY_REG);
					}
					else
					{
						x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 0, 4);
						x86_adjust_stack(pCompiledCode, -8);
					}
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_I8:
				case ElementType_U8:
					Panic("Your a bad bad person!");
					//x86_alu_reg_imm(pCompiledCode, X86_SUB, X86_ESP, 4);
					//x86_fnstcw_membase(pCompiledCode, X86_ESP, 0);
					//x86_mov_reg_membase(pCompiledCode, X86_EAX, X86_ESP, 0, 2);
					//x86_alu_reg_imm(pCompiledCode, X86_OR, X86_EAX, 0x0C00);
					//x86_mov_membase_reg(pCompiledCode, X86_ESP, 2, X86_EAX, 2);
					//x86_fldcw_membase(pCompiledCode, X86_ESP, 2);
					//x86_alu_reg_imm(pCompiledCode, X86_SUB, X86_ESP, 8);
					//x86_fist_pop_membase(pCompiledCode, X86_ESP, 0, TRUE);
					//x86_pop_reg(pCompiledCode, X86_EAX);
					//x86_pop_reg(pCompiledCode, X86_EBX);
					//x86_fldcw_membase(pCompiledCode, X86_ESP, 0);
					//x86_alu_reg_imm(pCompiledCode, X86_ADD, X86_ESP, 4);
					//x86_push_reg(pCompiledCode, X86_EBX);
					//x86_push_reg(pCompiledCode, X86_EAX);
					break;
				case ElementType_R4:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					x86_fst_membase(pCompiledCode, X86_ESP, 0, FALSE, TRUE);
					x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 0, 4);
					x86_adjust_stack(pCompiledCode, -8);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				case ElementType_R8:
					pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
					x86_adjust_stack(pCompiledCode, 8);
					x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, 4);
					x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
					x86_fst_membase(pCompiledCode, X86_ESP, 0, TRUE, TRUE);
					pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
					break;
				default:
					{
						//snprintf(buf, 128, "Convert Unchecked (Branch 3), Invalid Arguments: From = 0x%x, To = 0x%x", (unsigned int)fromType, (unsigned int)toType);
						Panic("Convert Unchecked (Branch 3), Invalid Arguments");
					}
					break;
			}
			break;
		default:
			{
				//snprintf(buf, 128, "Convert Unchecked (Branch 4), Invalid Arguments: From = 0x%x, To = 0x%x", (unsigned int)fromType, (unsigned int)toType);
				Panic("Convert Unchecked (Branch 4), Invalid Arguments");
			}
			break;
	}
	return pCompiledCode;
}

char* JIT_Emit_Convert_Checked(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	Panic("Checked conversions aren't supported yet");
	return pCompiledCode;
}

char* JIT_Emit_CastClass(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	Panic("CastClass, exceptions not yet supported");
	return pCompiledCode;
}

char* JIT_Emit_IsInst(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	x86_push_reg(pCompiledCode, DOMAIN_REG);

	// TODO: replace this trampoline, it's basically a tiny wrapper to call IRType_IsSubclassOf
	x86_adjust_stack(pCompiledCode, gSizeOfPointerInBytes << 1);
	x86_mov_membase_imm(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, (size_t)pInstruction->Arg1, gSizeOfPointerInBytes);
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, gSizeOfPointerInBytes);
	x86_call_code(pCompiledCode, GCObject_Internal_IsInst);
	x86_adjust_stack(pCompiledCode, -(gSizeOfPointerInBytes << 1));
	pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, X86_EAX, SECONDARY_REG, THIRD_REG, NULL);

	x86_pop_reg(pCompiledCode, DOMAIN_REG);
	return pCompiledCode;
}

char* JIT_Emit_Box(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	IRType* type = (IRType*)pInstruction->Arg1;
	if (type->IsReferenceType)
	{
		pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source1, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
		goto EmitFinished;
	}
	if (!type->IsValueType) Panic("Nullable<T> not yet supported");

	pCompiledCode = JIT_Emit_LoadDestinationAddress(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG);
	x86_adjust_stack(pCompiledCode, (gSizeOfPointerInBytes * 3) + 4);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, (gSizeOfPointerInBytes << 1) + 4, PRIMARY_REG, gSizeOfPointerInBytes);
	x86_mov_membase_imm(pCompiledCode, X86_ESP, gSizeOfPointerInBytes << 1, type->Size, 4);
	x86_mov_membase_imm(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, (size_t)type, gSizeOfPointerInBytes);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, DOMAIN_REG, gSizeOfPointerInBytes);
	x86_call_code(pCompiledCode, GC_AllocateObject);
	x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, (gSizeOfPointerInBytes << 1) + 4, gSizeOfPointerInBytes);
	x86_mov_reg_membase(pCompiledCode, DOMAIN_REG, X86_ESP, 0, gSizeOfPointerInBytes);
	x86_adjust_stack(pCompiledCode, -((gSizeOfPointerInBytes * 3) + 4));

	size_t sizeOfSource = 0;
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, &sizeOfSource);
	if (sizeOfSource <= gSizeOfPointerInBytes) x86_push_reg(pCompiledCode, SECONDARY_REG);
	size_t sizeOfDestination = type->Size;
	Define_Move_To_Destination(X86_ESP, 0, PRIMARY_REG, 0, SECONDARY_REG, TRUE, FALSE);
	x86_adjust_stack(pCompiledCode, -sizeOfSource);

EmitFinished:
	return pCompiledCode;
}

char* JIT_Emit_Unbox(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	IRType* type = (IRType*)pInstruction->Arg1;
	if (type->IsValueType || type->IsReferenceType)
	{
		pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source1, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
	}
	else Panic("Unbox Nullable<T> is not yet supported!");
	return pCompiledCode;
}

char* JIT_Emit_UnboxAny(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	IRType* type = (IRType*)pInstruction->Arg1;
	if (type->IsValueType)
	{
		pCompiledCode = JIT_Emit_LoadDestinationAddress(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG);
		pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, NULL);
		size_t sizeOfSource = type->Size;
		size_t sizeOfDestination = JIT_StackAlign(sizeOfSource);
		Define_Move_To_Destination(SECONDARY_REG, 0, PRIMARY_REG, 0, THIRD_REG, FALSE, TRUE);
	}
	else if(type->IsReferenceType)
	{
		pCompiledCode = JIT_Emit_Move(pCompiledCode, pMethod, &pInstruction->Source1, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
	}
	else Panic("UnboxAny Nullable<T> is not yet supported!");

	return pCompiledCode;
}

char* JIT_Emit_Throw(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	x86_push_imm(pCompiledCode, (uint32_t)"An Exception was thrown!");
	x86_call_code(pCompiledCode, Panic);
	return pCompiledCode;
}

char* JIT_Emit_Copy_Object(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	IRType* objectType = (IRType*)pInstruction->Arg2;

	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL); // Source ptr
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, NULL); // Destination ptr

	size_t sizeOfSource = objectType->Size;
	if(objectType->IsReferenceType) sizeOfSource = gSizeOfPointerInBytes;
	size_t sizeOfDestination = sizeOfSource;
	Define_Move_To_Destination(PRIMARY_REG, 0, SECONDARY_REG, 0, THIRD_REG, FALSE, FALSE);

	return pCompiledCode;
}

char* JIT_Emit_New_Array(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	IRType* arrayType = IRAssembly_MakeArrayType(pMethod->ParentAssembly, (IRType*)pInstruction->Arg1);
	if (!arrayType->ArrayType->ElementType->StackSizeCalculated) JIT_GetStackSizeOfType(arrayType->ArrayType->ElementType);

	pCompiledCode = JIT_Emit_LoadDestinationAddress(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG);
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, NULL);

	x86_adjust_stack(pCompiledCode, (gSizeOfPointerInBytes * 3) + 4);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, (gSizeOfPointerInBytes << 1) + 4, PRIMARY_REG, gSizeOfPointerInBytes);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, gSizeOfPointerInBytes << 1, SECONDARY_REG, 4);
	x86_mov_membase_imm(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, (size_t)arrayType, gSizeOfPointerInBytes);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, DOMAIN_REG, gSizeOfPointerInBytes);
	x86_call_code(pCompiledCode, GC_AllocateArray);
	x86_mov_reg_membase(pCompiledCode, DOMAIN_REG, X86_ESP, 0, gSizeOfPointerInBytes);
	x86_adjust_stack(pCompiledCode, -((gSizeOfPointerInBytes * 3) + 4));

	return pCompiledCode;
}

char* JIT_Emit_CheckFinite(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	x86_push_imm(pCompiledCode, (uint32_t)"CheckFinite is not yet supported!");
	x86_call_code(pCompiledCode, Panic);
	return pCompiledCode;
}

char* JIT_Emit_Allocate_Local(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	Panic("AllocateLocal emitter not yet implemented!");
	return pCompiledCode;
}

char* JIT_Emit_Initialize_Object(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	IRType* objectType = (IRType*)pInstruction->Arg2;
	uint32_t blockCount = objectType->Size >> gPointerDivideShift;
	uint32_t remainderCount = objectType->Size & (gSizeOfPointerInBytes - 1);
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	for (uint32_t index = 0; index < blockCount; ++index)
	{
		x86_mov_membase_imm(pCompiledCode, PRIMARY_REG, index * gSizeOfPointerInBytes, 0, gSizeOfPointerInBytes);
	}
	if (remainderCount)
	{
		x86_mov_membase_imm(pCompiledCode, PRIMARY_REG, blockCount * gSizeOfPointerInBytes, 0, remainderCount);
	}
	
	return pCompiledCode;
}

char* JIT_Emit_Copy_Block(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	x86_push_reg(pCompiledCode, DOMAIN_REG);

	x86_adjust_stack(pCompiledCode, gSizeOfPointerInBytes * 3);
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, gSizeOfPointerInBytes << 1, PRIMARY_REG, gSizeOfPointerInBytes);
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, PRIMARY_REG, gSizeOfPointerInBytes);
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source3, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, gSizeOfPointerInBytes);
	x86_call_code(pCompiledCode, memcpy);
	x86_adjust_stack(pCompiledCode, -(gSizeOfPointerInBytes * 3));

	x86_pop_reg(pCompiledCode, DOMAIN_REG);
	return pCompiledCode;
}

char* JIT_Emit_Initialize_Block(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	x86_push_reg(pCompiledCode, DOMAIN_REG);

	x86_adjust_stack(pCompiledCode, gSizeOfPointerInBytes * 3);
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, gSizeOfPointerInBytes << 1, PRIMARY_REG, gSizeOfPointerInBytes);
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, PRIMARY_REG, gSizeOfPointerInBytes);
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source3, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, PRIMARY_REG, gSizeOfPointerInBytes);
	x86_call_code(pCompiledCode, memset);
	x86_adjust_stack(pCompiledCode, -(gSizeOfPointerInBytes * 3));

	x86_pop_reg(pCompiledCode, DOMAIN_REG);
	return pCompiledCode;
}

char* JIT_Emit_Branch(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	BranchCondition condition = (BranchCondition)(uint32_t)pInstruction->Arg1;
	IRInstruction* target = (IRInstruction*)pInstruction->Arg2;
	if (condition == BranchCondition_Always)
	{
		unsigned char* comp = (unsigned char*)pCompiledCode;
		x86_jump_code(comp, 0);
		BranchRegistry_RegisterBranch(pBranchRegistry, pInstruction->ILLocation, target->ILLocation, pCompiledCode);
		pCompiledCode = (char*)comp;
	}
	else if(condition == BranchCondition_False || condition == BranchCondition_True)
	{
		ElementType ArgType = (ElementType)(int32_t)pInstruction->Arg3;
		bool_t validArg = FALSE;
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
				{
					validArg = TRUE;
					break;
				}
			default:
				{
					if ((int32_t)ArgType == -1)
					{
						validArg = TRUE;
						break;
					}
					char buf[256];
					snprintf(buf, 256, "Invalid argument type for simple branch (%d)!", (int)ArgType);
					Panic(buf);
					break;
				}
		}
		if (validArg)
		{
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			x86_test_reg_reg(pCompiledCode, PRIMARY_REG, PRIMARY_REG);

			unsigned char* comp = (unsigned char*)pCompiledCode;
			if (condition == BranchCondition_False)
			{
					x86_branch32(comp, X86_CC_Z, 0, FALSE);
			}
			else
			{
					x86_branch32(comp, X86_CC_NZ, 0, FALSE);
			}
			BranchRegistry_RegisterBranch(pBranchRegistry, pInstruction->ILLocation, target->ILLocation, pCompiledCode);
			pCompiledCode = (char*)comp;
		}
	}
	else
	{
		ElementType Arg1Type = (ElementType)(int32_t)pInstruction->Arg3;
		//ElementType Arg2Type = *(ElementType*)instr->Arg4;
		//if (Arg1Type != Arg2Type)
		//	Panic("Something went very wrong in the decomp, because this shouldn't be possible.");

		switch(Arg1Type) // Here we get to load the arguments and call cmp on them.
		{
			case ElementType_I:
			case ElementType_U:
			case ElementType_I4:
			case ElementType_U4:
			case ElementType_I2:
			case ElementType_I1:
			case ElementType_U2:
			case ElementType_U1:
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, NULL);
				x86_alu_reg_reg(pCompiledCode, X86_CMP, SECONDARY_REG, PRIMARY_REG);
				break;
			}
			case ElementType_I8:
			case ElementType_U8:
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
				x86_fcomip(pCompiledCode, 1);
				x86_fdecstp(pCompiledCode);
				x86_adjust_stack(pCompiledCode, -16);
				break;
			}
			case ElementType_R4:
			case ElementType_R8:
			{
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				if (Arg1Type == ElementType_R4)
				{
					x86_push_reg(pCompiledCode, PRIMARY_REG);
					x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
					x86_adjust_stack(pCompiledCode, -4);
				}
				else
				{
					x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					x86_adjust_stack(pCompiledCode, -8);
				}
				pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
				if (Arg1Type == ElementType_R4)
				{
					x86_push_reg(pCompiledCode, PRIMARY_REG);
					x86_fld_membase(pCompiledCode, X86_ESP, 0, FALSE);
					x86_adjust_stack(pCompiledCode, -4);
				}
				else
				{
					x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
					x86_adjust_stack(pCompiledCode, -8);
				}
				x86_fcomip(pCompiledCode, 1);
				x86_fdecstp(pCompiledCode);
				break;
			}

			default:
				Panic("Invalid type for non-simple branch comparison.");
				break;
		}
		unsigned char* comp = (unsigned char*)pCompiledCode;
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
		BranchRegistry_RegisterBranch(pBranchRegistry, pInstruction->ILLocation, target->ILLocation, pCompiledCode);
		pCompiledCode = (char*)comp;
	}
	return pCompiledCode;
}

char* JIT_Emit_Switch(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	uint32_t targetCount = (uint32_t)pInstruction->Arg1;
	IRInstruction** targets = (IRInstruction**)pInstruction->Arg2;

	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, X86_EAX, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	x86_alu_reg_imm(pCompiledCode, X86_CMP, X86_EAX, targetCount);
	unsigned char* jumpToDefault = (unsigned char*)pCompiledCode;
	x86_branch32(pCompiledCode, X86_CC_AE, 0, FALSE);

	unsigned char* skipTable = (unsigned char*)pCompiledCode;
	x86_jump32(pCompiledCode, 0);
	uint32_t switchTableLocation = (uint32_t)pCompiledCode;
	for (uint32_t index = 0; index < targetCount; ++index)
	{
		BranchRegistry_RegisterSpecialBranch(pBranchRegistry, pInstruction->ILLocation + (index << 2), targets[index]->ILLocation, pCompiledCode);
		x86_imm_emit32(pCompiledCode, targets[index]->ILLocation);
	}
	x86_patch(skipTable, (unsigned char*)pCompiledCode);
	x86_jump_memindex(pCompiledCode, X86_NOBASEREG, switchTableLocation, PRIMARY_REG, 2);
	// After this is the default case.
	x86_patch(jumpToDefault, (unsigned char*)pCompiledCode);
	return pCompiledCode;
}

char* JIT_Emit_Load_Function(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	IRMethod* method = (IRMethod*)pInstruction->Arg1;
	if (!method->AssembledMethod) JIT_CompileMethod(method);
	x86_mov_reg_imm(pCompiledCode, PRIMARY_REG, method->AssembledMethod);
	pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
	return pCompiledCode;
}

char* JIT_Emit_Load_VirtualFunction(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	x86_push_reg(pCompiledCode, DOMAIN_REG);

	uint32_t functionIndex = (uint32_t)pInstruction->Arg2;
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	x86_alu_reg_imm(pCompiledCode, X86_SUB, PRIMARY_REG, gSizeOfPointerInBytes);
	x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, PRIMARY_REG, 0, gSizeOfPointerInBytes); // Contains GCObject*
	x86_alu_reg_imm(pCompiledCode, X86_ADD, PRIMARY_REG, offsetof(GCObject, Type));
	x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, PRIMARY_REG, 0, gSizeOfPointerInBytes); // Contains GCObject->Type*
	x86_alu_reg_imm(pCompiledCode, X86_ADD, PRIMARY_REG, offsetof(IRType, Methods));
	x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, PRIMARY_REG, 0, gSizeOfPointerInBytes); // Contains GCObject->Type->Methods**
	x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, PRIMARY_REG, 0, gSizeOfPointerInBytes); // Contains GCObject->Type->Methods*
	x86_alu_reg_imm(pCompiledCode, X86_ADD, PRIMARY_REG, functionIndex * gSizeOfPointerInBytes);
	x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, PRIMARY_REG, 0, gSizeOfPointerInBytes); // Contains virtual IRMethod*

	x86_push_reg(pCompiledCode, PRIMARY_REG);
	x86_call_code(pCompiledCode, JIT_CompileMethod);
	x86_pop_reg(pCompiledCode, PRIMARY_REG);

	x86_alu_reg_imm(pCompiledCode, X86_ADD, PRIMARY_REG, offsetof(IRMethod, AssembledMethod));
	x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, PRIMARY_REG, 0, gSizeOfPointerInBytes);
	pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);

	x86_pop_reg(pCompiledCode, DOMAIN_REG);
	return pCompiledCode;
}

char* JIT_Emit_Compare(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	CompareCondition cond = (CompareCondition)(uint32_t)pInstruction->Arg1;
	ElementType et1 = (ElementType)(uint32_t)pInstruction->Arg2;
	ElementType et2 = (ElementType)(uint32_t)pInstruction->Arg3;
	bool_t fCompare = FALSE;
	bool_t is64bit = FALSE;
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
		default:
			Panic("Invalid arguments for Compare");
			break;
	}
	if (!fCompare && !is64bit)
	{
		pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
		pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, NULL);
		x86_alu_reg_reg(pCompiledCode, X86_CMP, SECONDARY_REG, PRIMARY_REG);
	}
	else
	{
		if (is64bit)
		{
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, NULL);
			x86_fild_membase(pCompiledCode, X86_ESP, 0, TRUE);
			x86_fild_membase(pCompiledCode, X86_ESP, 8, TRUE);
			x86_adjust_stack(pCompiledCode, -16);
		}
		else
		{
			size_t sizeOfSource1 = 0;
			size_t sizeOfSource2 = 0;
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &sizeOfSource1);
			if (sizeOfSource1 <= gSizeOfPointerInBytes) x86_push_reg(pCompiledCode, PRIMARY_REG);
			x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source2, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, &sizeOfSource2);
			if (sizeOfSource2 <= gSizeOfPointerInBytes) x86_push_reg(pCompiledCode, PRIMARY_REG);
			x86_fld_membase(pCompiledCode, X86_ESP, 0, TRUE);
			x86_adjust_stack(pCompiledCode, -(sizeOfSource1 + sizeOfSource2));
		}
		x86_fcomip(pCompiledCode, 1);
		x86_fdecstp(pCompiledCode);
	}
	unsigned char* BranchTrue = NULL;
	unsigned char* BranchFalse = NULL;
	bool_t needsBranch = TRUE;

	switch (cond)
	{
		case CompareCondition_Equal:
			// If they are equal the ZeroFlag will be 1
			//x86_alu_reg_reg(pCompiledCode, X86_XOR, X86_EAX, X86_EAX);
			//x86_lahf(pCompiledCode);
			//x86_alu_reg_imm(pCompiledCode, X86_AND, X86_EAX, (1 << 6)); // mask out everything except the ZeroFlag
			//x86_shift_reg_imm(pCompiledCode, X86_SHR, X86_EAX, 6);
			//// The only bit in eax that is now set is the ZeroFlag,
			//// and it is now at bit index 0.
			//needsBranch = FALSE;

			
			BranchTrue = (unsigned char*)pCompiledCode;
			x86_branch32(pCompiledCode, X86_CC_EQ, 0, FALSE);
			BranchFalse = (unsigned char*)pCompiledCode;
			x86_jump32(pCompiledCode, 0);
			
			break;
		case CompareCondition_Greater_Than:
			BranchTrue = (unsigned char*)pCompiledCode;
			x86_branch32(pCompiledCode, X86_CC_GT, 0, TRUE);
			BranchFalse = (unsigned char*)pCompiledCode;
			x86_jump32(pCompiledCode, 0);
			break;
		case CompareCondition_Greater_Than_Unsigned:
			BranchTrue = (unsigned char*)pCompiledCode;
			x86_branch32(pCompiledCode, X86_CC_GT, 0, FALSE);
			BranchFalse = (unsigned char*)pCompiledCode;
			x86_jump32(pCompiledCode, 0);
			break;
		case CompareCondition_Less_Than:
			BranchTrue = (unsigned char*)pCompiledCode;
			x86_branch32(pCompiledCode, X86_CC_LT, 0, TRUE);
			BranchFalse = (unsigned char*)pCompiledCode;
			x86_jump32(pCompiledCode, 0);
			break;
		case CompareCondition_Less_Than_Unsigned:
			BranchTrue = (unsigned char*)pCompiledCode;
			x86_branch32(pCompiledCode, X86_CC_LT, 0, FALSE);
			BranchFalse = (unsigned char*)pCompiledCode;
			x86_jump32(pCompiledCode, 0);
			break;
		default:
			Panic("Unknown CompareCondition");
			break;
	}
	if (needsBranch)
	{
		x86_patch(BranchTrue, (unsigned char*)pCompiledCode);
		x86_mov_reg_imm(pCompiledCode, PRIMARY_REG, 1);
		pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
		unsigned char* end = (unsigned char*)pCompiledCode;
		x86_jump32(pCompiledCode, 0);
		x86_patch(BranchFalse, (unsigned char*)pCompiledCode);
		x86_alu_reg_reg(pCompiledCode, X86_XOR, PRIMARY_REG, PRIMARY_REG);
		pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
		x86_patch(end, (unsigned char*)pCompiledCode);
	}
	else
	{
		pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
	}
	return pCompiledCode;
}

char* JIT_Emit_Load_Token(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	x86_mov_reg_imm(pCompiledCode, PRIMARY_REG, pInstruction->Arg3);
	pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
	return pCompiledCode;
}

char* JIT_Emit_MkRefAny(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	x86_adjust_stack(pCompiledCode, gSizeOfPointerInBytes * 3);
	x86_mov_membase_imm(pCompiledCode, X86_ESP, gSizeOfPointerInBytes << 1, 0, gSizeOfPointerInBytes);
	x86_mov_membase_reg(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, PRIMARY_REG, gSizeOfPointerInBytes);
	x86_mov_membase_imm(pCompiledCode, X86_ESP, 0, (uint32_t)pInstruction->Arg2, gSizeOfPointerInBytes);
	pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
	return pCompiledCode;
}

char* JIT_Emit_RefAnyVal(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, gSizeOfPointerInBytes, gSizeOfPointerInBytes);
	x86_adjust_stack(pCompiledCode, -(gSizeOfPointerInBytes * 3));
	pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
	return pCompiledCode;
}

char* JIT_Emit_RefAnyType(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->Source1, PRIMARY_REG, SECONDARY_REG, THIRD_REG, FALSE, 0, NULL);
	x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, 0, gSizeOfPointerInBytes);
	x86_adjust_stack(pCompiledCode, -(gSizeOfPointerInBytes * 3));
	pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG, NULL);
	return pCompiledCode;
}

// Stuff that calls methods

char* JIT_Emit_Jump(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	Panic("Jump emitter not yet implemented!");
	return pCompiledCode;
}

char* JIT_Emit_New_Object(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	IRMethod* method = (IRMethod*)pInstruction->Arg1;
	IRType* type = method->ParentAssembly->Types[method->MethodDefinition->TypeDefinition->TableIndex - 1];
	pCompiledCode = JIT_Emit_LoadDestinationAddress(pCompiledCode, pMethod, &pInstruction->Destination, PRIMARY_REG, SECONDARY_REG, THIRD_REG);

	if (!method->AssembledMethod) JIT_CompileMethod(method);
	if (!method->AssembledMethod)
	{
		char buf[256];
		snprintf(buf, 256, "Missing Required Internal Call: %s.%s.%s", method->MethodDefinition->TypeDefinition->Namespace, method->MethodDefinition->TypeDefinition->Name, method->MethodDefinition->Name);
		Panic(buf);
	}

	if (type->TypeDefinition != method->ParentAssembly->ParentDomain->CachedType___System_String)
	{
		if (!type->SizeCalculated) JIT_GetSizeOfType(type);
		x86_adjust_stack(pCompiledCode, (gSizeOfPointerInBytes * 3) + 4);
		x86_mov_membase_reg(pCompiledCode, X86_ESP, (gSizeOfPointerInBytes << 1) + 4, PRIMARY_REG, gSizeOfPointerInBytes);
		x86_mov_membase_imm(pCompiledCode, X86_ESP, gSizeOfPointerInBytes << 1, type->Size, 4);
		x86_mov_membase_imm(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, (size_t)type, gSizeOfPointerInBytes);
		x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, DOMAIN_REG, gSizeOfPointerInBytes);
		x86_call_code(pCompiledCode, GC_AllocateObject);
		x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, X86_ESP, (gSizeOfPointerInBytes << 1) + 4, gSizeOfPointerInBytes); // Contains GCObject->Data*
		//x86_mov_reg_membase(pCompiledCode, PRIMARY_REG, PRIMARY_REG, 0, gSizeOfPointerInBytes);
		x86_mov_reg_membase(pCompiledCode, DOMAIN_REG, X86_ESP, 0, gSizeOfPointerInBytes);
		x86_adjust_stack(pCompiledCode, -((gSizeOfPointerInBytes * 3) + 4));

		uint32_t parametersSize = 0;
		size_t sizeOfParameter = 0;
		for (uint32_t index = 0; index < pInstruction->SourceArrayLength; ++index)
		{
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->SourceArray[index], SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, &sizeOfParameter);
			if (sizeOfParameter <= gSizeOfPointerInBytes) x86_push_reg(pCompiledCode, SECONDARY_REG);
			parametersSize += sizeOfParameter;
		}
		x86_push_reg(pCompiledCode, PRIMARY_REG); // Push the GCObject->Data* (this reference)
		parametersSize += gSizeOfPointerInBytes;
		if (method->MethodDefinition->InternalCall)
		{
			// Internal calls must push domain pointer (EDI) to stack as first parameter
			x86_push_reg(pCompiledCode, DOMAIN_REG);
			parametersSize += gSizeOfPointerInBytes;
		}

		x86_call_code(pCompiledCode, method->AssembledMethod);
		if (method->MethodDefinition->InternalCall)
		{
			// Internal calls must restore domain pointer (EDI) from first parameter
			// when returning to managed code
			x86_mov_reg_membase(pCompiledCode, DOMAIN_REG, X86_ESP, 0, gSizeOfPointerInBytes);
		}
		x86_adjust_stack(pCompiledCode, -parametersSize);
	}
	else
	{
		// String constructors are all internal calls, but we need to mangle the way they work a bit, since they are only
		// ever called from here, it should be alright
		// First, they will return their own new object from the constructor, due to dynamic size allocation and the hash table
		// Secondly, since they are internal calls they must pass the domain and stream pointers on stack, but they do not
		// take an existing "this" reference, so we will put the output parameter void** for the allocated object there instead
		// just for string constructors to pass their newly allocated object back directly to the destination address
		uint32_t parametersSize = 0;
		size_t sizeOfParameter = 0;
		for (uint32_t index = 0; index < pInstruction->SourceArrayLength; ++index)
		{
			pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->SourceArray[index], SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, &sizeOfParameter);
			if (sizeOfParameter <= gSizeOfPointerInBytes) x86_push_reg(pCompiledCode, SECONDARY_REG);
			parametersSize += sizeOfParameter;
		}
		x86_adjust_stack(pCompiledCode, gSizeOfPointerInBytes << 1);
		x86_mov_membase_reg(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, PRIMARY_REG, gSizeOfPointerInBytes); // Contains GCObject->Data**, instead of GCObject->Data* for string constructor
		// Internal calls must push domain pointer (EDI) to stack as first parameter
		x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, DOMAIN_REG, gSizeOfPointerInBytes);
		parametersSize += (gSizeOfPointerInBytes << 1);

		x86_call_code(pCompiledCode, method->AssembledMethod);
		// Internal calls must restore domain pointer (EDI) from first parameter
		// when returning to managed code
		x86_mov_reg_membase(pCompiledCode, DOMAIN_REG, X86_ESP, 0, gSizeOfPointerInBytes);
		x86_adjust_stack(pCompiledCode, -parametersSize);
	}

	return pCompiledCode;
}

__attribute__((noreturn)) void JIT_Trampoline_CallVirtual(IRType* pType, uint32_t pMethodIndex, void* pObject)
{
	GCObject* obj = *(GCObject**)((size_t)pObject - gSizeOfPointerInBytes);
	IRMethod* mToCall = NULL;
	if (pType->IsInterface)
	{
		IRInterfaceImpl* impl = NULL;
		HASH_FIND(HashHandle, obj->Type->InterfaceTable, pType, sizeof(void*), impl);
		if (impl)
		{
			mToCall = obj->Type->Methods[impl->MethodIndexes[pMethodIndex]];
		}
		else
		{
			Panic("Normally would throw an exception here because the type of pObject doesn't implement the requested interface!");
		}
	}
	else if (pType == obj->Type || pType->Methods[pMethodIndex] == obj->Type->Methods[pMethodIndex])
	{
		mToCall = pType->Methods[pMethodIndex];
	}
	else if (IRType_IsSubclassOf(obj->Type, pType))
	{
		mToCall = obj->Type->Methods[pMethodIndex];
	}
	else
	{
		Panic("The type of pObject doesn't inherit from pType, thus a virtual call cannot be made upon it. (supposed to throw an exception here)");
	}

	printf("Doing a virtual call to %s.%s.%s\n", obj->Type->TypeDefinition->Namespace, obj->Type->TypeDefinition->Name, mToCall->MethodDefinition->Name);
	if (!mToCall->AssembledMethod) JIT_CompileMethod(mToCall);

	__asm("leave;"); // cleanup locals
	__asm("mov (%esp), %eax;");
	__asm("mov %eax, 8(%esp);");
	__asm("add $8, %esp;"); // cleanup parameters
	__asm__("jmp *%0;" : : "r" (mToCall->AssembledMethod));
	while(TRUE);
}

char* JIT_Emit_Call_Virtual(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	uint32_t parametersSize = 0;
	size_t sizeOfParameter = 0;
	for (uint32_t index = 0; index < pInstruction->SourceArrayLength; ++index)
	{
		pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->SourceArray[index], SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, &sizeOfParameter);
		if (sizeOfParameter <= gSizeOfPointerInBytes) x86_push_reg(pCompiledCode, SECONDARY_REG);
		parametersSize += sizeOfParameter;
	}

	x86_adjust_stack(pCompiledCode, gSizeOfPointerInBytes + 4);
	x86_mov_membase_imm(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, (uint32_t)pInstruction->Arg2, 4);
	x86_mov_membase_imm(pCompiledCode, X86_ESP, 0, (size_t)pInstruction->Arg1, gSizeOfPointerInBytes);

	x86_call_code(pCompiledCode, JIT_Trampoline_CallVirtual);
	x86_adjust_stack(pCompiledCode, -parametersSize);

	IRMethod* method = ((IRType*)pInstruction->Arg1)->Methods[(uint32_t)pInstruction->Arg2];
	if (method->Returns)
	{
		if (method->ReturnType->StackSize > 4 && method->ReturnType->StackSize <= 8)
		{
			x86_adjust_stack(pCompiledCode, gSizeOfPointerInBytes << 1);
			x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, X86_EAX, gSizeOfPointerInBytes);
			x86_mov_membase_reg(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, X86_EDX, gSizeOfPointerInBytes);
		}
		pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, X86_EAX, X86_EDX, X86_ECX, NULL);
	}

	return pCompiledCode;
}

char* JIT_Emit_Call_Constrained(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	Panic("CallConstrained emitter not yet implemented!");
	return pCompiledCode;
}

char* JIT_Emit_Call_Absolute(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	IRMethod* method = (IRMethod*)pInstruction->Arg1;

	if (!method->AssembledMethod) JIT_CompileMethod(method);

	uint32_t parametersSize = 0;
	size_t sizeOfParameter = 0;
	for (uint32_t index = 0; index < pInstruction->SourceArrayLength; ++index)
	{
		pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->SourceArray[index], SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, &sizeOfParameter);
		if (sizeOfParameter <= gSizeOfPointerInBytes) x86_push_reg(pCompiledCode, SECONDARY_REG);
		parametersSize += sizeOfParameter;
	}
	
	//if (!(method->MethodDefinition->Flags & MethodAttributes_Static))
	//{
	//	x86_mov_reg_membase(pCompiledCode, X86_ECX, X86_ESP, 0, gSizeOfPointerInBytes);
	//	x86_test_reg_reg(pCompiledCode, X86_ECX, X86_ECX);
	//	unsigned char* tmp = (unsigned char*)pCompiledCode;
	//	x86_branch32(pCompiledCode, X86_CC_NZ, 0, FALSE);
	//	x86_push_imm(pCompiledCode, (size_t)"Attempted to make a call with a null 'this' pointer!");
	//	x86_call_code(pCompiledCode, Panic);
	//	x86_patch(tmp, (unsigned char*)pCompiledCode);
	//}

	x86_call_code(pCompiledCode, method->AssembledMethod);
	x86_adjust_stack(pCompiledCode, -parametersSize);

	if (method->Returns)
	{
		if (method->ReturnType->StackSize > 4 && method->ReturnType->StackSize <= 8)
		{
			x86_adjust_stack(pCompiledCode, gSizeOfPointerInBytes << 1);
			x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, X86_EAX, gSizeOfPointerInBytes);
			x86_mov_membase_reg(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, X86_EDX, gSizeOfPointerInBytes);
		}
		pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, X86_EAX, X86_EDX, X86_ECX, NULL);
	}

	return pCompiledCode;
}

char* JIT_Emit_Call_Internal(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	IRMethod* method = (IRMethod*)pInstruction->Arg1;

	if (!method->AssembledMethod) JIT_CompileMethod(method);
	if (!method->AssembledMethod)
	{
		char buf[256];
		snprintf(buf, 256, "Missing Required Internal Call: %s.%s.%s", method->MethodDefinition->TypeDefinition->Namespace, method->MethodDefinition->TypeDefinition->Name, method->MethodDefinition->Name);
		Panic(buf);
	}

	uint32_t parametersSize = 0;
	size_t sizeOfParameter = 0;
	bool_t returnContainsReferenceType = method->Returns && IRType_ContainsReferenceType(method->ReturnType);
	if (returnContainsReferenceType)
	{
		pCompiledCode = JIT_Emit_LoadDestinationAddress(pCompiledCode, pMethod, &pInstruction->Destination, SECONDARY_REG, THIRD_REG, FOURTH_REG);
		x86_push_reg(pCompiledCode, SECONDARY_REG);
		parametersSize += gSizeOfPointerInBytes;
	}
	for (uint32_t index = 0; index < pInstruction->SourceArrayLength; ++index)
	{
		pCompiledCode = JIT_Emit_Load(pCompiledCode, pMethod, &pInstruction->SourceArray[index], SECONDARY_REG, THIRD_REG, FOURTH_REG, FALSE, 0, &sizeOfParameter);
		if (sizeOfParameter <= gSizeOfPointerInBytes) x86_push_reg(pCompiledCode, SECONDARY_REG);
		parametersSize += sizeOfParameter;
	}
	x86_push_reg(pCompiledCode, DOMAIN_REG);
	parametersSize += gSizeOfPointerInBytes;

	x86_call_code(pCompiledCode, method->AssembledMethod);
	x86_mov_reg_membase(pCompiledCode, DOMAIN_REG, X86_ESP, 0, gSizeOfPointerInBytes);
	x86_adjust_stack(pCompiledCode, -parametersSize);

	if (method->Returns && !returnContainsReferenceType)
	{
		if (method->ReturnType->StackSize > 4 && method->ReturnType->StackSize <= 8)
		{
			x86_adjust_stack(pCompiledCode, gSizeOfPointerInBytes << 1);
			x86_mov_membase_reg(pCompiledCode, X86_ESP, 0, X86_EAX, gSizeOfPointerInBytes);
			x86_mov_membase_reg(pCompiledCode, X86_ESP, gSizeOfPointerInBytes, X86_EDX, gSizeOfPointerInBytes);
		}
		pCompiledCode = JIT_Emit_Store(pCompiledCode, pMethod, &pInstruction->Destination, X86_EAX, X86_EDX, X86_ECX, NULL);
	}

	return pCompiledCode;
}

char* JIT_Emit_Call_Intrinsic(char* pCompiledCode, IRMethod* pMethod, IRInstruction* pInstruction, BranchRegistry* pBranchRegistry)
{
	IntrinsicCallType callType = (IntrinsicCallType)(uint32_t)pInstruction->Arg1;
	switch (callType)
	{
		default:
		{
			printf("Unimplemented IntrinsicCallType: %u\n", (unsigned int)callType);
			Panic("Unimplemented IntrinsicCallType!\n");
			break;
		}
	}
	return pCompiledCode;
}
