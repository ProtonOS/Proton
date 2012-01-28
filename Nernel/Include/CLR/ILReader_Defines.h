#pragma once


#define ClearFlags() \
    { Constrained = FALSE; \
    No = FALSE; \
    ReadOnly = FALSE; \
    Tail = FALSE; \
    UnAligned = FALSE; \
    Volatile = FALSE; }


#define DefineUnSupportedOpCode(ilOpCode) \
	{ Panic("Encountered unsupported Op-Code '" #ilOpCode "'! Cannot continue!"); \
	break; }


#define GetInstrOffset() \
    if (Constrained) { instr->InstructionLocation -= 2; } \
    if (No) { instr->InstructionLocation -= 2; } \
    if (ReadOnly) { instr->InstructionLocation -= 2; } \
    if (Tail) { instr->InstructionLocation -= 2; } \
    if (UnAligned) { instr->InstructionLocation -= 2; } \
    if (Volatile) { instr->InstructionLocation -= 2; } \



#define EMIT_IR(instrType) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->InstructionLocation = (CurInstructionBase - orig); \
    Log_WriteLine(LogFlags_ILReading_BranchLinker, "instr->InstructionLocation = 0x%x", (unsigned int)instr->InstructionLocation); \
    GetInstrOffset(); \
    instr->OpCode = instrType; \
    Log_WriteLine(LogFlags_IREmitting, "Emitting " #instrType); \
    IRMethod_AddInstruction(m, instr); }
#define EMIT_IR_1ARG(instrType, arg1) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->InstructionLocation = (CurInstructionBase - orig); \
    Log_WriteLine(LogFlags_ILReading_BranchLinker, "instr->InstructionLocation = 0x%x", (unsigned int)instr->InstructionLocation); \
    GetInstrOffset(); \
    instr->Arg1 = arg1; \
    instr->OpCode = instrType; \
    Log_WriteLine(LogFlags_IREmitting, "Emitting " #instrType " With 1 argument ('" #arg1 "')"); \
    IRMethod_AddInstruction(m, instr); }
#define EMIT_IR_2ARG(instrType, arg1, arg2) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->InstructionLocation = (CurInstructionBase - orig); \
    Log_WriteLine(LogFlags_ILReading_BranchLinker, "instr->InstructionLocation = 0x%x", (unsigned int)instr->InstructionLocation); \
    GetInstrOffset(); \
    instr->Arg1 = arg1; \
    instr->Arg2 = arg2; \
    instr->OpCode = instrType; \
    Log_WriteLine(LogFlags_IREmitting, "Emitting " #instrType " With 2 arguments ('" #arg1 "', '" #arg2 "')"); \
    IRMethod_AddInstruction(m, instr); }
#define EMIT_IR_3ARG(instrType, arg1, arg2, arg3) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->InstructionLocation = (CurInstructionBase - orig); \
    Log_WriteLine(LogFlags_ILReading_BranchLinker, "instr->InstructionLocation = 0x%x", (unsigned int)instr->InstructionLocation); \
    GetInstrOffset(); \
    instr->Arg1 = arg1; \
    instr->Arg2 = arg2; \
    instr->Arg3 = arg3; \
    instr->OpCode = instrType; \
    Log_WriteLine(LogFlags_IREmitting, "Emitting " #instrType " With 3 arguments ('" #arg1 "', '" #arg2 "', '" #arg3 "')"); \
    IRMethod_AddInstruction(m, instr); }
#define EMIT_IR_4ARG(instrType, arg1, arg2, arg3, arg4) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->InstructionLocation = (CurInstructionBase - orig); \
    Log_WriteLine(LogFlags_ILReading_BranchLinker, "instr->InstructionLocation = 0x%x", (unsigned int)instr->InstructionLocation); \
    GetInstrOffset(); \
    instr->Arg1 = arg1; \
    instr->Arg2 = arg2; \
    instr->Arg3 = arg3; \
    instr->Arg4 = arg4; \
    instr->OpCode = instrType; \
	Log_WriteLine(LogFlags_IREmitting, "Emitting " #instrType " With 4 arguments ('" #arg1 "', '" #arg2 "', '" #arg3 "', '" #arg4 "')"); \
    IRMethod_AddInstruction(m, instr); }

#define EMIT_IR_1ARG_NO_DISPOSE(instrType, arg1) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->InstructionLocation = (CurInstructionBase - orig); \
    Log_WriteLine(LogFlags_ILReading_BranchLinker, "instr->InstructionLocation = 0x%x", (unsigned int)instr->InstructionLocation); \
    GetInstrOffset(); \
    instr->Arg1 = arg1; \
	instr->Arg1NeedsDisposing = FALSE; \
    instr->OpCode = instrType; \
    Log_WriteLine(LogFlags_IREmitting, "Emitting " #instrType " With 1 argument ('" #arg1 "') and not disposing of it"); \
    IRMethod_AddInstruction(m, instr); }




#define DEFINE_CONV_UNCHECKED(conArgType, sObjType, sObjNumType) \
    { Log_WriteLine(LogFlags_ILReading, "Read Conv."#conArgType); \
    ConversionArgumentType* sType = (ConversionArgumentType*)malloc(sizeof(ConversionArgumentType)); \
    \
    StackObject* obj = SyntheticStack_Pop(stack); \
    switch(obj->NumericType) \
    { \
        case StackObjectNumericType_Int8: \
            *sType = ConversionArgumentType_I1; \
            break; \
        case StackObjectNumericType_UInt8: \
            *sType = ConversionArgumentType_U1; \
            break; \
        case StackObjectNumericType_Int16: \
            *sType = ConversionArgumentType_I2; \
            break; \
        case StackObjectNumericType_UInt16: \
            *sType = ConversionArgumentType_U2; \
            break; \
        case StackObjectNumericType_Int32: \
            *sType = ConversionArgumentType_I4; \
            break; \
        case StackObjectNumericType_UInt32: \
            *sType = ConversionArgumentType_U4; \
            break; \
        case StackObjectNumericType_Int64: \
            *sType = ConversionArgumentType_I8; \
            break; \
        case StackObjectNumericType_UInt64: \
            *sType = ConversionArgumentType_U8; \
            break; \
        case StackObjectNumericType_Float32: \
            *sType = ConversionArgumentType_R4; \
            break; \
        case StackObjectNumericType_Float64: \
            *sType = ConversionArgumentType_R8; \
            break; \
		case StackObjectNumericType_UPointer: \
            *sType = ConversionArgumentType_U; \
            break; \
		case StackObjectNumericType_Pointer: \
            *sType = ConversionArgumentType_I; \
            break; \
    \
        default: \
            Panic("Invalid argument for conversion"); \
            break; \
    } \
    ConversionArgumentType* tType = (ConversionArgumentType*)malloc(sizeof(ConversionArgumentType)); \
    *tType = ConversionArgumentType_##conArgType; \
    \
    StackObject* obj2 = StackObjectPool_Allocate(); \
    obj2->Type = StackObjectType_##sObjType; \
    obj2->NumericType = StackObjectNumericType_##sObjNumType; \
    StackObjectPool_Release(obj); \
    SyntheticStack_Push(stack, obj2); \
    EMIT_IR_2ARG(IROpCode_Convert_Unchecked, sType, tType); \
    ClearFlags(); \
    break; }



#define DEFINE_CONV_OVF_UN(conArgType, sObjType, sObjNumType) \
    { Log_WriteLine(LogFlags_ILReading, "Read Conv.Ovf."#conArgType".Un"); \
    ConversionArgumentType* sType = (ConversionArgumentType*)malloc(sizeof(ConversionArgumentType)); \
    \
    StackObject* obj = SyntheticStack_Pop(stack); \
    switch(obj->NumericType) \
    { \
        case StackObjectNumericType_UInt8: \
            *sType = ConversionArgumentType_U1; \
            break; \
        case StackObjectNumericType_UInt16: \
            *sType = ConversionArgumentType_U2; \
            break; \
        case StackObjectNumericType_UInt32: \
            *sType = ConversionArgumentType_U4; \
            break; \
        case StackObjectNumericType_UInt64: \
            *sType = ConversionArgumentType_U8; \
            break; \
		case StackObjectNumericType_UPointer: \
            *sType = ConversionArgumentType_U; \
            break; \
    \
        default: \
            Panic("Invalid argument for conversion"); \
            break; \
    } \
    ConversionArgumentType* tType = (ConversionArgumentType*)malloc(sizeof(ConversionArgumentType)); \
    *tType = ConversionArgumentType_##conArgType; \
    \
    StackObject* obj2 = StackObjectPool_Allocate(); \
    obj2->Type = StackObjectType_##sObjType; \
    obj2->NumericType = StackObjectNumericType_##sObjNumType; \
    StackObjectPool_Release(obj); \
    SyntheticStack_Push(stack, obj2); \
    EMIT_IR_2ARG(IROpCode_Convert_OverflowCheck, sType, tType); \
    ClearFlags(); \
    break; }
#define DEFINE_CONV_OVF(conArgType, sObjType, sObjNumType) \
    { Log_WriteLine(LogFlags_ILReading, "Read Conv.Ovf."#conArgType); \
    ConversionArgumentType* sType = (ConversionArgumentType*)malloc(sizeof(ConversionArgumentType)); \
    \
    StackObject* obj = SyntheticStack_Pop(stack); \
    switch(obj->NumericType) \
    { \
        case StackObjectNumericType_Int8: \
            *sType = ConversionArgumentType_I1; \
            break; \
        case StackObjectNumericType_Int16: \
            *sType = ConversionArgumentType_I2; \
            break; \
        case StackObjectNumericType_Int32: \
            *sType = ConversionArgumentType_I4; \
            break; \
        case StackObjectNumericType_Int64: \
            *sType = ConversionArgumentType_I8; \
            break; \
        case StackObjectNumericType_Float32: \
            *sType = ConversionArgumentType_R4; \
            break; \
        case StackObjectNumericType_Float64: \
            *sType = ConversionArgumentType_R8; \
            break; \
		case StackObjectNumericType_Pointer: \
            *sType = ConversionArgumentType_I; \
            break; \
    \
        default: \
            Panic("Invalid argument for conversion"); \
            break; \
    } \
    ConversionArgumentType* tType = (ConversionArgumentType*)malloc(sizeof(ConversionArgumentType)); \
    *tType = ConversionArgumentType_##conArgType; \
    \
    StackObject* obj2 = StackObjectPool_Allocate(); \
    obj2->Type = StackObjectType_##sObjType; \
    obj2->NumericType = StackObjectNumericType_##sObjNumType; \
    StackObjectPool_Release(obj); \
    SyntheticStack_Push(stack, obj2); \
    EMIT_IR_2ARG(IROpCode_Convert_OverflowCheck, sType, tType); \
    ClearFlags(); \
    break; }



#define DefineBranchTarget(brnchType, opCode) \
    Log_WriteLine(LogFlags_ILReading, "Read " #opCode ); \
	/* Branch conditions false & true only pop once
     * from the top of the stack.
	 */ \
	if (brnchType == BranchCondition_False || brnchType == BranchCondition_True) \
	{ \
		branch_Arg1 = SyntheticStack_Pop(stack);\
	} \
	/* Branch conditions other than false, true,
	 * and always, pop twice from the top of the
	 * stack.
	 */ \
	else if (brnchType != BranchCondition_Always) \
	{ \
		branch_Arg1 = SyntheticStack_Pop(stack);\
		branch_Arg2 = SyntheticStack_Pop(stack);\
	} \
    branch_Condition = brnchType; \
    branch_Target = (uint32_t)ReadUInt32(dat); \
    goto Branch_Common; 
#define DefineBranchTarget_Short(brnchType, opCode) \
    Log_WriteLine(LogFlags_ILReading, "Read " #opCode ".S"); \
	/* Branch conditions false & true only pop once
     * from the top of the stack.
	 */ \
	if (brnchType == BranchCondition_False || brnchType == BranchCondition_True) \
	{ \
		branch_Arg1 = SyntheticStack_Pop(stack);\
	} \
	/* Branch conditions other than false, true,
	 * and always, pop twice from the top of the
	 * stack.
	 */ \
	else if (brnchType != BranchCondition_Always) \
	{ \
		branch_Arg1 = SyntheticStack_Pop(stack);\
		branch_Arg2 = SyntheticStack_Pop(stack);\
	} \
    branch_Condition = brnchType; \
    branch_Target = (uint32_t)((int32_t)((int8_t)ReadUInt8(dat))); \
    goto Branch_Common; 


#define DefineStLoc(indx) \
	{ Log_WriteLine(LogFlags_ILReading, "Read StLoc." #indx); \
	uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t)); \
	*dt = (uint32_t)indx; \
	EMIT_IR_1ARG(IROpCode_Store_LocalVar, dt); \
	StackObjectPool_Release(SyntheticStack_Pop(stack)); \
	ClearFlags(); \
	break; }

#define DefineLdcI4(nmb) \
	{ Log_WriteLine(LogFlags_ILReading, "Read Ldc.I4." #nmb); \
	uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t)); \
	*dt = (uint32_t)nmb; \
	EMIT_IR_1ARG(IROpCode_LoadInt32_Val, dt); \
	StackObject* s = StackObjectPool_Allocate(); \
	s->Type = StackObjectType_Int32; \
	s->NumericType = StackObjectNumericType_Int32; \
	SyntheticStack_Push(stack, s); \
	ClearFlags(); \
	break; }

#define DefineShift(SftType, opCode) \
	{ Log_WriteLine(LogFlags_ILReading, "Read " #opCode); \
	ShiftType* sType = (ShiftType*)malloc(sizeof(ShiftType)); \
	*sType = ShiftType_##SftType; \
	StackObjectType* sValType = (StackObjectType*)malloc(sizeof(StackObjectType)); \
	StackObjectType* sAmntType = (StackObjectType*)malloc(sizeof(StackObjectType)); \
	StackObject* obj = SyntheticStack_Pop(stack); /* pop the amount to shift by */ \
	/* We use a switch statement rather than directly assigning it, \
	   so that we have a form of error checking in place. */ \
	switch(obj->Type) \
	{ \
		case StackObjectType_Int32: \
			*sAmntType = StackObjectType_Int32; \
			break; \
		case StackObjectType_NativeInt: \
			*sAmntType = StackObjectType_NativeInt; \
			break; \
		default: \
			Panic("Invalid stack object type on the top of the stack!"); \
			break; \
	} \
	StackObjectPool_Release(obj); \
	obj = SyntheticStack_Pop(stack); /* pop the value to shift */ \
	switch(obj->Type) \
	{ \
		case StackObjectType_Int32: \
			*sValType = StackObjectType_Int32; \
			break; \
		case StackObjectType_Int64: \
			*sValType = StackObjectType_Int64; \
			break; \
		case StackObjectType_NativeInt: \
			*sValType = StackObjectType_NativeInt; \
			break; \
		default: \
			Panic("Invalid stack object type on the top of the stack!"); \
			break; \
	} \
	StackObjectPool_Release(obj); \
	obj = StackObjectPool_Allocate(); \
	obj->Type = *sValType; \
	switch (obj->Type) \
	{ \
		case StackObjectType_Int32: \
			obj->NumericType = StackObjectNumericType_Int32; \
			break; \
		case StackObjectType_Int64: \
			obj->NumericType = StackObjectNumericType_Int64; \
			break; \
		case StackObjectType_NativeInt: \
			obj->NumericType = StackObjectNumericType_Pointer; \
			break; \
		default: \
			Panic("Something went very very wrong, because this point should never be reached!"); \
			break; \
	} \
	SyntheticStack_Push(stack, obj); \
	EMIT_IR_3ARG(IROpCode_Shift, sType, sValType, sAmntType); \
	ClearFlags(); \
	break; }

#define DefineLdElem(elemType, sObjType, sObjNumType) \
	{ Log_WriteLine(LogFlags_ILReading, "Read LdElem." #elemType); \
	StackObjectType* indxType = (StackObjectType*)malloc(sizeof(StackObjectType)); \
	StackObject* obj = SyntheticStack_Pop(stack); \
	switch(obj->Type) \
	{ \
		case StackObjectType_Int32: \
			*indxType = StackObjectType_Int32; \
			break; \
		case StackObjectType_NativeInt: \
			*indxType = StackObjectType_NativeInt; \
			break; \
		default: \
			Panic("Invalid index type!"); \
			break; \
	} \
	StackObjectPool_Release(obj); \
	obj = NULL; \
	StackObjectPool_Release(SyntheticStack_Pop(stack)); /* Pop the array off the top of the stack. */ \
	ElementType* elmntType = (ElementType*)malloc(sizeof(ElementType)); \
	*elmntType = ElementType_##elemType; \
	EMIT_IR_2ARG(IROpCode_Load_Element, elmntType, indxType); \
	\
	obj = StackObjectPool_Allocate(); \
	obj->Type = StackObjectType_##sObjType; \
	obj->NumericType = StackObjectNumericType_##sObjNumType; \
	SyntheticStack_Push(stack, obj); \
	ClearFlags(); \
	break; }

#define DefineStElem(elemType) \
	{ Log_WriteLine(LogFlags_ILReading, "Read StElem." #elemType); \
	StackObjectPool_Release(SyntheticStack_Pop(stack)); /* Pop the value off the top of the stack. */ \
	StackObjectType* indxType = (StackObjectType*)malloc(sizeof(StackObjectType)); \
	StackObject* obj = SyntheticStack_Pop(stack); \
	switch(obj->Type) \
	{ \
		case StackObjectType_Int32: \
			*indxType = StackObjectType_Int32; \
			break; \
		case StackObjectType_NativeInt: \
			*indxType = StackObjectType_NativeInt; \
			break; \
		default: \
			Panic("Invalid index type!"); \
			break; \
	} \
	StackObjectPool_Release(obj); \
	obj = NULL; \
	StackObjectPool_Release(SyntheticStack_Pop(stack)); /* Pop the array off the top of the stack. */ \
	ElementType* elmntType = (ElementType*)malloc(sizeof(ElementType)); \
	*elmntType = ElementType_##elemType; \
	EMIT_IR_2ARG(IROpCode_Store_Element, elmntType, indxType); \
	ClearFlags(); \
	break; }

#define SetTypeOfStackObjectFromSigElementType(obj, elType) \
	switch (elType) \
	{ \
		case Signature_ElementType_Boolean: \
		case Signature_ElementType_I1: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type I1"); \
			obj->NumericType = StackObjectNumericType_Int8; \
			obj->Type = StackObjectType_Int32; \
			break; \
		case Signature_ElementType_I2: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type I2"); \
			obj->NumericType = StackObjectNumericType_Int16; \
			obj->Type = StackObjectType_Int32; \
			break; \
		case Signature_ElementType_I4: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type I4"); \
			obj->NumericType = StackObjectNumericType_Int32; \
			obj->Type = StackObjectType_Int32; \
			break; \
		case Signature_ElementType_I8: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type I8"); \
			obj->NumericType = StackObjectNumericType_Int64; \
			obj->Type = StackObjectType_Int64; \
			break; \
		case Signature_ElementType_U1: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type U1"); \
			obj->NumericType = StackObjectNumericType_UInt8; \
			obj->Type = StackObjectType_Int32; \
			break; \
		case Signature_ElementType_Char: \
		case Signature_ElementType_U2: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type U2"); \
			obj->NumericType = StackObjectNumericType_UInt16; \
			obj->Type = StackObjectType_Int32; \
			break; \
		case Signature_ElementType_U4: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type U4"); \
			obj->NumericType = StackObjectNumericType_UInt32; \
			obj->Type = StackObjectType_Int32; \
			break; \
		case Signature_ElementType_U8: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type U8"); \
			obj->NumericType = StackObjectNumericType_UInt64; \
			obj->Type = StackObjectType_Int64; \
			break; \
		case Signature_ElementType_R4: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type R4"); \
			obj->NumericType = StackObjectNumericType_Float32; \
			obj->Type = StackObjectType_Float; \
			break; \
		case Signature_ElementType_R8: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type R8"); \
			obj->NumericType = StackObjectNumericType_Float64; \
			obj->Type = StackObjectType_Float; \
			break; \
		case Signature_ElementType_IPointer: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type IPointer"); \
			obj->NumericType = StackObjectNumericType_Pointer; \
			obj->Type = StackObjectType_NativeInt; \
			break; \
		case Signature_ElementType_UPointer: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type UPointer"); \
			obj->NumericType = StackObjectNumericType_UPointer; \
			obj->Type = StackObjectType_NativeInt; \
			break; \
		case Signature_ElementType_ValueType: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type ValueType"); \
			obj->NumericType = StackObjectNumericType_DataType; \
			obj->Type = StackObjectType_DataType; \
			break; \
		case Signature_ElementType_Object: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type Object"); \
		case Signature_ElementType_Array: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type Array"); \
		case Signature_ElementType_SingleDimensionArray: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type SingleDimArray"); \
		case Signature_ElementType_String: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type String"); \
		case Signature_ElementType_ByReference: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type ReferenceType"); \
		case Signature_ElementType_Class: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type Class"); \
			obj->NumericType = StackObjectNumericType_Ref; \
			obj->Type = StackObjectType_ReferenceType; \
			break; \
		case Signature_ElementType_GenericInstantiation: \
			Log_WriteLine(LogFlags_ILReading_ElementTypes, "Element Type GenericInstantation"); \
			obj->NumericType = StackObjectNumericType_Ref; \
			obj->Type = StackObjectType_ReferenceType; \
			break; \
		default: \
			Panic(String_Format("Unknown Element Type '0x%x'!", (unsigned int)(elType))); \
			break; \
	} 


