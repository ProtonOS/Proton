#pragma once


#define ClearFlags() \
    { prefixConstrained = FALSE; \
    prefixNo = FALSE; \
    prefixReadOnly = FALSE; \
    prefixTail = FALSE; \
    prefixUnaligned = FALSE; \
    prefixVolatile = FALSE; }

#define GetILInstructionOffset() \
    if (prefixConstrained) { instr->ILLocation -= 6; } \
    if (prefixNo) { instr->ILLocation -= 2; } \
    if (prefixReadOnly) { instr->ILLocation -= 2; } \
    if (prefixTail) { instr->ILLocation -= 2; } \
    if (prefixUnaligned) { instr->ILLocation -= 2; } \
    if (prefixVolatile) { instr->ILLocation -= 2; }

#define EMIT_IR(instrType) \
    { IRInstruction* instr = IRInstruction_Create((currentILInstructionBase - originalDataPointer), instrType); \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "instr->ILLocation = 0x%x", (unsigned int)instr->ILLocation); \
    GetILInstructionOffset(); \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "Emitting " #instrType); \
    IRMethod_AddInstruction(pMethod, instr); }
#define EMIT_IR_1ARG(instrType, arg1) \
    { IRInstruction* instr = IRInstruction_Create((currentILInstructionBase - originalDataPointer), instrType); \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "instr->ILLocation = 0x%x", (unsigned int)instr->ILLocation); \
    GetILInstructionOffset(); \
    instr->Arg1 = arg1; \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "Emitting " #instrType " With 1 argument ('" #arg1 "')"); \
    IRMethod_AddInstruction(pMethod, instr); }
#define EMIT_IR_2ARG(instrType, arg1, arg2) \
    { IRInstruction* instr = IRInstruction_Create((currentILInstructionBase - originalDataPointer), instrType); \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "instr->ILLocation = 0x%x", (unsigned int)instr->ILLocation); \
    GetILInstructionOffset(); \
    instr->Arg1 = arg1; \
    instr->Arg2 = arg2; \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "Emitting " #instrType " With 2 arguments ('" #arg1 "', '" #arg2 "')"); \
    IRMethod_AddInstruction(pMethod, instr); }
#define EMIT_IR_3ARG(instrType, arg1, arg2, arg3) \
    { IRInstruction* instr = IRInstruction_Create((currentILInstructionBase - originalDataPointer), instrType); \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "instr->ILLocation = 0x%x", (unsigned int)instr->ILLocation); \
    GetILInstructionOffset(); \
    instr->Arg1 = arg1; \
    instr->Arg2 = arg2; \
    instr->Arg3 = arg3; \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "Emitting " #instrType " With 3 arguments ('" #arg1 "', '" #arg2 "', '" #arg3 "')"); \
    IRMethod_AddInstruction(pMethod, instr); }
#define EMIT_IR_4ARG(instrType, arg1, arg2, arg3, arg4) \
    { IRInstruction* instr = IRInstruction_Create((currentILInstructionBase - originalDataPointer), instrType); \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "instr->ILLocation = 0x%x", (unsigned int)instr->ILLocation); \
    GetILInstructionOffset(); \
    instr->Arg1 = arg1; \
    instr->Arg2 = arg2; \
    instr->Arg3 = arg3; \
    instr->Arg4 = arg4; \
	Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "Emitting " #instrType " With 4 arguments ('" #arg1 "', '" #arg2 "', '" #arg3 "', '" #arg4 "')"); \
    IRMethod_AddInstruction(pMethod, instr); }

#define EMIT_IR_1ARG_NO_DISPOSE(instrType, arg1) \
    { IRInstruction* instr = IRInstruction_Create((currentILInstructionBase - originalDataPointer), instrType); \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "instr->ILLocation = 0x%x", (unsigned int)instr->ILLocation); \
    GetILInstructionOffset(); \
    instr->Arg1 = arg1; \
	instr->Arg1NeedsDisposing = FALSE; \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "Emitting " #instrType " With 1 argument ('" #arg1 "') and not disposing of it"); \
    IRMethod_AddInstruction(pMethod, instr); }
#define EMIT_IR_2ARG_NO_DISPOSE(instrType, arg1, arg2) \
    { IRInstruction* instr = IRInstruction_Create((currentILInstructionBase - originalDataPointer), instrType); \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "instr->ILLocation = 0x%x", (unsigned int)instr->ILLocation); \
    GetILInstructionOffset(); \
    instr->Arg1 = arg1; \
	instr->Arg1NeedsDisposing = FALSE; \
    instr->Arg2 = arg2; \
	instr->Arg2NeedsDisposing = FALSE; \
	Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "Emitting " #instrType " With 2 arguments ('" #arg1 "', '" #arg2"') and not disposing of either of them."); \
    IRMethod_AddInstruction(pMethod, instr); }
#define EMIT_IR_3ARG_NO_DISPOSE(instrType, arg1, arg2, arg3) \
    { IRInstruction* instr = IRInstruction_Create((currentILInstructionBase - originalDataPointer), instrType); \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "instr->ILLocation = 0x%x", (unsigned int)instr->ILLocation); \
    GetILInstructionOffset(); \
    instr->Arg1 = arg1; \
	instr->Arg1NeedsDisposing = FALSE; \
    instr->Arg2 = arg2; \
	instr->Arg2NeedsDisposing = FALSE; \
    instr->Arg3 = arg3; \
	instr->Arg3NeedsDisposing = FALSE; \
	Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "Emitting " #instrType " With 3 arguments ('" #arg1 "', '" #arg2"', '" #arg3 "') and not disposing of any of them."); \
    IRMethod_AddInstruction(pMethod, instr); }
#define EMIT_IR_4ARG_NO_DISPOSE(instrType, arg1, arg2, arg3, arg4) \
    { IRInstruction* instr = IRInstruction_Create((currentILInstructionBase - originalDataPointer), instrType); \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "instr->ILLocation = 0x%x", (unsigned int)instr->ILLocation); \
    GetILInstructionOffset(); \
    instr->Arg1 = arg1; \
	instr->Arg1NeedsDisposing = FALSE; \
    instr->Arg2 = arg2; \
	instr->Arg2NeedsDisposing = FALSE; \
    instr->Arg3 = arg3; \
	instr->Arg3NeedsDisposing = FALSE; \
    instr->Arg4 = arg4; \
	instr->Arg4NeedsDisposing = FALSE; \
	Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "Emitting " #instrType " With 4 arguments ('" #arg1 "', '" #arg2"', '" #arg3 "', '" #arg4 "') and not disposing of any of them."); \
    IRMethod_AddInstruction(pMethod, instr); }

#define EMIT_IR_2ARG_DISPOSE__NO_DISPOSE(instrType, arg1, arg2) \
    { IRInstruction* instr = IRInstruction_Create((currentILInstructionBase - originalDataPointer), instrType); \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "instr->ILLocation = 0x%x", (unsigned int)instr->ILLocation); \
    GetILInstructionOffset(); \
    instr->Arg1 = arg1; \
    instr->Arg2 = arg2; \
	instr->Arg2NeedsDisposing = FALSE; \
    Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "Emitting " #instrType " With 2 arguments ('" #arg1 "', '" #arg2 "')"); \
    IRMethod_AddInstruction(pMethod, instr); }
