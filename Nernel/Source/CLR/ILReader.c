#include <CLR/ILReader.h>
#include <CLR/ILReader_Defines.h>
#include <CLR/OpCodes_IL.h>
#include <CLR/OpCodes_IR.h>
#include <CLR/SyntheticStack.h>
#include <CLR/IROptimizer.h>
#include <CLR/IRMethod_BranchLinker.h>
#include <CLR/Log.h>
#include <String_Format.h>
#include <stdlib.h>
#include <stdio.h>


uint8_t ReadUInt8(uint8_t** dat);
uint16_t ReadUInt16(uint8_t** dat);
uint32_t ReadUInt32(uint8_t** dat);
uint64_t ReadUInt64(uint8_t** dat);
IRMethod* ReadIL(uint8_t** dat, uint32_t len, MethodDefinition* methodDef, CLIFile* fil);

ILAssembly* ILReader_CreateAssembly(CLIFile* fil)
{
    StackObjectPool_Initialize();
	ILAssembly* asmbly = (ILAssembly*)calloc(1, sizeof(ILAssembly));
    asmbly->CLIFile = fil;
    asmbly->IRAssembly = IRAssembly_Create();;

    for (uint32_t i = 1; i <= fil->MethodDefinitionCount; i++)
    {
        uint8_t* ilLoc = (uint8_t*)fil->MethodDefinitions[i].Body.Code;
        Log_WriteLine(LogFlags_ILReading, "Reading Method %s.%s.%s", fil->MethodDefinitions[i].TypeDefinition->Namespace, fil->MethodDefinitions[i].TypeDefinition->Name, fil->MethodDefinitions[i].Name);
        IRMethod* irMeth = ReadIL(&ilLoc, fil->MethodDefinitions[i].Body.CodeSize, &fil->MethodDefinitions[i], fil);
        IRMethod_BranchLinker_LinkMethod(irMeth);
        fil->MethodDefinitions[i].LoadedMethod = irMeth;
        irMeth->MethodIndex = i - 1;
        IRAssembly_AddMethod(asmbly->IRAssembly, irMeth);
    }

    StackObjectPool_Destroy();

    IROptimizer_Optimize(asmbly);

	return asmbly;
}