#define GetElementTypeOfStackObject(dest, stkObj) \
	switch (stkObj->NumericType) \
	{ \
		case StackObjectNumericType_UInt8: \
			dest = ElementType_U1; \
			break; \
		case StackObjectNumericType_UInt16: \
			dest = ElementType_U2; \
			break; \
		case StackObjectNumericType_UInt32: \
			dest = ElementType_U4; \
			break; \
		case StackObjectNumericType_UInt64: \
			dest = ElementType_U8; \
			break; \
		case StackObjectNumericType_Int8: \
			dest = ElementType_I1; \
			break; \
		case StackObjectNumericType_Int16: \
			dest = ElementType_I2; \
			break; \
		case StackObjectNumericType_Int32: \
			dest = ElementType_I4; \
			break; \
		case StackObjectNumericType_Int64: \
			dest = ElementType_I8; \
			break; \
		case StackObjectNumericType_Float32: \
			dest = ElementType_R4; \
			break; \
		case StackObjectNumericType_Float64: \
			dest = ElementType_R8; \
			break; \
		case StackObjectNumericType_Pointer: \
			dest = ElementType_I; \
			break; \
		case StackObjectNumericType_UPointer: \
			dest = ElementType_U; \
			break; \
		case StackObjectNumericType_DataType: \
			dest = ElementType_DataType; \
			break; \
		case StackObjectNumericType_Ref: \
			dest = ElementType_Ref; \
			break; \
		case StackObjectNumericType_ManagedPointer: \
			dest = ElementType_ManagedPointer; \
			break; \
		default: \
			Panic("Unknown StackObjectNumericType!"); \
			break; \
	} 



