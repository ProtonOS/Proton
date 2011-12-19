#include <CLR/ILReader.h>
#include <CLR/OpCodes_IL.h>
#include <CLR/OpCodes_IR.h>
#include <CLR/SyntheticStack.h>
#include <stdlib.h>
#include <stdio.h>


uint8_t ReadUInt8(uint8_t** dat);
uint16_t ReadUInt16(uint8_t** dat);
uint32_t ReadUInt32(uint8_t** dat);
IRMethod* ReadIL(uint8_t** dat, uint32_t len);

ILAssembly* ILReader_CreateAssembly(CLIFile* fil)
{
	ILAssembly* asmbly = (ILAssembly*)calloc(1, sizeof(ILAssembly));
    asmbly->CLIFile = fil;
    asmbly->IRAssembly = IRAssembly_Create();;

    for (uint32_t i = 0; i < fil->MethodDefinitionCount; i++)
    {
        uint8_t* ilLoc = (uint8_t*)fil->MethodDefinitions[i].BodyCode;
        IRAssembly_AddMethod(asmbly->IRAssembly, ReadIL(&ilLoc, fil->MethodDefinitions[i].BodyHeader.CodeSize));
    }

	return asmbly;
}

#define EMIT_IR(instrType) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->OpCode = (uint32_t)(instrType); \
    printf("Emitting " #instrType "\n"); \
    IRMethod_AddInstruction(m, instr); }
#define EMIT_IR_1ARG(instrType, arg1) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->Arg1 = arg1; \
    instr->OpCode = (uint32_t)(instrType); \
    printf("Emitting " #instrType "\n"); \
    IRMethod_AddInstruction(m, instr); }
#define EMIT_IR_2ARG(instrType, arg1, arg2) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->Arg1 = arg1; \
    instr->Arg2 = arg2; \
    instr->OpCode = (uint32_t)(instrType); \
    printf("Emitting " #instrType "\n"); \
    IRMethod_AddInstruction(m, instr); }
