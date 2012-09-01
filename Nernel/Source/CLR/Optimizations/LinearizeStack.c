#include <CLR/Optimizations/LinearizeStack.h>
#include <CLR/SyntheticStack.h>

#define PA() StackObjectPool_Allocate(stack)
#define PR(obj) StackObjectPool_Release(stack, obj)
#define Push(obj) SyntheticStack_Push(stack, obj)
#define Pop() SyntheticStack_Pop(stack)
#define Peek() SyntheticStack_Peek(stack)

ElementType GetElementTypeOfSourceType(SourceType sourceType, SourceData sourceData, IRMethod* pMethod)
{
	switch(sourceType)
	{
		case SourceType_SizeOf:
		case SourceType_ArrayLength:
		case SourceType_ConstantI4:
			return ElementType_I4;
		case SourceType_ConstantI8:
			return ElementType_I8;
		case SourceType_ConstantR4:
			return ElementType_R4;
		case SourceType_ConstantR8:
			return ElementType_R8;
		case SourceType_FieldAddress:
		case SourceType_LocalAddress:
		case SourceType_ParameterAddress:
		case SourceType_StaticFieldAddress:
		case SourceType_ArrayElementAddress:
			return ElementType_I;
		case SourceType_Field:
			return AppDomain_GetElementTypeFromIRType(pMethod->ParentAssembly->ParentDomain, sourceData.Field.ParentType->Fields[sourceData.Field.FieldIndex]->FieldType);
		case SourceType_StaticField:
			return AppDomain_GetElementTypeFromIRType(pMethod->ParentAssembly->ParentDomain, sourceData.StaticField.Field->FieldType);
		case SourceType_Local:
			return AppDomain_GetElementTypeFromIRType(pMethod->ParentAssembly->ParentDomain, pMethod->LocalVariables[sourceData.LocalVariable.LocalVariableIndex]->VariableType);
		case SourceType_Parameter:
			return AppDomain_GetElementTypeFromIRType(pMethod->ParentAssembly->ParentDomain, pMethod->Parameters[sourceData.Parameter.ParameterIndex]->Type);
		case SourceType_Indirect:
			return AppDomain_GetElementTypeFromIRType(pMethod->ParentAssembly->ParentDomain, sourceData.Indirect.Type);
		case SourceType_ArrayElement:
			return AppDomain_GetElementTypeFromIRType(pMethod->ParentAssembly->ParentDomain, sourceData.ArrayElement.ElementType);

		case SourceType_Null:
			Panic("Invalid Source type!");
			break;
	}
	Panic("Unknown SourceType!");
	return (ElementType)0xFFFF;
}

IRType* GetIRTypeOfSourceType(SourceType sourceType, SourceData sourceData, IRMethod* pMethod)
{
	switch(sourceType)
	{
		case SourceType_SizeOf:
		case SourceType_ConstantI4:
		case SourceType_ArrayLength:
			return pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_Int32->TableIndex - 1];
		case SourceType_ConstantI8:
			return pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_Int64->TableIndex - 1];
		case SourceType_ConstantR4:
			return pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_Single->TableIndex - 1];
		case SourceType_ConstantR8:
			return pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_Double->TableIndex - 1];
		case SourceType_Null:
			return pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_Object->TableIndex - 1];
		case SourceType_FieldAddress:
		case SourceType_LocalAddress:
		case SourceType_ParameterAddress:
		case SourceType_StaticFieldAddress:
		case SourceType_ArrayElementAddress:
			return pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_IntPtr->TableIndex - 1];
		case SourceType_Field:
			return sourceData.Field.ParentType->Fields[sourceData.Field.FieldIndex]->FieldType;
		case SourceType_StaticField:
			return sourceData.StaticField.Field->FieldType;
		case SourceType_Local:
			return pMethod->LocalVariables[sourceData.LocalVariable.LocalVariableIndex]->VariableType;
		case SourceType_Parameter:
			return pMethod->Parameters[sourceData.Parameter.ParameterIndex]->Type;
		case SourceType_Indirect:
			return sourceData.Indirect.Type;
		case SourceType_ArrayElement:
			return sourceData.ArrayElement.ElementType;
	}
	Panic("Unknown SourceType!");
	return NULL;
}

typedef struct StackLocal
{
	uint32_t StackDepthLocation;
	IRType* Type;
	
	uint32_t LocalIndex;
	UT_hash_handle HashHandle;
} StackLocal;