IRMethod* ReadIL(uint8_t** dat, uint32_t len, MethodDefinition* methodDef, CLIFile* fil)
{
    SyntheticStack* stack = SyntheticStack_Create();
    bool_t Constrained = FALSE;
    bool_t No = FALSE;
    bool_t ReadOnly = FALSE;
    bool_t Tail = FALSE;
    bool_t UnAligned = FALSE;
    bool_t Volatile = FALSE;
    BranchCondition branch_Condition = (BranchCondition)0;
    uint32_t branch_Target;
	size_t orig = (size_t)(*dat);
    size_t CurInstructionBase;
    uint8_t b;
    IRMethod* m = IRMethod_Create();

    while ((size_t)(*dat) - orig < len)
    {
        CurInstructionBase = ((size_t)*dat);
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "CurInstructionBase: %i", (int)CurInstructionBase);
        b = ReadUInt8(dat);
        switch (b)
        {
            case ILOpCode_Nop:				// 0x00
                Log_WriteLine(LogFlags_ILReading, "Read Nop");
                EMIT_IR(IROpCode_Nop);
                ClearFlags();
                break;
            case ILOpCode_Break:			// 0x01
                Log_WriteLine(LogFlags_ILReading, "Read Break");
                EMIT_IR(IROpCode_BreakForDebugger);
                ClearFlags();
                break;


            case ILOpCode_LdArg_0:			// 0x02
                
                ClearFlags();
                break;
            case ILOpCode_LdArg_1:			// 0x03
                {
                    Log_WriteLine(LogFlags_ILReading, "Read LdArg.1");

                    //methodDef->ParameterList[1].

                    uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)1;
                    EMIT_IR_1ARG(IROpCode_Load_LocalVar, dt);
                }
                ClearFlags();
                break;
            case ILOpCode_LdArg_2:			// 0x04
                
                ClearFlags();
                break;
            case ILOpCode_LdArg_3:			// 0x05
                
                ClearFlags();
                break;
            case ILOpCode_LdArg_S:			// 0x0E
                
                ClearFlags();
                break;
            case ILOpCode_LdArgA_S:			// 0x0F
                
                ClearFlags();
                break;
                

            case ILOpCode_StArg_S:			// 0x10
				{
					Log_WriteLine(LogFlags_ILReading, "Read StArg.S");
					uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)ReadUInt8(dat);
                    EMIT_IR_1ARG(IROpCode_Store_Parameter, dt);

                    StackObjectPool_Release(SyntheticStack_Pop(stack));
				}
                ClearFlags();
                break;


            case ILOpCode_LdLoc_0:			// 0x06
                {
                    Log_WriteLine(LogFlags_ILReading, "Read LdLoc.0");

                    //methodDef->Body.LocalVariableSignatureToken

                    uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)0;
                    EMIT_IR_1ARG(IROpCode_Load_LocalVar, dt);
                }
                ClearFlags();
                break;
            case ILOpCode_LdLoc_1:			// 0x07
                {
                    Log_WriteLine(LogFlags_ILReading, "Read LdLoc.1");



                    uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)1;
                    EMIT_IR_1ARG(IROpCode_Load_LocalVar, dt);
                }
                ClearFlags();
                break;
            case ILOpCode_LdLoc_2:			// 0x08
                {
                    Log_WriteLine(LogFlags_ILReading, "Read LdLoc.2");



                    uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)2;
                    EMIT_IR_1ARG(IROpCode_Load_LocalVar, dt);
                }
                ClearFlags();
                break;
            case ILOpCode_LdLoc_3:			// 0x09
                {
                    Log_WriteLine(LogFlags_ILReading, "Read LdLoc.3");



                    uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)3;
                    EMIT_IR_1ARG(IROpCode_Load_LocalVar, dt);
                }
                ClearFlags();
                break;
            case ILOpCode_LdLoc_S:			// 0x11
                {
                    Log_WriteLine(LogFlags_ILReading, "Read LdLoc.S");



                    uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)ReadUInt8(dat);
                    EMIT_IR_1ARG(IROpCode_Load_LocalVar, dt);
                }
                ClearFlags();
                break;


            case ILOpCode_LdLocA_S:			// 0x12
                {
                    Log_WriteLine(LogFlags_ILReading, "Read LdLocA.S");

                    uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)ReadUInt8(dat);
                    EMIT_IR_1ARG(IROpCode_Load_LocalVar_Address, dt);

                    StackObject* obj = StackObjectPool_Allocate();
                    obj->Name = String_Format("Address of Local Variable #%i", (int)*dt);
                    obj->Type = StackObjectType_ManagedPointer;
                    SyntheticStack_Push(stack, obj);
                }
                ClearFlags();
                break;


            case ILOpCode_StLoc_0:			// 0x0A
                DefineStLoc(0);
            case ILOpCode_StLoc_1:			// 0x0B
                DefineStLoc(1);
            case ILOpCode_StLoc_2:			// 0x0C
                DefineStLoc(2);
            case ILOpCode_StLoc_3:			// 0x0D
                DefineStLoc(3);
            case ILOpCode_StLoc_S:			// 0x13
                {
                    Log_WriteLine(LogFlags_ILReading, "Read StLoc.S");
                    uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)ReadUInt8(dat);
                    EMIT_IR_1ARG(IROpCode_Store_LocalVar, dt);

                    StackObjectPool_Release(SyntheticStack_Pop(stack));
                }
                ClearFlags();
                break;


            case ILOpCode_LdNull:			// 0x14
                
                ClearFlags();
                break;
            case ILOpCode_LdStr:			// 0x72
                {
                    Log_WriteLine(LogFlags_ILReading, "Read LdStr");
                    MetaDataToken* tkn = CLIFile_ResolveToken(fil, ReadUInt32(dat));
                    if (!tkn->IsUserString)
                        Panic("Invalid token after LdStr!");

                }
                ClearFlags();
                break;
            case ILOpCode_LdObj:			// 0x71
                
                ClearFlags();
                break;


            case ILOpCode_Ldc_I4_M1:		// 0x15
                {
                    Log_WriteLine(LogFlags_ILReading, "Read Ldc.I4.M1");

                    uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)-1;
                    EMIT_IR_1ARG(IROpCode_LoadInt32_Val, dt);

                    StackObject* s = StackObjectPool_Allocate();
                    s->Type = StackObjectType_Int32;
                    s->NumericType = StackObjectNumericType_Int32;
                    SyntheticStack_Push(stack, s);
                }
                ClearFlags();
                break;
            case ILOpCode_Ldc_I4_0:			// 0x16
                DefineLdcI4(0);
            case ILOpCode_Ldc_I4_1:			// 0x17
                DefineLdcI4(1);
            case ILOpCode_Ldc_I4_2:			// 0x18
                DefineLdcI4(2);
            case ILOpCode_Ldc_I4_3:			// 0x19
                DefineLdcI4(3);
            case ILOpCode_Ldc_I4_4:			// 0x1A
                DefineLdcI4(4);
            case ILOpCode_Ldc_I4_5:			// 0x1B
                DefineLdcI4(5);
            case ILOpCode_Ldc_I4_6:			// 0x1C
                DefineLdcI4(6);
            case ILOpCode_Ldc_I4_7:			// 0x1D
                DefineLdcI4(7);
            case ILOpCode_Ldc_I4_8:			// 0x1E
                DefineLdcI4(8);
            case ILOpCode_Ldc_I4_S:			// 0x1F
                {
                    Log_WriteLine(LogFlags_ILReading, "Read Ldc.I4.S");
                    
                    uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)((int32_t)((int8_t)ReadUInt8(dat)));
                    EMIT_IR_1ARG(IROpCode_LoadInt32_Val, dt);

                    StackObject* s = StackObjectPool_Allocate();
                    s->Type = StackObjectType_Int32;
                    s->NumericType = StackObjectNumericType_Int32;
                    SyntheticStack_Push(stack, s);
                }
                ClearFlags();
                break;
            case ILOpCode_Ldc_I4:			// 0x20
                {
                    Log_WriteLine(LogFlags_ILReading, "Read Ldc.I4");
                    
                    uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)ReadUInt32(dat);
                    EMIT_IR_1ARG(IROpCode_LoadInt32_Val, dt);

                    StackObject* s = StackObjectPool_Allocate();
                    s->Type = StackObjectType_Int32;
                    s->NumericType = StackObjectNumericType_Int32;
                    SyntheticStack_Push(stack, s);
                }
                ClearFlags();
                break;
            case ILOpCode_Ldc_I8:			// 0x21
                {
                    Log_WriteLine(LogFlags_ILReading, "Read Ldc.I8");
                    
                    uint64_t* dt = (uint64_t*)malloc(sizeof(uint64_t));
                    *dt = (uint64_t)ReadUInt64(dat);
                    EMIT_IR_1ARG(IROpCode_LoadInt64_Val, dt);

                    StackObject* s = StackObjectPool_Allocate();
                    s->Type = StackObjectType_Int64;
                    s->NumericType = StackObjectNumericType_Int64;
                    SyntheticStack_Push(stack, s);
                }
                ClearFlags();
                break;
            case ILOpCode_Ldc_R4:			// 0x22
                {
                    Log_WriteLine(LogFlags_ILReading, "Read Ldc.R4");
                    
                    uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                    *dt = (uint32_t)ReadUInt32(dat);
                    EMIT_IR_1ARG(IROpCode_LoadFloat32_Val, dt);

                    StackObject* s = StackObjectPool_Allocate();
                    s->Type = StackObjectType_Float;
                    s->NumericType = StackObjectNumericType_Float32;
                    SyntheticStack_Push(stack, s);
                }
                ClearFlags();
                break;
            case ILOpCode_Ldc_R8:			// 0x23
                {
                    Log_WriteLine(LogFlags_ILReading, "Read Ldc.R8");
                    
                    uint64_t* dt = (uint64_t*)malloc(sizeof(uint64_t));
                    *dt = (uint64_t)ReadUInt64(dat);
                    EMIT_IR_1ARG(IROpCode_LoadFloat64_Val, dt);

                    StackObject* s = StackObjectPool_Allocate();
                    s->Type = StackObjectType_Float;
                    s->NumericType = StackObjectNumericType_Float64;
                    SyntheticStack_Push(stack, s);
                }
                ClearFlags();
                break;


            // 0x24 Doesn't exist
            case ILOpCode_Dup:				// 0x25
                
                ClearFlags();
                break;
            case ILOpCode_Pop:				// 0x26
                
                ClearFlags();
                break;


            case ILOpCode_Call:				// 0x28
                
                ClearFlags();
                break;
            case ILOpCode_CallI:			// 0x29
                
                ClearFlags();
                break;
            case ILOpCode_CallVirt:			// 0x6F
                
                ClearFlags();
                break;
            case ILOpCode_Jmp:				// 0x27
                
                ClearFlags();
                break;


            case ILOpCode_Ret:				// 0x2A
                {
                    
                }
                ClearFlags();
                break;


            case ILOpCode_Br:				// 0x38
                DefineBranchTarget(BranchCondition_Always, Br);
            case ILOpCode_Br_S:				// 0x2B
                DefineBranchTarget_Short(BranchCondition_Always, Br);

            case ILOpCode_BrFalse:			// 0x39
                DefineBranchTarget(BranchCondition_False, BrFalse);
            case ILOpCode_BrFalse_S:		// 0x2C
                DefineBranchTarget_Short(BranchCondition_False, BrFalse);

            case ILOpCode_BrTrue:			// 0x3A
                DefineBranchTarget(BranchCondition_True, BrTrue);
            case ILOpCode_BrTrue_S:			// 0x2D
                DefineBranchTarget_Short(BranchCondition_True, BrTrue);

            case ILOpCode_Beq:				// 0x3B
                DefineBranchTarget(BranchCondition_Equal, Beq);
            case ILOpCode_Beq_S:			// 0x2E
                DefineBranchTarget_Short(BranchCondition_Equal, Beq);

            case ILOpCode_Bne_Un:			// 0x40
                DefineBranchTarget(BranchCondition_NotEqual_Unsigned, Bne.Un);
            case ILOpCode_Bne_Un_S:			// 0x33
                DefineBranchTarget_Short(BranchCondition_NotEqual_Unsigned, Bne.Un);

            case ILOpCode_Bge:				// 0x3C
                DefineBranchTarget(BranchCondition_Greater_Or_Equal, Bge);
            case ILOpCode_Bge_S:			// 0x2F
                DefineBranchTarget_Short(BranchCondition_Greater_Or_Equal, Bge);
            case ILOpCode_Bge_Un:			// 0x41
                DefineBranchTarget(BranchCondition_Greater_Or_Equal_Unsigned, Bge.Un);
            case ILOpCode_Bge_Un_S:			// 0x34
                DefineBranchTarget_Short(BranchCondition_Greater_Or_Equal_Unsigned, Bge.Un);

            case ILOpCode_Bgt:				// 0x3D
                DefineBranchTarget(BranchCondition_Greater, Bgt);
            case ILOpCode_Bgt_S:			// 0x30
                DefineBranchTarget_Short(BranchCondition_Greater, Bgt);
            case ILOpCode_Bgt_Un:			// 0x42
                DefineBranchTarget(BranchCondition_Greater_Unsigned, Bgt.Un);
            case ILOpCode_Bgt_Un_S:			// 0x35
                DefineBranchTarget_Short(BranchCondition_Greater_Unsigned, Bgt.Un);

            case ILOpCode_Ble:				// 0x3E
                DefineBranchTarget(BranchCondition_Less_Or_Equal, Ble);
            case ILOpCode_Ble_S:			// 0x31
                DefineBranchTarget_Short(BranchCondition_Less_Or_Equal, Ble);
            case ILOpCode_Ble_Un:			// 0x43
                DefineBranchTarget(BranchCondition_Less_Or_Equal_Unsigned, Ble.Un);
            case ILOpCode_Ble_Un_S:			// 0x36
                DefineBranchTarget_Short(BranchCondition_Less_Or_Equal_Unsigned, Ble.Un);

            case ILOpCode_Blt:				// 0x3F
                DefineBranchTarget(BranchCondition_Less, Blt);
            case ILOpCode_Blt_S:			// 0x32
                DefineBranchTarget_Short(BranchCondition_Less, Blt);
            case ILOpCode_Blt_Un:			// 0x44
                DefineBranchTarget(BranchCondition_Less_Unsigned, Blt.Un);
            case ILOpCode_Blt_Un_S:			// 0x37
                DefineBranchTarget_Short(BranchCondition_Less_Unsigned, Blt.Un);