#define EMIT_IR_3ARG(instrType, arg1, arg2, arg3) \
    { IRInstruction* instr = IRInstruction_Create(); \
    instr->Arg1 = arg1; \
    instr->Arg2 = arg2; \
    instr->Arg3 = arg3; \
    instr->OpCode = (uint32_t)(instrType); \
    printf("Emitting " #instrType "\n"); \
    IRMethod_AddInstruction(m, instr); }

IRMethod* ReadIL(uint8_t** dat, uint32_t len)
{
    //SyntheticStack* stack = SyntheticStack_Create();
	uint32_t orig = (uint32_t)(*dat);
    uint8_t b;
    IRMethod* m = IRMethod_Create();

    while ((uint32_t)(*dat) - orig < len)
    {
        b = ReadUInt8(dat);
        switch (b)
        {
            case ILOpCode_Nop:				// 0x00
                EMIT_IR(IROpCode_Nop);
                break;
            case ILOpCode_Break:			// 0x01

                break;
            case ILOpCode_LdArg_0:			// 0x02
                
                break;
            case ILOpCode_LdArg_1:			// 0x03

                break;
            case ILOpCode_LdArg_2:			// 0x04

                break;
            case ILOpCode_LdArg_3:			// 0x05

                break;
            case ILOpCode_LdLoc_0:			// 0x06
                 
                break;
            case ILOpCode_LdLoc_1:			// 0x07

                break;
            case ILOpCode_LdLoc_2:			// 0x08

                break;
            case ILOpCode_LdLoc_3:			// 0x09

                break;
            case ILOpCode_StLoc_0:			// 0x0A

                break;
            case ILOpCode_StLoc_1:			// 0x0B

                break;
            case ILOpCode_StLoc_2:			// 0x0C

                break;
            case ILOpCode_StLoc_3:			// 0x0D

                break;
            case ILOpCode_LdArg_S:			// 0x0E

                break;
            case ILOpCode_LdArgA_S:			// 0x0F

                break;
            case ILOpCode_StArg_S:			// 0x10

                break;
            case ILOpCode_LdLoc_S:			// 0x11

                break;
            case ILOpCode_LdLocA_S:			// 0x12

                break;
            case ILOpCode_StLoc_S:			// 0x13

                break;
            case ILOpCode_LdNull:			// 0x14

                break;
            case ILOpCode_Ldc_I4_M1:		// 0x15

                break;
			case ILOpCode_Ldc_I4_0:			// 0x16

                break;
			case ILOpCode_Ldc_I4_1:			// 0x17

                break;
			case ILOpCode_Ldc_I4_2:			// 0x18

                break;
			case ILOpCode_Ldc_I4_3:			// 0x19

                break;
			case ILOpCode_Ldc_I4_4:			// 0x1A

                break;
			case ILOpCode_Ldc_I4_5:			// 0x1B

                break;
			case ILOpCode_Ldc_I4_6:			// 0x1C

                break;
			case ILOpCode_Ldc_I4_7:			// 0x1D

                break;
			case ILOpCode_Ldc_I4_8:			// 0x1E

                break;
			case ILOpCode_Ldc_I4_S:			// 0x1F

                break;
			case ILOpCode_Ldc_I4:			// 0x20

                break;
			case ILOpCode_Ldc_I8:			// 0x21

                break;
			case ILOpCode_Ldc_R4:			// 0x22

                break;
			case ILOpCode_Ldc_R8:			// 0x23

                break;
			// 0x24 Doesn't exist
            case ILOpCode_Dup:				// 0x25

                break;
            case ILOpCode_Pop:				// 0x26

                break;
            case ILOpCode_Jmp:				// 0x27

                break;
            case ILOpCode_Call:				// 0x28

                break;
            case ILOpCode_CallI:			// 0x29

                break;
            case ILOpCode_Ret:				// 0x2A

                break;
            case ILOpCode_Br_S:				// 0x2B

                break;
            case ILOpCode_BrFalse_S:		// 0x2C

                break;
            case ILOpCode_BrTrue_S:			// 0x2D

                break;
            case ILOpCode_Beq_S:			// 0x2E

                break;
            case ILOpCode_Bge_S:			// 0x2F

                break;
            case ILOpCode_Bgt_S:			// 0x30

                break;
            case ILOpCode_Ble_S:			// 0x31

                break;
            case ILOpCode_Blt_S:			// 0x32

                break;
            case ILOpCode_Bne_Un_S:			// 0x33

                break;
            case ILOpCode_Bge_Un_S:			// 0x34

                break;
            case ILOpCode_Bgt_Un_S:			// 0x35

                break;
            case ILOpCode_Ble_Un_S:			// 0x36

                break;
            case ILOpCode_Blt_Un_S:			// 0x37

                break;
            case ILOpCode_Br:				// 0x38

                break;
            case ILOpCode_BrFalse:			// 0x39

                break;
            case ILOpCode_BrTrue:			// 0x3A

                break;
            case ILOpCode_Beq:				// 0x3B

                break;
            case ILOpCode_Bge:				// 0x3C

                break;
            case ILOpCode_Bgt:				// 0x3D

                break;
            case ILOpCode_Ble:				// 0x3E

                break;
            case ILOpCode_Blt:				// 0x3F

                break;
            case ILOpCode_Bne_Un:			// 0x40

                break;
            case ILOpCode_Bge_Un:			// 0x41

                break;
            case ILOpCode_Bgt_Un:			// 0x42

                break;
            case ILOpCode_Ble_Un:			// 0x43

                break;
            case ILOpCode_Blt_Un:			// 0x44

                break;
            case ILOpCode_Switch:			// 0x45

                break;
            case ILOpCode_LdInd_I1:			// 0x46

                break;
			case ILOpCode_LdInd_U1:			// 0x47

                break;
			case ILOpCode_LdInd_I2:			// 0x48

                break;
			case ILOpCode_LdInd_U2:			// 0x49

                break;
			case ILOpCode_LdInd_I4:			// 0x4A

                break;
			case ILOpCode_LdInd_U4:			// 0x4B

                break;
			case ILOpCode_LdInd_I8:			// 0x4C

                break;
			case ILOpCode_LdInd_I:			// 0x4D

                break;
			case ILOpCode_LdInd_R4:			// 0x4D

                break;
			case ILOpCode_LdInd_R8:			// 0x4F

                break;
			case ILOpCode_LdInd_Ref:		// 0x50

                break;
			case ILOpCode_StInd_Ref:		// 0x51

                break;
			case ILOpCode_StInd_I1:			// 0x52

                break;
			case ILOpCode_StInd_I2:			// 0x53

                break;
			case ILOpCode_StInd_I4:			// 0x54

                break;
			case ILOpCode_StInd_I8:			// 0x55

                break;
			case ILOpCode_StInd_R4:			// 0x56

                break;
			case ILOpCode_StInd_R8:			// 0x57

                break;
            case ILOpCode_Add:				// 0x58

                break;
            case ILOpCode_Sub:				// 0x59

                break;
            case ILOpCode_Mul:				// 0x5A

                break;
            case ILOpCode_Div:				// 0x5B

                break;
            case ILOpCode_Div_Un:			// 0x5C

                break;
            case ILOpCode_Rem:				// 0x5D

                break;
            case ILOpCode_Rem_Un:			// 0x5E

                break;
            case ILOpCode_And:				// 0x5F

                break;
            case ILOpCode_Or:				// 0x60

                break;
            case ILOpCode_Xor:				// 0x61

                break;
            case ILOpCode_Shl:				// 0x62

                break;
            case ILOpCode_Shr:				// 0x63

                break;
            case ILOpCode_Shr_Un:			// 0x64

                break;
            case ILOpCode_Neg:				// 0x65

                break;
            case ILOpCode_Not:				// 0x66

                break;
            case ILOpCode_Conv_I1:			// 0x67

                break;
			case ILOpCode_Conv_I2:			// 0x68

                break;
			case ILOpCode_Conv_I4:			// 0x69

                break;
			case ILOpCode_Conv_I8:			// 0x6A

                break;
			case ILOpCode_Conv_R4:			// 0x6B

                break;
			case ILOpCode_Conv_R8:			// 0x6C

                break;
			case ILOpCode_Conv_U4:			// 0x6D

                break;
			case ILOpCode_Conv_U8:			// 0x6E

                break;
            case ILOpCode_CallVirt:			// 0x6F

                break;
            case ILOpCode_CpObj:			// 0x70

                break;
            case ILOpCode_LdObj:			// 0x71

                break;
            case ILOpCode_LdStr:			// 0x72

                break;
            case ILOpCode_NewObj:			// 0x73

                break;
            case ILOpCode_CastClass:		// 0x74

                break;
            case ILOpCode_IsInst:			// 0x75

                break;
            case ILOpCode_Conv_R_Un:		// 0x76

                break;
			// 0x77 Doesn't exist
			// 0x78 Doesn't exist
            case ILOpCode_UnBox:			// 0x79

                break;
            case ILOpCode_Throw:			// 0x7A

                break;
            case ILOpCode_LdFld:			// 0x7B

                break;
            case ILOpCode_LdFldA:			// 0x7C

                break;
            case ILOpCode_StFld:			// 0x7D

                break;
            case ILOpCode_LdSFld:			// 0x7E

                break;
            case ILOpCode_LdSFldA:			// 0x7F

                break;
            case ILOpCode_StSFld:			// 0x80

                break;
            case ILOpCode_StObj:			// 0x81

                break;
            case ILOpCode_Conv_Ovf_I1_Un:	// 0x82

                break;
			case ILOpCode_Conv_Ovf_I2_Un:	// 0x83

                break;
			case ILOpCode_Conv_Ovf_I4_Un:	// 0x84

                break;
			case ILOpCode_Conv_Ovf_I8_Un:	// 0x85

                break;
			case ILOpCode_Conv_Ovf_U1_Un:	// 0x86

                break;
			case ILOpCode_Conv_Ovf_U2_Un:	// 0x87

                break;
			case ILOpCode_Conv_Ovf_U4_Un:	// 0x88

                break;
			case ILOpCode_Conv_Ovf_U8_Un:	// 0x89

                break;
			case ILOpCode_Conv_Ovf_I_Un:	// 0x8A

                break;
			case ILOpCode_Conv_Ovf_U_Un:	// 0x8B

                break;
            case ILOpCode_Box:				// 0x8C

                break;
            case ILOpCode_NewArr:			// 0x8D

                break;
            case ILOpCode_LdLen:			// 0x8E

                break;
            case ILOpCode_LdElemA:			// 0x8F

                break;
			case ILOpCode_LdElem_I1:		// 0x90

                break;
			case ILOpCode_LdElem_U1:		// 0x91

                break;
			case ILOpCode_LdElem_I2:		// 0x92

                break;
			case ILOpCode_LdElem_U2:		// 0x93

                break;
			case ILOpCode_LdElem_I4:		// 0x94

                break;
			case ILOpCode_LdElem_U4:		// 0x95

                break;
			case ILOpCode_LdElem_I8:		// 0x96

                break;
			case ILOpCode_LdElem_I:			// 0x97

                break;
			case ILOpCode_LdElem_R4:		// 0x98

                break;
			case ILOpCode_LdElem_R8:		// 0x99

                break;
			case ILOpCode_LdElem_Ref:		// 0x9A

                break;
			case ILOpCode_StElem_I:			// 0x9B

                break;
			case ILOpCode_StElem_I1:		// 0x9C

                break;
			case ILOpCode_StElem_I2:		// 0x9D

                break;
			case ILOpCode_StElem_I4:		// 0x9E

                break;
			case ILOpCode_StElem_I8:		// 0x9F

                break;
			case ILOpCode_StElem_R4:		// 0xA0

                break;
			case ILOpCode_StElem_R8:		// 0xA1

                break;
			case ILOpCode_StElem_Ref:		// 0xA2

                break;
            case ILOpCode_LdElem:			// 0xA3

                break;
            case ILOpCode_StElem:			// 0xA4

                break;
            case ILOpCode_Unbox_Any:		// 0xA5

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
            case ILOpCode_Conv_Ovf_I1:		// 0xB3

                break;
			case ILOpCode_Conv_Ovf_U1:		// 0xB4

                break;
			case ILOpCode_Conv_Ovf_I2:		// 0xB5

                break;
			case ILOpCode_Conv_Ovf_U2:		// 0xB6

                break;
			case ILOpCode_Conv_Ovf_I4:		// 0xB7

                break;
			case ILOpCode_Conv_Ovf_U4:		// 0xB8

                break;
			case ILOpCode_Conv_Ovf_I8:		// 0xB9

                break;
    		case ILOpCode_Conv_Ovf_U8:		// 0xBA

				break;
			// 0xBB Doesn't exist
			// 0xBC Doesn't exist
			// 0xBD Doesn't exist
			// 0xBE Doesn't exist
			// 0xBF Doesn't exist
			// 0xC0 Doesn't exist
			// 0xC1 Doesn't exist
            case ILOpCode_RefAnyVal:		// 0xC2

                break;
            case ILOpCode_CkFinite:			// 0xC3

				break;
			// 0xC4 Doesn't exist
			// 0xC5 Doesn't exist
            case ILOpCode_MkRefAny:			// 0xC6

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

                break;
            case ILOpCode_Conv_U2:			// 0xD1

                break;
            case ILOpCode_Conv_U1:			// 0xD2

                break;
            case ILOpCode_Conv_I:			// 0xD3

                break;
            case ILOpCode_Conv_Ovf_I:		// 0xD4

                break;
            case ILOpCode_Conv_Ovf_U:		// 0xD5

                break;
            case ILOpCode_Add_Ovf:			// 0xD6

                break;
            case ILOpCode_Add_Ovf_Un:		// 0xD7

                break;
            case ILOpCode_Mul_Ovf:			// 0xD8

                break;
            case ILOpCode_Mul_Ovf_Un:		// 0xD9

                break;
            case ILOpCode_Sub_Ovf:			// 0xDA

                break;
            case ILOpCode_Sub_Ovf_Un:		// 0xDB

                break;
            case ILOpCode_EndFinally:		// 0xDC

                break;
            case ILOpCode_Leave:			// 0xDD

                break;
            case ILOpCode_Leave_S:			// 0xDE

				break;
    		case ILOpCode_StInd_I:			// 0xDF

				break;
			case ILOpCode_Conv_U:			// 0xE0

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

                        break;
                    case ILOpCodes_Extended_Ceq:			// 0x01

                        break;
                    case ILOpCodes_Extended_Cgt:			// 0x02

                        break;
                    case ILOpCodes_Extended_Cgt_Un:			// 0x03

                        break;
                    case ILOpCodes_Extended_Clt:			// 0x04

                        break;
                    case ILOpCodes_Extended_Clt_Un:			// 0x05

                        break;
                    case ILOpCodes_Extended_LdFtn:			// 0x06

                        break;
                    case ILOpCodes_Extended_LdVirtFtn:		// 0x07

                        break;
                    // 0x08 Doesn't exist
                    case ILOpCodes_Extended_LdArg:			// 0x09

                        break;
                    case ILOpCodes_Extended_LdArgA:			// 0x0A

                        break;
                    case ILOpCodes_Extended_StArg:			// 0x0B

                        break;
                    case ILOpCodes_Extended_LdLoc:			// 0x0C

                        break;
                    case ILOpCodes_Extended_LdLocA:			// 0x0D

                        break;
                    case ILOpCodes_Extended_StLoc:			// 0x0E

                        break;
                    case ILOpCodes_Extended_LocAlloc:		// 0x0F

                        break;
					// 0x10 Doesn't exist
                    case ILOpCodes_Extended_EndFilter:		// 0x11

                        break;
                    case ILOpCodes_Extended_Unaligned__:	// 0x12

                        break;
                    case ILOpCodes_Extended_Volitile__:		// 0x13

                        break;
                    case ILOpCodes_Extended_Tail__:			// 0x14

                        break;
                    case ILOpCodes_Extended_InitObj:		// 0x15

                        break;
                    case ILOpCodes_Extended_Constrained__:	// 0x16

                        break;
                    case ILOpCodes_Extended_CpBlk:			// 0x17

                        break;
                    case ILOpCodes_Extended_InitBlk:		// 0x18

                        break;
                    case ILOpCodes_Extended_No__:			// 0x19

                        break;
                    case ILOpCodes_Extended_ReThrow:		// 0x1A

                        break;
					// 0x1B Doesn't exist
                    case ILOpCodes_Extended_SizeOf:			// 0x1C

                        break;
                    case ILOpCodes_Extended_RefAnyType:		// 0x1D

                        break;
                    case ILOpCodes_Extended_ReadOnly__:		// 0x1E

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

uint32_t ReadUInt32(uint8_t** dat)
{
    uint32_t i = *((uint32_t*)*dat);
    *dat += 4;
    return i;
}
uint16_t ReadUInt16(uint8_t** dat)
{
	uint16_t i = *((uint16_t*)*dat);
    *dat += 2;
    return i;
}