#define BinaryNumericOp_Add 0
#define BinaryNumericOp_Sub 1
#define BinaryNumericOp_Div 2
#define BinaryNumericOp_Mul 3
#define BinaryNumericOp_Rem 4

#define CheckBinaryNumericOpOperandTypesAndSetResult(OperandA, OperandB, BinaryNumericOp, ResultObject) \
	Log_WriteLine(LogFlags_ILReading_ElementTypes, "Operand A: 0x%x", (unsigned int)OperandA); \
	Log_WriteLine(LogFlags_ILReading_ElementTypes, "Operand B: 0x%x", (unsigned int)OperandB); \
	switch(OperandA) \
	{ \
		case ElementType_U1: \
		case ElementType_U2: \
		case ElementType_U4: \
		case ElementType_I1: \
		case ElementType_I2: \
		case ElementType_I4: \
		{ \
			if (BinaryNumericOp == BinaryNumericOp_Add) \
			{ \
				if (OperandB == ElementType_ManagedPointer) \
				{ \
					ResultObject->Type = StackObjectType_ManagedPointer; \
					ResultObject->NumericType = StackObjectNumericType_ManagedPointer; \
					break; \
				} \
			} \
			switch(OperandB) \
			{ \
				case ElementType_I: \
				case ElementType_U: \
					ResultObject->Type = StackObjectType_NativeInt; \
					ResultObject->NumericType = StackObjectNumericType_Pointer; \
					break; \
				case ElementType_U1: \
				case ElementType_U2: \
				case ElementType_U4: \
				case ElementType_I1: \
				case ElementType_I2: \
				case ElementType_I4: \
					ResultObject->Type = StackObjectType_Int32; \
					ResultObject->NumericType = StackObjectNumericType_Int32; \
					break; \
				default: \
					Panic("Invalid Operands for Binary Numeric Operation!"); \
					break; \
			} \
			break; \
		} \
		\
		case ElementType_I8: \
		case ElementType_U8: \
		{ \
			switch(OperandB) \
			{ \
				case ElementType_I8: \
				case ElementType_U8: \
					ResultObject->Type = StackObjectType_Int64; \
					ResultObject->NumericType = StackObjectNumericType_Int64; \
					break; \
				default: \
					Panic("Invalid Operands for Binary Numeric Operation!"); \
					break; \
			} \
			break; \
		} \
		\
		case ElementType_R4: \
		case ElementType_R8: \
		{ \
			switch(OperandB) \
			{ \
				case ElementType_R4: \
				case ElementType_R8: \
					ResultObject->Type = StackObjectType_Float; \
					ResultObject->NumericType = StackObjectNumericType_Float64; \
					break; \
				default: \
					Panic("Invalid Operands for Binary Numeric Operation!"); \
					break; \
			} \
			break; \
		} \
		\
		case ElementType_I: \
		case ElementType_U: \
		{ \
			if (BinaryNumericOp == BinaryNumericOp_Add) \
			{ \
				if (OperandB == ElementType_ManagedPointer) \
				{ \
					ResultObject->Type = StackObjectType_ManagedPointer; \
					ResultObject->NumericType = StackObjectNumericType_ManagedPointer; \
					break; \
				} \
			} \
			switch(OperandB) \
			{ \
				case ElementType_I: \
				case ElementType_U: \
				case ElementType_U1: \
				case ElementType_U2: \
				case ElementType_U4: \
				case ElementType_I1: \
				case ElementType_I2: \
				case ElementType_I4: \
					ResultObject->Type = StackObjectType_NativeInt; \
					ResultObject->NumericType = StackObjectNumericType_Pointer; \
					break; \
				default: \
					Panic("Invalid Operands for Binary Numeric Operation!"); \
					break; \
			} \
			break; \
		} \
		\
		case ElementType_ManagedPointer: \
		{ \
			if (BinaryNumericOp == BinaryNumericOp_Add || BinaryNumericOp == BinaryNumericOp_Sub) \
			{ \
				if (BinaryNumericOp == BinaryNumericOp_Sub) \
				{ \
					if (OperandB == ElementType_ManagedPointer) \
					{ \
						ResultObject->Type = StackObjectType_NativeInt; \
						ResultObject->NumericType = StackObjectNumericType_Pointer; \
						break; \
					} \
				} \
				switch(OperandB) \
				{ \
					case ElementType_I: \
					case ElementType_U: \
					case ElementType_U1: \
					case ElementType_U2: \
					case ElementType_U4: \
					case ElementType_I1: \
					case ElementType_I2: \
					case ElementType_I4: \
						ResultObject->Type = StackObjectType_ManagedPointer; \
						ResultObject->NumericType = StackObjectNumericType_ManagedPointer; \
						break; \
					default: \
						Panic("Invalid Operands for Binary Numeric Operation!"); \
						break; \
				} \
			} \
			else \
			{ \
				Panic("Invalid Operands for Binary Numeric Operation!"); \
			} \
			break; \
		} \
		\
		default: \
			Panic("Invalid Operands for Addition!"); \
			break; \
	}