uint32_t AddLocal(IRType* localType, IRMethod* pMethod, uint32_t depth, StackLocal** stackLocalTable)
{
	printf("StackLinearize AddLocal: 0x%X %s\n", (unsigned int)localType, localType->TypeDefinition->Name);
	if (!localType) Panic("Grr");
	if (!localType->TypeDefinition)
	{
		Panic("Well, it wasn't resolved :(");
	}
	StackLocal lKey;
	StackLocal* lKeyPtr = &lKey;
	lKey.StackDepthLocation = depth;
	lKey.Type = localType;

	StackLocal* fLocal = NULL;
	HASH_FIND(HashHandle, *stackLocalTable, lKeyPtr, offsetof(StackLocal, LocalIndex), fLocal);
	if (!fLocal)
	{
		IRLocalVariable* loc = IRLocalVariable_Create(pMethod, localType);
		loc->NonSSADerivable = TRUE;
		IRMethod_AddLocal(pMethod, loc);
		fLocal = (StackLocal*)calloc(1, sizeof(StackLocal));
		fLocal->StackDepthLocation = depth;
		fLocal->Type = localType;
		fLocal->LocalIndex = loc->LocalVariableIndex;
		HASH_ADD(HashHandle, *stackLocalTable, StackDepthLocation, offsetof(StackLocal, LocalIndex), fLocal);
		printf("Creating a local at index %i for stack depth of %i of the type (0x%x) %s.%s\n", (int)loc->LocalVariableIndex, (int)depth, (unsigned int)localType, localType->TypeDefinition->Namespace, localType->TypeDefinition->Name);
		return loc->LocalVariableIndex;
	}
	else
	{
		printf("Not creating a local at index %i for stack depth of %i of the type (0x%x) %s.%s\n", (int)fLocal->LocalIndex, (int)depth, (unsigned int)localType, localType->TypeDefinition->Namespace, localType->TypeDefinition->Name);
		return fLocal->LocalIndex;
	}
}

