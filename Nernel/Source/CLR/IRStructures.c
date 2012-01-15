#include <CLR/IRStructures.h>
#include <CLR/Architecture.h>
#include <stdlib.h>


IRAssembly* IRAssembly_Create()
{
    IRAssembly* asmb = (IRAssembly*)calloc(1, sizeof(IRAssembly));
    return asmb;
}

IRMethod* IRMethod_Create()
{
    IRMethod* mth = (IRMethod*)calloc(1, sizeof(IRMethod));
    return mth;
}

IRInstruction* IRInstruction_Create()
{
    IRInstruction* instr = (IRInstruction*)calloc(1, sizeof(IRInstruction));
    instr->Arg1NeedsDisposing = TRUE;
    instr->Arg2NeedsDisposing = TRUE;
    instr->Arg3NeedsDisposing = TRUE;
    return instr;
}

IRParameter* IRParameter_Create()
{
	IRParameter* param = (IRParameter*)calloc(1, sizeof(IRParameter));
	return param;
}

IRType* IRType_Create()
{
	IRType* tp = (IRType*)calloc(1, sizeof(IRType));
	return tp;
}

IRLocalVariable* IRLocalVariable_Create()
{
	IRLocalVariable* var = (IRLocalVariable*)calloc(1, sizeof(IRLocalVariable));
	return var;
}

IRField* IRField_Create()
{
	IRField* fld = (IRField*)calloc(1, sizeof(IRField));
	return fld;
}





void IRAssembly_Destroy(IRAssembly* assembly)
{
	for (uint32_t i = 0; i < assembly->MethodCount; i++)
	{
        IRMethod_Destroy(assembly->Methods[i]);
	}
	free(assembly->Methods);
    free(assembly);
}

void IRMethod_Destroy(IRMethod* method)
{
    if (method->AssembledMethod)
        free(method->AssembledMethod);
    for (uint32_t i = 0; i < method->IRCodesCount; i++)
    {
        IRInstruction_Destroy(method->IRCodes[i]);
    }
	for (uint32_t i = 0; i < method->LocalVariableCount; i++)
    {
        IRLocalVariable_Destroy(method->LocalVariables[i]);
    }
	for (uint32_t i = 0; i < method->ParameterCount; i++)
    {
        IRParameter_Destroy(method->Parameters[i]);
    }
    if (method->IRCodes)
        free(method->IRCodes);
    if (method->LocalVariables)
        free(method->LocalVariables);
    if (method->Parameters)
        free(method->Parameters);
    free(method);
}

void IRInstruction_Destroy(IRInstruction* instr)
{
    if (instr->Arg1NeedsDisposing && instr->Arg1)
    {
        free(instr->Arg1);
    }
    if (instr->Arg2NeedsDisposing && instr->Arg2)
    {
        free(instr->Arg2);
    }
    if (instr->Arg3NeedsDisposing && instr->Arg3)
    {
        free(instr->Arg3);
    }
    free(instr);
}

void IRParameter_Destroy(IRParameter* param)
{
	free(param);
}

void IRType_Destroy(IRType* tp)
{
	if (tp->Fields)
		free(tp->Fields);
	free(tp);
}

void IRLocalVariable_Destroy(IRLocalVariable* var)
{
	free(var);
}

void IRField_Destroy(IRField* fld)
{
	free(fld);
}





void IRAssembly_AddMethod(IRAssembly* asmb, IRMethod* mth)
{
	asmb->MethodCount++;
	mth->MethodIndex = asmb->MethodCount;
	// The reason this is safe to do even if
	// we haven't allocated for it yet, is because
	// realloc acts like a normal malloc if the first
	// argument is NULL.
	asmb->Methods = (IRMethod**)realloc(asmb->Methods, sizeof(IRMethod*) * asmb->MethodCount);
	asmb->Methods[asmb->MethodCount - 1] = mth;
}

void IRAssembly_AddType(IRAssembly* asmb, IRType* tp)
{
	asmb->TypeCount++;
	tp->TypeIndex = asmb->TypeCount;
	asmb->Types = (IRType**)realloc(asmb->Types, sizeof(IRType*) * asmb->TypeCount);
	asmb->Types[asmb->TypeCount - 1] = tp;
}

void IRAssembly_AddField(IRAssembly* asmb, IRField* fld)
{
	asmb->FieldCount++;
	fld->FieldIndex = asmb->FieldCount;
	asmb->Fields = (IRField**)realloc(asmb->Fields, sizeof(IRField*) * asmb->FieldCount);
	asmb->Fields[asmb->FieldCount - 1] = fld;
}






void IRMethod_AddInstruction(IRMethod* mth, IRInstruction* instr)
{
	mth->IRCodesCount++;
	mth->IRCodes = (IRInstruction**)realloc(mth->IRCodes, sizeof(IRInstruction*) * mth->IRCodesCount);
	mth->IRCodes[mth->IRCodesCount - 1] = instr;
}

void IRMethod_AddLocalVariable(IRMethod* mth, IRLocalVariable* var)
{
	var->LocalVariableIndex = mth->LocalVariableCount;
	mth->LocalVariableCount++;
	mth->LocalVariables = (IRLocalVariable**)realloc(mth->LocalVariables, sizeof(IRLocalVariable*) * mth->LocalVariableCount);
	mth->LocalVariables[mth->LocalVariableCount - 1] = var;
}

void IRMethod_AddParameter(IRMethod* mth, IRParameter* param)
{
	param->ParameterIndex = mth->ParameterCount;
	mth->ParameterCount++;
	mth->Parameters = (IRParameter**)realloc(mth->Parameters, sizeof(IRParameter*) * mth->ParameterCount);
	mth->Parameters[mth->ParameterCount - 1] = param;
}






uint32_t IRType_GetSize(IRType* tp)
{
	AppDomain* domain = tp->ParentAssembly->ParentDomain;
	if (
		(tp->TypeDef == domain->CachedType___System_Byte)
	||  (tp->TypeDef == domain->CachedType___System_SByte)
	||  (tp->TypeDef == domain->CachedType___System_Boolean)
	)
	{
		return 1;
	}
	else if (
		(tp->TypeDef == domain->CachedType___System_Int16)
	||  (tp->TypeDef == domain->CachedType___System_UInt16)
	||  (tp->TypeDef == domain->CachedType___System_Char)
	)
	{
		return 2;
	}
	else if (
		(tp->TypeDef == domain->CachedType___System_Int32)
	||  (tp->TypeDef == domain->CachedType___System_UInt32)
	||  (tp->TypeDef == domain->CachedType___System_Object)
	||  (tp->TypeDef == domain->CachedType___System_Single)
	)
	{
		return 4;
	}
	else if (
		(tp->TypeDef == domain->CachedType___System_Int64)
	||  (tp->TypeDef == domain->CachedType___System_UInt64)
	||  (tp->TypeDef == domain->CachedType___System_Double)
	)
	{
		return 8;
	}
	else if (
		(tp->TypeDef == domain->CachedType___System_IntPtr)
	||  (tp->TypeDef == domain->CachedType___System_UIntPtr)
	)
	{
		return global_SizeOfPointerInBytes;
	}
	else
	{
		uint32_t size = 0;
		for (uint32_t i2 = 1; i2 <= tp->FieldCount; i2++)
		{
			size += IRType_GetSize(tp->Fields[i2]->FieldType);
		}
		return size;
	}
}