#define DefineStInd(elmnType) \
	{ Log_WriteLine(LogFlags_ILReading, "Read StInd." #elmnType); \
	StackObjectPool_Release(SyntheticStack_Pop(stack)); \
	StackObjectPool_Release(SyntheticStack_Pop(stack)); \
	ElementType* emnTp = (ElementType*)malloc(sizeof(ElementType)); \
	*emnTp = ElementType_##elmnType; \
	EMIT_IR_1ARG(IROpCode_StoreIndirect, emnTp); \
	ClearFlags(); \
	break; }

#define DefineLdInd(elmnType) \
	{ Log_WriteLine(LogFlags_ILReading, "Read LdInd." #elmnType); \
	StackObjectPool_Release(SyntheticStack_Pop(stack)); \
	ElementType* emnTp = (ElementType*)malloc(sizeof(ElementType)); \
	*emnTp = ElementType_##elmnType; \
	EMIT_IR_1ARG(IROpCode_LoadIndirect, emnTp); \
	StackObject* obj = StackObjectPool_Allocate(); \
	switch(*emnTp) \
	{ \
		case ElementType_I1: \
		case ElementType_I2: \
		case ElementType_I4: \
		case ElementType_U1: \
		case ElementType_U2: \
		case ElementType_U4: \
			obj->Type = StackObjectType_Int32; \
			break; \
		case ElementType_I8: \
		case ElementType_U8: \
			obj->Type = StackObjectType_Int64; \
			break; \
		case ElementType_I: \
		case ElementType_U: \
			obj->Type = StackObjectType_NativeInt; \
			break; \
		case ElementType_Ref: \
			obj->Type = StackObjectType_ReferenceType; \
			break; \
		default: \
			Panic("Unknown ElementType!"); \
			break; \
	} \
	switch(*emnTp) \
	{ \
		case ElementType_I1: \
			obj->NumericType = StackObjectNumericType_Int8; \
			break; \
		case ElementType_I2: \
			obj->NumericType = StackObjectNumericType_Int16; \
			break; \
		case ElementType_I4: \
			obj->NumericType = StackObjectNumericType_Int32; \
			break; \
		case ElementType_U1: \
			obj->NumericType = StackObjectNumericType_UInt8; \
			break; \
		case ElementType_U2: \
			obj->NumericType = StackObjectNumericType_UInt16; \
			break; \
		case ElementType_U4: \
			obj->NumericType = StackObjectNumericType_UInt32; \
			break; \
		case ElementType_I8: \
			obj->NumericType = StackObjectNumericType_Int64; \
			break; \
		case ElementType_U8: \
			obj->NumericType = StackObjectNumericType_UInt64; \
			break; \
		case ElementType_I: \
			obj->NumericType = StackObjectNumericType_Pointer; \
			break; \
		case ElementType_U: \
			obj->NumericType = StackObjectNumericType_UPointer; \
			break; \
		case ElementType_Ref: \
			obj->NumericType = StackObjectNumericType_Ref; \
			break; \
		default: \
			Panic("Unknown ElementType!"); \
			break; \
	} \
	SyntheticStack_Push(stack, obj); \
	ClearFlags(); \
	break; }