void IROptimizer_LinearizeStack(IRMethod* pMethod)
{
	printf("This pMethod is <b>EBIL</b> because it has %u locals, and %u opcodes.\n", (unsigned int)pMethod->LocalVariableCount, (unsigned int)pMethod->IRCodesCount);
	StackLocal* stackLocalTable = NULL;
	SyntheticStack* stack = SyntheticStack_Create();
	StackObjectPool_Initialize(stack);
	StackObject* obj = NULL;
	StackObject* obj2 = NULL;
	//printf("IRCode Count: %i\n", (int)pMethod->IRCodesCount);
	for (uint32_t i = 0; i < pMethod->IRCodesCount; i++)
	{
		IRInstruction* ins = pMethod->IRCodes[i];
		//printf("Instruction %i is %i at IL 0x%x\n", (int)i, (int)ins->Opcode, (unsigned int)ins->ILLocation);
		switch(ins->Opcode)
		{
			// These next few are all source points.
            case IROpcode_Load_Parameter:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(pMethod->Parameters[((uint32_t)ins->Arg1)]->Type, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Opcode = IROpcode_Move;
				ins->Source1.Type = SourceType_Parameter;
				ins->Source1.Data.Parameter.ParameterIndex = (uint32_t)ins->Arg1;
				ins->Arg1 = NULL;
				Push(obj);
                break;
            case IROpcode_Load_ParameterAddress:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Opcode = IROpcode_Move;
				ins->Source1.Type = SourceType_ParameterAddress;
				ins->Source1.Data.ParameterAddress.ParameterIndex = (uint32_t)ins->Arg1;
				ins->Arg1 = NULL;
				Push(obj);
                break;
            case IROpcode_Load_Local:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(pMethod->LocalVariables[((uint32_t)ins->Arg1)]->VariableType, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Opcode = IROpcode_Move;
				ins->Source1.Type = SourceType_Local;
				ins->Source1.Data.LocalVariable.LocalVariableIndex = (uint32_t)ins->Arg1;
				ins->Arg1 = NULL;
				Push(obj);
                break;
            case IROpcode_Load_LocalAddress:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Opcode = IROpcode_Move;
				ins->Source1.Type = SourceType_LocalAddress;
				ins->Source1.Data.LocalVariableAddress.LocalVariableIndex = (uint32_t)ins->Arg1;
				ins->Arg1 = NULL;
				Push(obj);
                break;
            case IROpcode_SizeOf:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I4), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Source1.Type = SourceType_SizeOf;
				ins->Source1.Data.SizeOf.Type = (IRType*)ins->Arg1;
				ins->Opcode = IROpcode_Move;
				Push(obj);
                break;
            case IROpcode_Load_Null:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_Object->TableIndex - 1], pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Source1.Type = SourceType_Null;
				ins->Opcode = IROpcode_Move;
				Push(obj);
                break;
            case IROpcode_Load_String:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_String->TableIndex - 1], pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				Push(obj);
                break;
            case IROpcode_Load_Int32:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I4), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Source1.Type = SourceType_ConstantI4;
				ins->Source1.Data.ConstantI4.Value = (uint32_t)ins->Arg1;
				ins->Opcode = IROpcode_Move;
				Push(obj);
                break;
            case IROpcode_Load_Int64:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I8), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Source1.Type = SourceType_ConstantI8;
				ins->Source1.Data.ConstantI8.Value = *(uint64_t*)ins->Arg1;
				ins->Opcode = IROpcode_Move;
				Push(obj);
				free(ins->Arg1);
				ins->Arg1NeedsDisposing = FALSE;
                break;
            case IROpcode_Load_Float32:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_R4), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Source1.Type = SourceType_ConstantR4;
				ins->Source1.Data.ConstantR4.Value = (uint32_t)ins->Arg1;
				ins->Opcode = IROpcode_Move;
				Push(obj);
                break;
            case IROpcode_Load_Float64:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_R8), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Source1.Type = SourceType_ConstantR8;
				ins->Source1.Data.ConstantR8.Value = *(uint64_t*)ins->Arg1;
				ins->Opcode = IROpcode_Move;
				Push(obj);
				free(ins->Arg1);
				ins->Arg1NeedsDisposing = FALSE;
                break;
            case IROpcode_Load_Field:
			{
				obj = Pop();
				SourceTypeData* sDat = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
				*sDat = obj->LinearData;
				PR(obj);
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(((IRType*)ins->Arg1)->Fields[((uint32_t)ins->Arg3)]->FieldType, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Opcode = IROpcode_Move;
				ins->Source1.Type = SourceType_Field;
				ins->Source1.Data.Field.FieldIndex = (uint32_t)ins->Arg3;
				ins->Source1.Data.Field.ParentType = (IRType*)ins->Arg1;
				ins->Source1.Data.Field.FieldSource = sDat;
				ins->Arg1 = NULL;
				ins->Arg2 = NULL;
				ins->Arg3 = NULL;
				Push(obj);
                break;
			}
            case IROpcode_Load_FieldAddress:
			{
				obj = Pop();
				SourceTypeData* sDat = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
				*sDat = obj->LinearData;
				PR(obj);
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Opcode = IROpcode_Move;
				ins->Source1.Type = SourceType_FieldAddress;
				ins->Source1.Data.FieldAddress.FieldIndex = (uint32_t)ins->Arg3;
				ins->Source1.Data.FieldAddress.ParentType = (IRType*)ins->Arg1;
				ins->Source1.Data.FieldAddress.FieldSource = sDat;
				ins->Arg1 = NULL;
				ins->Arg2 = NULL;
				ins->Arg3 = NULL;
				Push(obj);
                break;
			}
            case IROpcode_Store_Field:
			{
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				obj = Pop();
				SourceTypeData* sDat = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
				*sDat = obj->LinearData;
				PR(obj);
				ins->Opcode = IROpcode_Move;
				ins->Destination.Type = SourceType_Field;
				ins->Destination.Data.Field.ParentType = (IRType*)ins->Arg1;
				ins->Destination.Data.Field.FieldIndex = (uint32_t)ins->Arg3;
				ins->Destination.Data.Field.FieldSource = sDat;
				ins->Arg1 = NULL;
				ins->Arg2 = NULL;
				ins->Arg3 = NULL;
                break;
			}
            case IROpcode_Load_StaticField:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(((IRField*)ins->Arg1)->FieldType, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Opcode = IROpcode_Move;
				ins->Source1.Type = SourceType_StaticField;
				ins->Source1.Data.StaticField.Field = (IRField*)ins->Arg1;
				ins->Arg1 = NULL;
				Push(obj);
                break;
            case IROpcode_Load_StaticFieldAddress:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data = obj->LinearData.Data;
				ins->Opcode = IROpcode_Move;
				ins->Source1.Type = SourceType_StaticFieldAddress;
				ins->Source1.Data.StaticFieldAddress.Field = (IRField*)ins->Arg1;
				ins->Arg1 = NULL;
				Push(obj);
                break;
            case IROpcode_Store_Local:
				obj = Pop();
				ins->Source1.Type = obj->LinearData.Type;
				ins->Source1.Data = obj->LinearData.Data;
				PR(obj);
				ins->Opcode = IROpcode_Move;
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data.LocalVariable.LocalVariableIndex = (uint32_t)ins->Arg1;
				ins->Arg1 = NULL;
                break;
            case IROpcode_Store_StaticField:
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				ins->Opcode = IROpcode_Move;
				ins->Destination.Type = SourceType_StaticField;
				ins->Destination.Data.StaticField.Field = (IRField*)ins->Arg1;
				ins->Arg1 = NULL;
                break;
            case IROpcode_Store_Parameter:
				obj = Pop();
				ins->Source1.Type = obj->LinearData.Type;
				ins->Source1.Data = obj->LinearData.Data;
				PR(obj);
				ins->Opcode = IROpcode_Move;
				ins->Destination.Type = SourceType_Parameter;
				ins->Destination.Data.Parameter.ParameterIndex = (uint32_t)ins->Arg1;
				ins->Arg1 = NULL;
                break;
            case IROpcode_Store_Object:
            case IROpcode_Store_Indirect:
			{
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				obj = Pop();
				SourceTypeData* sDat = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
				*sDat = obj->LinearData;
				PR(obj);
				ins->Destination.Type = SourceType_Indirect;
				if (ins->Opcode == IROpcode_Store_Object)
				{
					ins->Destination.Data.Indirect.Type = (IRType*)ins->Arg2;
				}
				else
				{
					ins->Destination.Data.Indirect.Type = (IRType*)ins->Arg1;
				}
				ins->Destination.Data.Indirect.AddressSource = sDat;
				ins->Opcode = IROpcode_Move;
                break;
			}
            case IROpcode_Load_Object:
            case IROpcode_Load_Indirect:
			{
				obj = Pop();
				SourceTypeData* sDat = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
				*sDat = obj->LinearData;
				PR(obj);
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal((IRType*)ins->Arg1, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				ins->Opcode = IROpcode_Move;
				ins->Source1.Type = SourceType_Indirect;
				ins->Source1.Data.Indirect.Type = (IRType*)ins->Arg1;
				ins->Source1.Data.Indirect.AddressSource = sDat;
				Push(obj);
                break;
			}
            case IROpcode_Load_Element:
			{
				obj = Pop();
				SourceTypeData* sIDat = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
				*sIDat = obj->LinearData;
				PR(obj);
				obj = Pop();
				SourceTypeData* sADat = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
				*sADat = obj->LinearData;
				PR(obj);
				ins->Source1.Type = SourceType_ArrayElement;
				ins->Source1.Data.ArrayElement.IndexSource = sIDat;
				ins->Source1.Data.ArrayElement.ArraySource = sADat;
				ins->Source1.Data.ArrayElement.ElementType = GetIRTypeOfSourceType(sADat->Type, sADat->Data, pMethod)->ArrayType->ElementType;
				ins->Opcode = IROpcode_Move;
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(ins->Source1.Data.ArrayElement.ElementType, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				Push(obj);
				ins->Arg1 = NULL;
				ins->Arg2 = NULL;
                break;
			}
            case IROpcode_Load_ElementAddress:
			{
				obj = Pop();
				SourceTypeData* sIDat = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
				*sIDat = obj->LinearData;
				PR(obj);
				obj = Pop();
				SourceTypeData* sADat = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
				*sADat = obj->LinearData;
				PR(obj);
				ins->Source1.Type = SourceType_ArrayElementAddress;
				ins->Source1.Data.ArrayElementAddress.IndexSource = sIDat;
				ins->Source1.Data.ArrayElementAddress.ArraySource = sADat;
				ins->Source1.Data.ArrayElementAddress.ElementType = (IRType*)ins->Arg2;
				printf("linearize loading array element %s.%s\n", ins->Source1.Data.ArrayElement.ElementType->TypeDefinition->Namespace, ins->Source1.Data.ArrayElement.ElementType->TypeDefinition->Name);
				ins->Opcode = IROpcode_Move;
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				Push(obj);
				ins->Arg1 = NULL;
				ins->Arg2 = NULL;
                break;
			}
            case IROpcode_Store_Element:
			{
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				obj = Pop();
				SourceTypeData* sIDat = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
				*sIDat = obj->LinearData;
				PR(obj);
				obj = Pop();
				SourceTypeData* sADat = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
				*sADat = obj->LinearData;
				PR(obj);
				ins->Destination.Type = SourceType_ArrayElement;
				ins->Destination.Data.ArrayElement.IndexSource = sIDat;
				ins->Destination.Data.ArrayElement.ArraySource = sADat;
				ins->Destination.Data.ArrayElement.ElementType = (IRType*)ins->Arg2;
				ins->Opcode = IROpcode_Move;
				ins->Arg1 = NULL;
				ins->Arg2 = NULL;
                break;
			}
            case IROpcode_Load_ArrayLength:
			{
				obj = Pop();
				SourceTypeData* sDat = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
				*sDat = obj->LinearData;
				PR(obj);
				ins->Source1.Type = SourceType_ArrayLength;
				ins->Source1.Data.ArrayLength.ArraySource = sDat;
				ins->Opcode = IROpcode_Move;
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I4), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				Push(obj);
                break;
			}

            case IROpcode_Pop:
				PR(Pop());
				ins->Opcode = IROpcode_Nop;
                break;

				
            case IROpcode_Dup:
			{
				IRType* destType = NULL;
				obj = PA();

				obj2 = Pop();
				destType = GetIRTypeOfSourceType(obj2->LinearData.Type, obj2->LinearData.Data, pMethod);
				ins->Source1 = obj2->LinearData;
				PR(obj2);

				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(destType, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data.LocalVariable.LocalVariableIndex = obj->LinearData.Data.LocalVariable.LocalVariableIndex;
				Push(obj);

				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(destType, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Source3.Type = SourceType_Local;
				ins->Source3.Data.LocalVariable.LocalVariableIndex = obj->LinearData.Data.LocalVariable.LocalVariableIndex;
				Push(obj);
                break;
			}
            case IROpcode_Return:
				if (pMethod->Returns)
				{
					obj = Pop();
					ins->Source1.Type = obj->LinearData.Type;
					ins->Source1.Data = obj->LinearData.Data;
					PR(obj);
				}
                break;
            case IROpcode_Throw:
				obj = Pop();
				ins->Source1.Type = obj->LinearData.Type;
				ins->Source1.Data = obj->LinearData.Data;
				PR(obj);
                break;
				
            case IROpcode_Rem:
            case IROpcode_Div:
            case IROpcode_Mul:
            case IROpcode_Sub:
            case IROpcode_Add:
            case IROpcode_And:
            case IROpcode_Or:
            case IROpcode_Xor:
			{
				ElementType arg1 = (ElementType)0;
				ElementType arg2 = (ElementType)0;

				obj = Pop();
				ins->Source1.Type = obj->LinearData.Type;
				ins->Source1.Data = obj->LinearData.Data;
				arg1 = GetElementTypeOfSourceType(ins->Source1.Type, ins->Source1.Data, pMethod);
				PR(obj);
				obj = Pop();
				ins->Source2.Type = obj->LinearData.Type;
				ins->Source2.Data = obj->LinearData.Data;
				printf("Source2.Type = %u, Source2.Data = %u, Type = %s\n", (unsigned int)ins->Source2.Type, (unsigned int)ins->Source2.Data.LocalVariable.LocalVariableIndex, pMethod->LocalVariables[4]->VariableType->TypeDefinition->Name);
				arg2 = GetElementTypeOfSourceType(ins->Source2.Type, ins->Source2.Data, pMethod);
				PR(obj);

				IRType* destType = NULL;
				switch(arg1)
				{
					case ElementType_I:
					case ElementType_U:
						switch(arg2)
						{
							case ElementType_I:
							case ElementType_U:
							case ElementType_I1:
							case ElementType_I2:
							case ElementType_I4:
							case ElementType_U1:
							case ElementType_U2:
							case ElementType_U4:
								destType = AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I);
								break;
							default:
								Panic("Invalid Element Type 1!");
								break;
						}
						break;
					case ElementType_I1:
					case ElementType_I2:
					case ElementType_I4:
					case ElementType_U1:
					case ElementType_U2:
					case ElementType_U4:
						switch(arg2)
						{
							case ElementType_I:
							case ElementType_U:
								destType = AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I);
								break;
							case ElementType_I1:
							case ElementType_I2:
							case ElementType_I4:
							case ElementType_U1:
							case ElementType_U2:
							case ElementType_U4:
								destType = AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I4);
								break;
							default:
								printf("ElementType = %i\n", (int)arg2);
								Panic("Invalid Element Type 2!");
								break;
						}
						break;
					case ElementType_I8:
					case ElementType_U8:
						switch(arg2)
						{
							case ElementType_I8:
							case ElementType_U8:
								destType = AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I8);
								break;
							default:
								Panic("Invalid Element Type 3!");
								break;
						}
						break;
					case ElementType_R4:
					case ElementType_R8:
						switch(arg2)
						{
							case ElementType_R4:
							case ElementType_R8:
								destType = AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_R8);
								break;
							default:
								Panic("Invalid Element Type 4!");
								break;
						}
						break;
				}
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(destType, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data.LocalVariable.LocalVariableIndex = obj->LinearData.Data.LocalVariable.LocalVariableIndex;
				Push(obj);
                break;
			}
			
            case IROpcode_Not:
            case IROpcode_Neg:
				obj = Pop();
				ins->Source1.Type = obj->LinearData.Type;
				ins->Source1.Data = obj->LinearData.Data;
				PR(obj);
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, (ElementType)(uint32_t)ins->Arg1), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data.LocalVariable.LocalVariableIndex = obj->LinearData.Data.LocalVariable.LocalVariableIndex;
				Push(obj);
                break;

            case IROpcode_Shift:
			{
				ElementType arg1 = (ElementType)0;
				ElementType arg2 = (ElementType)0;

				obj = Pop();
				ins->Source1.Type = obj->LinearData.Type;
				ins->Source1.Data = obj->LinearData.Data;
				arg1 = GetElementTypeOfSourceType(ins->Source1.Type, ins->Source1.Data, pMethod);
				PR(obj);
				obj = Pop();
				ins->Source2.Type = obj->LinearData.Type;
				ins->Source2.Data = obj->LinearData.Data;
				arg2 = GetElementTypeOfSourceType(ins->Source2.Type, ins->Source2.Data, pMethod);
				PR(obj);

				IRType* destType = NULL;
				switch(arg1)
				{
					case ElementType_I:
					case ElementType_U:
						switch(arg2)
						{
							case ElementType_I:
							case ElementType_U:
							case ElementType_I1:
							case ElementType_I2:
							case ElementType_I4:
							case ElementType_U1:
							case ElementType_U2:
							case ElementType_U4:
								destType = AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I);
								break;
							default:
								Panic("Invalid Element Type 5!");
								break;
						}
						break;
					case ElementType_I1:
					case ElementType_I2:
					case ElementType_I4:
					case ElementType_U1:
					case ElementType_U2:
					case ElementType_U4:
						switch(arg2)
						{
							case ElementType_I:
							case ElementType_U:
							case ElementType_I1:
							case ElementType_I2:
							case ElementType_I4:
							case ElementType_U1:
							case ElementType_U2:
							case ElementType_U4:
								destType = AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I4);
								break;
							default:
								Panic("Invalid Element Type 6!");
								break;
						}
						break;
					case ElementType_I8:
					case ElementType_U8:
						switch(arg2)
						{
							case ElementType_I8:
							case ElementType_U8:
								destType = AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I8);
								break;
							default:
								Panic("Invalid Element Type 7!");
								break;
						}
						break;
					default:
						Panic("Invalid Element Type 8!");
						break;
				}
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(destType, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data.LocalVariable.LocalVariableIndex = obj->LinearData.Data.LocalVariable.LocalVariableIndex;
				Push(obj);
                break;
			}

            case IROpcode_New_Object:
			{
				uint32_t paramCount = ((IRMethod*)ins->Arg1)->ParameterCount - 1;
				ins->SourceArray = (SourceTypeData*)calloc(1, sizeof(SourceTypeData) * paramCount);
				ins->SourceArrayLength = paramCount;
				for (uint32_t i2 = 0; i2 < paramCount; i2++)
				{
					obj = Pop();
					ins->SourceArray[i2].Type = obj->LinearData.Type;
					ins->SourceArray[i2].Data = obj->LinearData.Data;
					PR(obj);
				}

				// This is a new object call, all constructors are instance
				// methods, so the first arg will always be the parent type.
				printf("NewObj Linearize: 0x%x, 0x%x, 0x%x, 0x%x\n", (unsigned int)ins->Arg1, (unsigned int)((IRMethod*)ins->Arg1)->Parameters, (unsigned int)((IRMethod*)ins->Arg1)->Parameters[0], (unsigned int)((IRMethod*)ins->Arg1)->Parameters[0]->Type);
				IRType* objType = (IRType*)ins->Arg2;
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(objType, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination.Type = SourceType_Local;
				ins->Destination.Data.LocalVariable.LocalVariableIndex = obj->LinearData.Data.LocalVariable.LocalVariableIndex;
				Push(obj);
                break;
			}
            case IROpcode_New_Array:
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(IRAssembly_MakeArrayType(pMethod->ParentAssembly, (IRType*)ins->Arg1), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				Push(obj);
                break;

			// 1 Source, 1 Destination Type in Arg1
            case IROpcode_IsInst:
            case IROpcode_CastClass:
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal((IRType*)ins->Arg1, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				Push(obj);
				break;
			// 1 Source, No Destination
            case IROpcode_Initialize_Object:
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
                break;
			// 2 Sources, No Destination
            case IROpcode_Copy_Object:
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				obj = Pop();
				ins->Source2 = obj->LinearData;
				PR(obj);
                break;
			// 3 Sources, No Destination
            case IROpcode_Initialize_Block:
            case IROpcode_Copy_Block:
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				obj = Pop();
				ins->Source2 = obj->LinearData;
				PR(obj);
				obj = Pop();
				ins->Source3 = obj->LinearData;
				PR(obj);
                break;
            case IROpcode_Convert_Checked:
            case IROpcode_Convert_Unchecked:
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, (ElementType)(uint32_t)ins->Arg2), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				Push(obj);
                break;
            case IROpcode_Call_Internal:
            case IROpcode_Call_Absolute:
			{
				IRMethod* mth = (IRMethod*)ins->Arg1;
				uint32_t paramCount = mth->ParameterCount;
				ins->SourceArray = (SourceTypeData*)calloc(1, sizeof(SourceTypeData) * paramCount);
				ins->SourceArrayLength = paramCount;
				for (uint32_t i2 = 0; i2 < paramCount; i2++)
				{
					obj = Pop();
					ins->SourceArray[i2].Type = obj->LinearData.Type;
					ins->SourceArray[i2].Data = obj->LinearData.Data;
					PR(obj);
				}
				if (!(mth->MethodDefinition->Flags & MethodAttributes_Static) && mth->MethodDefinition->TypeDefinition->GenericParameterCount)
				{
					IRType* parType = GetIRTypeOfSourceType(ins->SourceArray[ins->SourceArrayLength - 1].Type, ins->SourceArray[ins->SourceArrayLength - 1].Data, pMethod);
					if (!parType) Panic("Ooopsies");
					if (parType->IsGeneric)
					{
						printf("Resolving the implementation for %s.%s\n", parType->TypeDefinition->Namespace, parType->TypeDefinition->Name);
						bool_t found = FALSE;
						for (uint32_t i2 = 0; i2 < parType->MethodCount; i2++)
						{
							if (parType->Methods[i2]->MethodDefinition == mth->MethodDefinition)
							{
								mth = parType->Methods[i2];
								ins->Arg1 = mth;
								found = TRUE;
								break;
							}
						}
						if (!found)
							Panic("Unable to resolve the method on a generic type!");
					}
				}

				if (mth->Returns)
				{
					IRType* retType = mth->ReturnType;
					obj = PA();
					obj->LinearData.Type = SourceType_Local;
					obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(retType, pMethod, stack->StackDepth, &stackLocalTable);
					ins->Destination.Type = SourceType_Local;
					ins->Destination.Data.LocalVariable.LocalVariableIndex = obj->LinearData.Data.LocalVariable.LocalVariableIndex;
					Push(obj);
				}
                break;
			}
            case IROpcode_Call_Constrained:
            case IROpcode_Call_Virtual:
			{
				IRMethod* mth = ((IRType*)ins->Arg1)->Methods[(uint32_t)ins->Arg2];
				uint32_t paramCount = mth->ParameterCount;
				ins->SourceArray = (SourceTypeData*)calloc(1, sizeof(SourceTypeData) * paramCount);
				ins->SourceArrayLength = paramCount;
				for (uint32_t i2 = 0; i2 < paramCount; i2++)
				{
					obj = Pop();
					ins->SourceArray[i2].Type = obj->LinearData.Type;
					ins->SourceArray[i2].Data = obj->LinearData.Data;
					PR(obj);
				}

				if (mth->Returns)
				{
					IRType* retType = mth->ReturnType;
					obj = PA();
					obj->LinearData.Type = SourceType_Local;
					obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(retType, pMethod, stack->StackDepth, &stackLocalTable);
					ins->Destination.Type = SourceType_Local;
					ins->Destination.Data.LocalVariable.LocalVariableIndex = obj->LinearData.Data.LocalVariable.LocalVariableIndex;
					Push(obj);
				}
                break;
			}
            case IROpcode_CheckFinite:
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_R8), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				Push(obj);
                break;
            case IROpcode_Load_Token:
			{
				IRType* localType = 0;
				switch((RuntimeHandleType)(uint32_t)ins->Arg2)
				{
					case RuntimeHandleType_IRField:
						localType = pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_RuntimeFieldHandle->TableIndex - 1];
						break;
					case RuntimeHandleType_IRMethod:
						localType = pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_RuntimeMethodHandle->TableIndex - 1];
						break;
					case RuntimeHandleType_IRType:
						localType = pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_RuntimeTypeHandle->TableIndex - 1];
						break;
				}
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(localType, pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				Push(obj);
                break;
			}
            case IROpcode_Compare:
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				obj = Pop();
				ins->Source2 = obj->LinearData;
				PR(obj);
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I4), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				Push(obj);
                break;
			// 1 Source, 1 Destination of Native Int
            case IROpcode_Unbox:
            case IROpcode_Load_VirtualFunction:
            case IROpcode_Allocate_Local:
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				Push(obj);
                break;
            case IROpcode_Load_Function:
				obj = PA();
				obj->LinearData.Type = SourceType_Local;
				obj->LinearData.Data.LocalVariable.LocalVariableIndex = AddLocal(AppDomain_GetIRTypeFromElementType(pMethod->ParentAssembly->ParentDomain, ElementType_I), pMethod, stack->StackDepth, &stackLocalTable);
				ins->Destination = obj->LinearData;
				Push(obj);
                break;
            case IROpcode_Switch:
			{
				obj = Pop();
				ins->Source1 = obj->LinearData;
				PR(obj);
				break;
			}
            case IROpcode_Branch:
			{
				switch((BranchCondition)(uint32_t)ins->Arg1)
				{
					case BranchCondition_Always:
						if (stack->StackDepth > 0)
						{
							PR(Pop());
						}
						break;
					case BranchCondition_False:
					case BranchCondition_True:
						obj = Pop();
						ins->Source1 = obj->LinearData;
						PR(obj);
						break;
					case BranchCondition_Greater:
					case BranchCondition_Greater_Or_Equal:
					case BranchCondition_Greater_Or_Equal_Unsigned:
					case BranchCondition_Greater_Unsigned:
					case BranchCondition_Less:
					case BranchCondition_Less_Or_Equal:
					case BranchCondition_Less_Or_Equal_Unsigned:
					case BranchCondition_Less_Unsigned:
					case BranchCondition_Equal:
					case BranchCondition_NotEqual_Unsigned:
						obj = Pop();
						ins->Source1 = obj->LinearData;
						PR(obj);
						obj = Pop();
						ins->Source2 = obj->LinearData;
						PR(obj);
						break;
				}
				break;
			}

            case IROpcode_UnboxAny:
            case IROpcode_Box:
            case IROpcode_MkRefAny:
            case IROpcode_RefAnyVal:
            case IROpcode_RefAnyType:
				Panic("Not yet supported!");
                break;
				
			// These op-codes do nothing
			// in terms of stack analysis
			// or linearization of the stack.
            case IROpcode_Jump:
            case IROpcode_Nop:
            case IROpcode_Break:
                break;

			// These op-codes shouldn't
			// exist yet, because they
			// are only created in this
			// function or later.
			case IROpcode_Move:
            case IROpcode_Call_Intrinsic:
				Panic("This shouldn't exist yet!");
				break;

			// There is no default here for good reason/
        }
	}

	StackObjectPool_Destroy(stack);
	SyntheticStack_Destroy(stack);
}

