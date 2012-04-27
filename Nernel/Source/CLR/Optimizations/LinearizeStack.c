#include <CLR/Optimizations/LinearizeStack.h>
#include <CLR/SyntheticStack.h>

#define PA() StackObjectPool_Allocate(stack)
#define PR() StackObjectPool_Release(stack)
#define Push(obj) SyntheticStack_Push(stack, obj)
#define Pop() SyntheticStack_Pop(stack)

void IROptimizer_LinearizeStack(IRMethod* pMethod)
{
	SyntheticStack* stack = SyntheticStack_Create();
	StackObjectPool_Initialize(stack);
	StackObject* obj = NULL;
	for (uint32_t i = 0; i < pMethod->IRCodesCount; i++)
	{
		IRInstruction* ins = pMethod->IRCodes[i];
		switch(ins->Opcode)
		{

			// These next few are all source points.
            case IROpcode_Load_Parameter:
				obj = PA();
				obj->LinearData.Source = SourceType_Parameter;
				obj->LinearData.SourceData.Parameter.ParameterIndex = (uint32_t)ins->Arg1;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_Parameter_Address:
				obj = PA();
				obj->LinearData.Source = SourceType_ParameterAddress;
				obj->LinearData.SourceData.ParameterAddress.ParameterIndex = (uint32_t)ins->Arg1;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_Local:
				obj = PA();
				obj->LinearData.Source = SourceType_Local;
				obj->LinearData.SourceData.LocalVariable.LocalVariableIndex = (uint32_t)ins->Arg1;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_Local_Address:
				obj = PA();
				obj->LinearData.Source = SourceType_LocalAddress;
				obj->LinearData.SourceData.LocalVariableAddress.LocalVariableIndex = (uint32_t)ins->Arg1;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_Null:
				obj = PA();
				obj->LinearData.Source = SourceType_Null;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_String:
				obj = PA();
				obj->LinearData.Source = SourceType_StringLiteral;
				obj->LinearData.SourceData.StringLiteral.Length = (uint32_t)ins->Arg1;
				obj->LinearData.SourceData.StringLiteral.Data = (uint8_t*)ins->Arg2;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_Int32:
				obj = PA();
				obj->LinearData.Source = SourceType_ConstantI4;
				obj->LinearData.SourceData.ConstantI4.Value = (uint32_t)ins->Arg1;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_Int64:
				obj = PA();
				obj->LinearData.Source = SourceType_ConstantI8;
				obj->LinearData.SourceData.ConstantI8.Value = *(uint64_t*)ins->Arg1;
				free(ins->Arg1);
				ins->Arg1NeedsDisposing = FALSE;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_Float32:
				obj = PA();
				obj->LinearData.Source = SourceType_ConstantR4;
				obj->LinearData.SourceData.ConstantR4.Value = (uint32_t)ins->Arg1;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_Float64:
				obj = PA();
				obj->LinearData.Source = SourceType_ConstantR8;
				obj->LinearData.SourceData.ConstantR8.Value = *(uint64_t*)ins->Arg1;
				free(ins->Arg1);
				ins->Arg1NeedsDisposing = FALSE;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_Field:
				obj = PA();
				obj->LinearData.Source = SourceType_Field;
				obj->LinearData.SourceData.Field.ParentType = (IRType*)ins->Arg1;
				obj->LinearData.SourceData.Field.FieldIndex = (uint32_t)ins->Arg3;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_FieldAddress:
				obj = PA();
				obj->LinearData.Source = SourceType_FieldAddress;
				obj->LinearData.SourceData.FieldAddress.ParentType = (IRType*)ins->Arg1;
				obj->LinearData.SourceData.FieldAddress.FieldIndex = (uint32_t)ins->Arg3;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_StaticField:
				obj = PA();
				obj->LinearData.Source = SourceType_StaticField;
				obj->LinearData.SourceData.StaticField.Field = (IRField*)ins->Arg1;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;
            case IROpcode_Load_StaticFieldAddress:
				obj = PA();
				obj->LinearData.Source = SourceType_StaticFieldAddress;
				obj->LinearData.SourceData.StaticFieldAddress.Field = (IRField*)ins->Arg1;
				Push(obj);
				ins->Opcode = IROpcode_Nop;
                break;

				
            case IROpcode_Dup:
				Push(SyntheticStack_Peek(stack));
                break;
            case IROpcode_Return:
				if (pMethod->Returns)
				{
					obj = Pop();
					ins->Source1Type = obj->LinearData.Source;
					ins->Source1Data = obj->LinearData.SourceData;
				}
                break;
            case IROpcode_Store_Parameter:
                break;
            case IROpcode_Store_Local:
                break;
            case IROpcode_Pop:
                break;
            case IROpcode_Load_Indirect:
                break;
            case IROpcode_Store_Indirect:
                break;
            case IROpcode_Add:
                break;
            case IROpcode_Sub:
                break;
            case IROpcode_Mul:
                break;
            case IROpcode_Div:
                break;
            case IROpcode_Rem:
                break;
            case IROpcode_And:
                break;
            case IROpcode_Or:
                break;
            case IROpcode_Xor:
                break;
            case IROpcode_Neg:
                break;
            case IROpcode_Not:
                break;
            case IROpcode_Shift:
                break;
            case IROpcode_Convert_Unchecked:
                break;
            case IROpcode_Convert_Checked:
                break;
            case IROpcode_CastClass:
                break;
            case IROpcode_IsInst:
                break;
            case IROpcode_Unbox:
                break;
            case IROpcode_Unbox_Any:
                break;
            case IROpcode_Box:
                break;
            case IROpcode_Throw:
                break;
            case IROpcode_Load_Object:
                break;
            case IROpcode_Store_Object:
                break;
            case IROpcode_Copy_Object:
                break;
            case IROpcode_New_Object:
                break;
            case IROpcode_Load_ArrayLength:
                break;
            case IROpcode_New_Array:
                break;
            case IROpcode_CheckFinite:
                break;
            case IROpcode_Store_Field:
                break;
            case IROpcode_Store_StaticField:
                break;
            case IROpcode_Load_Element:
                break;
            case IROpcode_Load_ElementAddress:
                break;
            case IROpcode_Store_Element:
                break;
            case IROpcode_Allocate_Local:
                break;
            case IROpcode_Initialize_Object:
                break;
            case IROpcode_Copy_Block:
                break;
            case IROpcode_Initialize_Block:
                break;
            case IROpcode_SizeOf:
                break;
            case IROpcode_Jump:
                break;
            case IROpcode_Call_Virtual:
                break;
            case IROpcode_Call_Constrained:
                break;
            case IROpcode_Call_Absolute:
                break;
            case IROpcode_Call_Internal:
                break;
            case IROpcode_Branch:
                break;
            case IROpcode_Switch:
                break;
            case IROpcode_Load_Function:
                break;
            case IROpcode_Load_VirtualFunction:
                break;
            case IROpcode_Compare:
                break;
            case IROpcode_Load_Token:
                break;
            case IROpcode_MkRefAny:
                break;
            case IROpcode_RefAnyVal:
                break;
            case IROpcode_RefAnyType:
                break;
				
			// These op-codes do nothing
			// in terms of stack analysis
			// or linearization of the stack.
            case IROpcode_Nop:
            case IROpcode_Break:
                break;

			// There is no default here for good reason/
        }
	}

	StackObjectPool_Destroy(stack);
	SyntheticStack_Destroy(stack);
}