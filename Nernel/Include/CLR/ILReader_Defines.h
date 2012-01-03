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
    Log_WriteLine(LogFlags_ILReading_BranchLinker, "instr->InstructionLocation = %i", (int)instr->InstructionLocation); \
    GetInstrOffset(); \
    instr->OpCode = instrType; \
    Log_WriteLine(LogFlags_IREmitting, "Emitting " #instrType); \
    IRMethod_AddInstruction(m, instr); }
#define EMIT_IR_1ARG(instrType, arg1) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->InstructionLocation = (CurInstructionBase - orig); \
    Log_WriteLine(LogFlags_ILReading_BranchLinker, "instr->InstructionLocation = %i", (int)instr->InstructionLocation); \
    GetInstrOffset(); \
    instr->Arg1 = arg1; \
    instr->OpCode = instrType; \
    Log_WriteLine(LogFlags_IREmitting, "Emitting " #instrType " With 1 argument ('" #arg1 "')"); \
    IRMethod_AddInstruction(m, instr); }
#define EMIT_IR_2ARG(instrType, arg1, arg2) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->InstructionLocation = (CurInstructionBase - orig); \
    Log_WriteLine(LogFlags_ILReading_BranchLinker, "instr->InstructionLocation = %i", (int)instr->InstructionLocation); \
    GetInstrOffset(); \
    instr->Arg1 = arg1; \
    instr->Arg2 = arg2; \
    instr->OpCode = instrType; \
    Log_WriteLine(LogFlags_IREmitting, "Emitting " #instrType " With 2 arguments ('" #arg1 "', '" #arg2 "')"); \
    IRMethod_AddInstruction(m, instr); }
#define EMIT_IR_3ARG(instrType, arg1, arg2, arg3) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->InstructionLocation = (CurInstructionBase - orig); \
    Log_WriteLine(LogFlags_ILReading_BranchLinker, "instr->InstructionLocation = %i", (int)instr->InstructionLocation); \
    GetInstrOffset(); \
    instr->Arg1 = arg1; \
    instr->Arg2 = arg2; \
    instr->Arg3 = arg3; \
    instr->OpCode = instrType; \
    Log_WriteLine(LogFlags_IREmitting, "Emitting " #instrType " With 3 arguments ('" #arg1 "', '" #arg2 "', '" #arg3 "')"); \
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
    obj2->Name = obj->Name; \
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
    obj2->Name = obj->Name; \
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
    obj2->Name = obj->Name; \
    StackObjectPool_Release(obj); \
    SyntheticStack_Push(stack, obj2); \
    EMIT_IR_2ARG(IROpCode_Convert_OverflowCheck, sType, tType); \
    ClearFlags(); \
    break; }



#define DefineBranchTarget(brnchType, opCode) \
    Log_WriteLine(LogFlags_ILReading, "Read " #opCode ); \
    branch_Condition = brnchType; \
    branch_Target = (uint32_t)ReadUInt32(dat); \
    goto Branch_Common; 
#define DefineBranchTarget_Short(brnchType, opCode) \
    Log_WriteLine(LogFlags_ILReading, "Read " #opCode ".S"); \
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
	obj = NULL; \
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