Branch_Common:
                {
                    uint32_t* targt = (uint32_t*)malloc(sizeof(uint32_t));
                    *targt = ((((size_t)(*dat)) - orig) + ((int32_t)branch_Target));
                    BranchCondition* condtn = (BranchCondition*)malloc(sizeof(BranchCondition));
                    *condtn = branch_Condition;
                    EMIT_IR_2ARG(IROpCode_Branch, condtn, targt);
                }
                ClearFlags();
                break;


            case ILOpCode_Switch:			// 0x45
                
                ClearFlags();
                break;

				
            case ILOpCode_LdInd_I:			// 0x4D
                
                ClearFlags();
                break;
            case ILOpCode_LdInd_I1:			// 0x46
                
                ClearFlags();
                break;
            case ILOpCode_LdInd_U1:			// 0x47
                
                ClearFlags();
                break;
            case ILOpCode_LdInd_I2:			// 0x48
                
                ClearFlags();
                break;
            case ILOpCode_LdInd_U2:			// 0x49
                
                ClearFlags();
                break;
            case ILOpCode_LdInd_I4:			// 0x4A
                
                ClearFlags();
                break;
            case ILOpCode_LdInd_U4:			// 0x4B
                
                ClearFlags();
                break;
            case ILOpCode_LdInd_I8:			// 0x4C
                
                ClearFlags();
                break;
            case ILOpCode_LdInd_R4:			// 0x4D
                
                ClearFlags();
                break;
            case ILOpCode_LdInd_R8:			// 0x4F
                
                ClearFlags();
                break;
            case ILOpCode_LdInd_Ref:		// 0x50
                
                ClearFlags();
                break;


            case ILOpCode_StInd_Ref:		// 0x51
                
                ClearFlags();
                break;
            case ILOpCode_StInd_I:			// 0xDF
                
                ClearFlags();
                break;
            case ILOpCode_StInd_I1:			// 0x52
                
                ClearFlags();
                break;
            case ILOpCode_StInd_I2:			// 0x53
                
                ClearFlags();
                break;
            case ILOpCode_StInd_I4:			// 0x54
                
                ClearFlags();
                break;
            case ILOpCode_StInd_I8:			// 0x55
                
                ClearFlags();
                break;
            case ILOpCode_StInd_R4:			// 0x56
                
                ClearFlags();
                break;
            case ILOpCode_StInd_R8:			// 0x57
                
                ClearFlags();
                break;
                

            case ILOpCode_Add:				// 0x58
                
                ClearFlags();
                break;
            case ILOpCode_Add_Ovf:			// 0xD6
                
                ClearFlags();
                break;
            case ILOpCode_Add_Ovf_Un:		// 0xD7
                
                ClearFlags();
                break;


            case ILOpCode_Sub:				// 0x59
                
                ClearFlags();
                break;
            case ILOpCode_Sub_Ovf:			// 0xDA
                
                ClearFlags();
                break;
            case ILOpCode_Sub_Ovf_Un:		// 0xDB
                
                ClearFlags();
                break;


            case ILOpCode_Mul:				// 0x5A
                
                ClearFlags();
                break;
            case ILOpCode_Mul_Ovf:			// 0xD8
                
                ClearFlags();
                break;
            case ILOpCode_Mul_Ovf_Un:		// 0xD9
                
                ClearFlags();
                break;


            case ILOpCode_Div:				// 0x5B
                
                ClearFlags();
                break;
            case ILOpCode_Div_Un:			// 0x5C
                
                ClearFlags();
                break;


            case ILOpCode_Rem:				// 0x5D
                
                ClearFlags();
                break;
            case ILOpCode_Rem_Un:			// 0x5E
                
                ClearFlags();
                break;


            case ILOpCode_And:				// 0x5F
                
                ClearFlags();
                break;


            case ILOpCode_Or:				// 0x60
                
                ClearFlags();
                break;


            case ILOpCode_Xor:				// 0x61
                
                ClearFlags();
                break;


            case ILOpCode_Shl:				// 0x62
				DefineShift(Left, Shl);
            // The only language I know of that 
            // emits this currently is J#.
            // (This is a sign-extended right shift,
            // aka. the >>> operator)
            case ILOpCode_Shr:				// 0x63
				DefineShift(Right_Sign_Extended, Shr);
            case ILOpCode_Shr_Un:			// 0x64
				DefineShift(Right, Shr.Un);


            case ILOpCode_Neg:				// 0x65
                
                ClearFlags();
                break;


            case ILOpCode_Not:				// 0x66
                
                ClearFlags();
                break;


            case ILOpCode_Conv_I1:			// 0x67
                DEFINE_CONV_UNCHECKED(I1, Int32, Int8);
            case ILOpCode_Conv_U1:			// 0xD2
                DEFINE_CONV_UNCHECKED(U1, Int32, UInt8);
            case ILOpCode_Conv_I2:			// 0x68
                DEFINE_CONV_UNCHECKED(I2, Int32, Int16);
            case ILOpCode_Conv_U2:			// 0xD1
                DEFINE_CONV_UNCHECKED(U2, Int32, UInt16);
            case ILOpCode_Conv_I4:			// 0x69
                DEFINE_CONV_UNCHECKED(I4, Int32, Int32);
            case ILOpCode_Conv_U4:			// 0x6D
                DEFINE_CONV_UNCHECKED(U4, Int32, UInt32);
            case ILOpCode_Conv_I8:			// 0x6A
                DEFINE_CONV_UNCHECKED(I8, Int64, Int64);
            case ILOpCode_Conv_U8:			// 0x6E
                DEFINE_CONV_UNCHECKED(U8, Int64, UInt64);
            case ILOpCode_Conv_R4:			// 0x6B
                DEFINE_CONV_UNCHECKED(R4, Float, Float32);
            case ILOpCode_Conv_R8:			// 0x6C
                DEFINE_CONV_UNCHECKED(R8, Float, Float64);
            case ILOpCode_Conv_I:			// 0xD3
                DEFINE_CONV_UNCHECKED(I, NativeInt, Pointer);
            case ILOpCode_Conv_U:			// 0xE0
                DEFINE_CONV_UNCHECKED(U, NativeInt, UPointer);




            case ILOpCode_Conv_R_Un:		// 0x76
                
                ClearFlags();
                break;




            case ILOpCode_CpObj:			// 0x70
                
                ClearFlags();
                break;


            case ILOpCode_NewObj:			// 0x73
                
                ClearFlags();
                break;
            case ILOpCode_NewArr:			// 0x8D
                
                ClearFlags();
                break;



            case ILOpCode_CastClass:		// 0x74
                
                ClearFlags();
                break;
            case ILOpCode_IsInst:			// 0x75
                
                ClearFlags();
                break;


            // 0x77 Doesn't exist
            // 0x78 Doesn't exist
            case ILOpCode_UnBox:			// 0x79
                
                ClearFlags();
                break;
            case ILOpCode_Throw:			// 0x7A
                
                ClearFlags();
                break;


            case ILOpCode_LdFld:			// 0x7B
                
                ClearFlags();
                break;
            case ILOpCode_LdFldA:			// 0x7C
                
                ClearFlags();
                break;
            case ILOpCode_StFld:			// 0x7D
                
                ClearFlags();
                break;
            case ILOpCode_LdSFld:			// 0x7E
                
                ClearFlags();
                break;
            case ILOpCode_LdSFldA:			// 0x7F
                
                ClearFlags();
                break;
            case ILOpCode_StSFld:			// 0x80
                
                ClearFlags();
                break;
            case ILOpCode_StObj:			// 0x81
                
                ClearFlags();
                break;


            case ILOpCode_Conv_Ovf_I1:		// 0xB3
                DEFINE_CONV_OVF(I1, Int32, Int8);
            case ILOpCode_Conv_Ovf_I1_Un:	// 0x82
                DEFINE_CONV_OVF_UN(I1, Int32, Int8);
            case ILOpCode_Conv_Ovf_U1:		// 0xB4
                DEFINE_CONV_OVF(U1, Int32, UInt8);
            case ILOpCode_Conv_Ovf_U1_Un:	// 0x86
                DEFINE_CONV_OVF_UN(U1, Int32, UInt8);
            case ILOpCode_Conv_Ovf_I2:		// 0xB5
                DEFINE_CONV_OVF(I2, Int32, Int16);
            case ILOpCode_Conv_Ovf_I2_Un:	// 0x83
                DEFINE_CONV_OVF_UN(I2, Int32, Int16);
            case ILOpCode_Conv_Ovf_U2:		// 0xB6
                DEFINE_CONV_OVF(U2, Int32, UInt16);
            case ILOpCode_Conv_Ovf_U2_Un:	// 0x87
                DEFINE_CONV_OVF_UN(U2, Int32, UInt16);
            case ILOpCode_Conv_Ovf_I4:		// 0xB7
                DEFINE_CONV_OVF(I4, Int32, Int32);
            case ILOpCode_Conv_Ovf_I4_Un:	// 0x84
                DEFINE_CONV_OVF_UN(I4, Int32, Int32);
            case ILOpCode_Conv_Ovf_U4:		// 0xB8
                DEFINE_CONV_OVF(U4, Int32, UInt32);
            case ILOpCode_Conv_Ovf_U4_Un:	// 0x88
                DEFINE_CONV_OVF_UN(U4, Int32, UInt32);
            case ILOpCode_Conv_Ovf_I8:		// 0xB9
                DEFINE_CONV_OVF(I8, Int64, Int64);
            case ILOpCode_Conv_Ovf_I8_Un:	// 0x85
                DEFINE_CONV_OVF_UN(I8, Int64, Int64);
            case ILOpCode_Conv_Ovf_U8:		// 0xBA
                DEFINE_CONV_OVF(U8, Int64, UInt64);
            case ILOpCode_Conv_Ovf_U8_Un:	// 0x89
                DEFINE_CONV_OVF_UN(U8, Int64, UInt64);
            case ILOpCode_Conv_Ovf_I:		// 0xD4
                DEFINE_CONV_OVF(I, NativeInt, Pointer);
            case ILOpCode_Conv_Ovf_I_Un:	// 0x8A
                DEFINE_CONV_OVF_UN(I, NativeInt, Pointer);
            case ILOpCode_Conv_Ovf_U:		// 0xD5
                DEFINE_CONV_OVF(U, NativeInt, UPointer);
            case ILOpCode_Conv_Ovf_U_Un:	// 0x8B
                DEFINE_CONV_OVF_UN(U, NativeInt, UPointer);


            case ILOpCode_Box:				// 0x8C
                
                ClearFlags();
                break;
            case ILOpCode_LdLen:			// 0x8E
                
                ClearFlags();
                break;
            case ILOpCode_LdElemA:			// 0x8F
                
                ClearFlags();
                break;
            case ILOpCode_LdElem_I1:		// 0x90
                
                ClearFlags();
                break;
            case ILOpCode_LdElem_U1:		// 0x91
                
                ClearFlags();
                break;
            case ILOpCode_LdElem_I2:		// 0x92
                
                ClearFlags();
                break;
            case ILOpCode_LdElem_U2:		// 0x93
                
                ClearFlags();
                break;
            case ILOpCode_LdElem_I4:		// 0x94
                
                ClearFlags();
                break;
            case ILOpCode_LdElem_U4:		// 0x95
                
                ClearFlags();
                break;
			// This is also known as LdElem.U8
            case ILOpCode_LdElem_I8:		// 0x96
                
                ClearFlags();
                break;
            case ILOpCode_LdElem_I:			// 0x97
                
                ClearFlags();
                break;
            case ILOpCode_LdElem_R4:		// 0x98
                
                ClearFlags();
                break;
            case ILOpCode_LdElem_R8:		// 0x99
                
                ClearFlags();
                break;
            case ILOpCode_LdElem_Ref:		// 0x9A
                
                ClearFlags();
                break;
            case ILOpCode_LdElem:			// 0xA3
                
                ClearFlags();
                break;


            case ILOpCode_StElem_I:			// 0x9B
                
                ClearFlags();
                break;
            case ILOpCode_StElem_I1:		// 0x9C
                
                ClearFlags();
                break;
            case ILOpCode_StElem_I2:		// 0x9D
                
                ClearFlags();
                break;
            case ILOpCode_StElem_I4:		// 0x9E
                
                ClearFlags();
                break;
            case ILOpCode_StElem_I8:		// 0x9F
                
                ClearFlags();
                break;
            case ILOpCode_StElem_R4:		// 0xA0
                
                ClearFlags();
                break;
            case ILOpCode_StElem_R8:		// 0xA1
                
                ClearFlags();
                break;
            case ILOpCode_StElem_Ref:		// 0xA2
                
                ClearFlags();
                break;
            case ILOpCode_StElem:			// 0xA4
                
                ClearFlags();
                break;


            case ILOpCode_Unbox_Any:		// 0xA5
                
                ClearFlags();
                break;
            // 0xA6 Doesn't exist
            // 0xA7 Doesn't exist
            // 0xA8 Doesn't exist
            // 0xA9 Doesn't exist
            // 0xAA Doesn't exist
            // 0xAB Doesn't exist
            // 0xAC Doesn't exist
            // 0xAD Doesn't exist
            // 0xAE Doesn't exist
            // 0xAF Doesn't exist
            // 0xB0 Doesn't exist
            // 0xB1 Doesn't exist
            // 0xB2 Doesn't exist
            // 0xBB Doesn't exist
            // 0xBC Doesn't exist
            // 0xBD Doesn't exist
            // 0xBE Doesn't exist
            // 0xBF Doesn't exist
            // 0xC0 Doesn't exist
            // 0xC1 Doesn't exist
            case ILOpCode_RefAnyVal:		// 0xC2
                
                ClearFlags();
                break;
            case ILOpCode_CkFinite:			// 0xC3
                
                ClearFlags();
				break;
            // 0xC4 Doesn't exist
            // 0xC5 Doesn't exist
            case ILOpCode_MkRefAny:			// 0xC6
                
                ClearFlags();
                break;
            // 0xC7 Doesn't exist
            // 0xC8 Doesn't exist
            // 0xC9 Doesn't exist
            // 0xCA Doesn't exist
            // 0xCB Doesn't exist
            // 0xCC Doesn't exist
            // 0xCD Doesn't exist
            // 0xCE Doesn't exist
            // 0xCF Doesn't exist
            case ILOpCode_LdToken:			// 0xD0
                
                ClearFlags();
                break;
            case ILOpCode_EndFinally:		// 0xDC
                
                ClearFlags();
                break;
            case ILOpCode_Leave:			// 0xDD
                
                ClearFlags();
                break;
            case ILOpCode_Leave_S:			// 0xDE
                
                ClearFlags();
                break;
            // 0xE1 Doesn't Exist
            // 0xE2 Doesn't Exist
            // 0xE3 Doesn't Exist
            // 0xE4 Doesn't Exist
            // 0xE5 Doesn't Exist
            // 0xE6 Doesn't Exist
            // 0xE7 Doesn't Exist
            // 0xE8 Doesn't Exist
            // 0xE9 Doesn't Exist
            // 0xEA Doesn't Exist
            // 0xEB Doesn't Exist
            // 0xEC Doesn't Exist
            // 0xED Doesn't Exist
            // 0xEE Doesn't Exist
            // 0xEF Doesn't Exist
            // 0xF0 Doesn't Exist
            // 0xF1 Doesn't Exist
            // 0xF2 Doesn't Exist
            // 0xF3 Doesn't Exist
            // 0xF4 Doesn't Exist
            // 0xF5 Doesn't Exist
            // 0xF6 Doesn't Exist
            // 0xF7 Doesn't Exist
            // 0xF8 Doesn't Exist
            // 0xF9 Doesn't Exist
            // 0xFA Doesn't Exist
            // 0xFB Doesn't Exist
            // 0xFC Doesn't Exist
            // 0xFD Doesn't Exist
            case ILOpCode_Extended:         // 0xFE
                b = ReadUInt8(dat);

                switch (b)
                {
                    case ILOpCodes_Extended_ArgList:		// 0x00
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_Ceq:			// 0x01
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_Cgt:			// 0x02
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_Cgt_Un:			// 0x03
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_Clt:			// 0x04
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_Clt_Un:			// 0x05
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_LdFtn:			// 0x06
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_LdVirtFtn:		// 0x07
                        
                        ClearFlags();
                        break;
                    // 0x08 Doesn't exist
                    case ILOpCodes_Extended_LdArg:			// 0x09

                        break;
                    case ILOpCodes_Extended_LdArgA:			// 0x0A
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_StArg:			// 0x0B
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_LdLoc:			// 0x0C
                        {
                            Log_WriteLine(LogFlags_ILReading, "Read LdLoc");
                            uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                            *dt = (uint32_t)ReadUInt16(dat);
                            EMIT_IR_1ARG(IROpCode_Load_LocalVar, dt);
                        }
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_LdLocA:			// 0x0D
                        {
                            Log_WriteLine(LogFlags_ILReading, "Read LdLocA");

                            uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                            *dt = (uint32_t)ReadUInt16(dat);
                            EMIT_IR_1ARG(IROpCode_Load_LocalVar_Address, dt);

                            StackObject* obj = StackObjectPool_Allocate();
                            obj->Name = String_Format("Address of Local Variable #%i", (int)*dt);
                            obj->Type = StackObjectType_ManagedPointer;
                            SyntheticStack_Push(stack, obj);
                        }
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_StLoc:			// 0x0E
                        {
                            Log_WriteLine(LogFlags_ILReading, "Read StLoc");
                            uint32_t* dt = (uint32_t*)malloc(sizeof(uint32_t));
                            *dt = (uint32_t)ReadUInt16(dat);
                            EMIT_IR_1ARG(IROpCode_Store_LocalVar, dt);

                            StackObjectPool_Release(SyntheticStack_Pop(stack));
                        }
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_LocAlloc:		// 0x0F
                        
                        ClearFlags();
                        break;
					// 0x10 Doesn't exist
                    case ILOpCodes_Extended_EndFilter:		// 0x11
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_InitObj:		// 0x15
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_CpBlk:			// 0x17
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_InitBlk:		// 0x18
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_ReThrow:		// 0x1A

                        break;
                    // 0x1B Doesn't exist
                    case ILOpCodes_Extended_SizeOf:			// 0x1C
                        
                        ClearFlags();
                        break;
                    case ILOpCodes_Extended_RefAnyType:		// 0x1D
                        
                        ClearFlags();
                        break;


                    case ILOpCodes_Extended_Constrained__:	// 0x16
                        Constrained = TRUE;
                        break;
                    case ILOpCodes_Extended_No__:			// 0x19
                        No = TRUE;
                        break;
                    case ILOpCodes_Extended_ReadOnly__:		// 0x1E
                        ReadOnly = TRUE;
                        break;
                    case ILOpCodes_Extended_Tail__:			// 0x14
                        Tail = TRUE;
                        break;
                    case ILOpCodes_Extended_Unaligned__:	// 0x12
                        UnAligned = TRUE;
                        break;
                    case ILOpCodes_Extended_Volatile__:		// 0x13
                        Volatile = TRUE;
                        break;

                }
    			break;
    		// 0xFF Doesn't Exist
        }
    }
    return m;
}

uint8_t ReadUInt8(uint8_t** dat)
{
    uint8_t b = **dat;
    (*dat)++;
    return b;
}

uint16_t ReadUInt16(uint8_t** dat)
{
	uint16_t i = *((uint16_t*)*dat);
    *dat += 2;
    return i;
}

uint32_t ReadUInt32(uint8_t** dat)
{
	uint32_t i = *((uint32_t*)*dat);
    *dat += 4;
    return i;
}

uint64_t ReadUInt64(uint8_t** dat)
{
    uint64_t i = *((uint64_t*)*dat);
    *dat += 8;
    return i;
}