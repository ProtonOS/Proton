#pragma once


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



#define ClearFlags() \
    { Constrained = FALSE; \
    No = FALSE; \
    ReadOnly = FALSE; \
    Tail = FALSE; \
    UnAligned = FALSE; \
    Volatile = FALSE; }



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

