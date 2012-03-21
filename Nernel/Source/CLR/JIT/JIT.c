#include <CLR/JIT/JIT.h>
#include <stdio.h>
#include <stdlib.h>
#include <Core/Console.h>
#include <CLR/Log.h>
#include <CLR/ILReader.h>
#include <CLR/IROptimizer.h>


void DumpMethod(IRMethod* mth, uint32_t len);
void JIT_CompileMethod(IRMethod* mthd)
{
	Log_WriteLine(LogFlags_JIT, "JITing method %s.%s.%s", mthd->MethodDefinition->TypeDefinition->Namespace, mthd->MethodDefinition->TypeDefinition->Name, mthd->MethodDefinition->Name);
	DecomposeMethod(mthd);
	Log_WriteLine(LogFlags_JIT, "Finished decomposition.");
	IROptimizer_Optimize(mthd);
	Log_WriteLine(LogFlags_JIT, "Applied Optimizations.");
	char* compMthd = malloc(mthd->IRCodesCount * 128);
	Log_WriteLine(LogFlags_JIT, "Address of JIT'd Method: 0x%x Size: 0x%x", (unsigned int)compMthd, mthd->IRCodesCount * 128);
	char* mthPtr = compMthd;
	mthd->AssembledMethod = ((void(*)())((unsigned int)mthPtr));

	BranchRegistry* branchRegistry = BranchRegistry_Create(mthd->IRCodes[mthd->IRCodesCount - 1]->InstructionLocation);
	
	JIT_Layout_Parameters(mthd);
	JIT_Layout_LocalVariables(mthd);


	compMthd = JIT_Emit_Prologue(mthd, compMthd);
	Log_WriteLine(LogFlags_JIT, "IRCodesCount: %i", (int)mthd->IRCodesCount);

	for (uint32_t i = 0; i < mthd->IRCodesCount; i++)
	{
		Log_WriteLine(LogFlags_JIT, "OpCode: %u being emitted at 0x%x", (unsigned int)mthd->IRCodes[i]->OpCode, (unsigned int)compMthd);
		branchRegistry->InstructionLocations[mthd->IRCodes[i]->InstructionLocation] = (size_t)compMthd;
		// Because of the fact that IROpCode is an enum
		// We know that we have all the op-codes at compile-time.
		switch(mthd->IRCodes[i]->OpCode)
		{
			case IROpCode_Nop:
				compMthd = JIT_Compile_Nop						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_BreakForDebugger:
				compMthd = JIT_Compile_BreakForDebugger			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Return:
				compMthd = JIT_Compile_Return					(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_LoadInt32_Val:
				compMthd = JIT_Compile_LoadInt32_Val			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_LoadInt64_Val:
				compMthd = JIT_Compile_LoadInt64_Val			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_LoadFloat32_Val:
				compMthd = JIT_Compile_LoadFloat32_Val			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_LoadFloat64_Val:
				compMthd = JIT_Compile_LoadFloat64_Val			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Branch:
				compMthd = JIT_Compile_Branch					(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Optimized_Jump:
				compMthd = JIT_Compile_Optimized_Jump			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Store_LocalVar:
				compMthd = JIT_Compile_Store_LocalVar			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_LocalVar:
				compMthd = JIT_Compile_Load_LocalVar			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_LocalVar_Address:
				compMthd = JIT_Compile_Load_LocalVar_Address	(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Convert_OverflowCheck:
				compMthd = JIT_Compile_Convert_OverflowCheck	(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Convert_Unchecked:
				compMthd = JIT_Compile_Convert_Unchecked		(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_Parameter:
				compMthd = JIT_Compile_Load_Parameter			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_Parameter_Address:
				compMthd = JIT_Compile_Load_Parameter_Address	(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Store_Parameter:
				compMthd = JIT_Compile_Store_Parameter			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_Element:
				compMthd = JIT_Compile_Load_Element				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_Element_Evil:
				compMthd = JIT_Compile_Load_Element_Evil		(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_Element_Address:
				compMthd = JIT_Compile_Load_Element_Address		(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Store_Element:
				compMthd = JIT_Compile_Store_Element			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Store_Element_Evil:
				compMthd = JIT_Compile_Store_Element_Evil		(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_Array_Length:
				compMthd = JIT_Compile_Load_Array_Length		(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Pop:
				compMthd = JIT_Compile_Pop						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Shift:
				compMthd = JIT_Compile_Shift					(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Add:
				compMthd = JIT_Compile_Add						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Sub:
				compMthd = JIT_Compile_Sub						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Mul:
				compMthd = JIT_Compile_Mul						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Div:
				compMthd = JIT_Compile_Div						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Rem:
				compMthd = JIT_Compile_Rem						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_LoadIndirect:
				compMthd = JIT_Compile_LoadIndirect				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_StoreIndirect:
				compMthd = JIT_Compile_StoreIndirect			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_LoadNull:
				compMthd = JIT_Compile_LoadNull					(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_NewObject:
				compMthd = JIT_Compile_NewObject				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_NewArray:
				compMthd = JIT_Compile_NewArray					(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Dup:
				compMthd = JIT_Compile_Dup						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_And:
				compMthd = JIT_Compile_And						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Or:
				compMthd = JIT_Compile_Or						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_XOr:
				compMthd = JIT_Compile_XOr						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Neg:
				compMthd = JIT_Compile_Neg						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Not:
				compMthd = JIT_Compile_Not						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_String:
				compMthd = JIT_Compile_Load_String				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_Field:
				compMthd = JIT_Compile_Load_Field				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_Field_Address:
				compMthd = JIT_Compile_Load_Field_Address		(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Store_Field:
				compMthd = JIT_Compile_Store_Field				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_Static_Field:
				compMthd = JIT_Compile_Load_Static_Field		(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_Static_Field_Address:
				compMthd = JIT_Compile_Load_Static_Field_Address(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Store_Static_Field:
				compMthd = JIT_Compile_Store_Static_Field		(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_Object:
				compMthd = JIT_Compile_Load_Object				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Store_Object:
				compMthd = JIT_Compile_Store_Object				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Copy_Object:
				compMthd = JIT_Compile_Copy_Object				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Switch:
				compMthd = JIT_Compile_Switch					(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_CastClass:
				compMthd = JIT_Compile_CastClass				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_IsInst:
				compMthd = JIT_Compile_IsInst					(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Unbox:
				compMthd = JIT_Compile_Unbox					(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Unbox_Any:
				compMthd = JIT_Compile_Unbox_Any				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Box:
				compMthd = JIT_Compile_Box						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Compare:
				compMthd = JIT_Compile_Compare					(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_CheckFinite:
				compMthd = JIT_Compile_CheckFinite				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_LocalAllocate:
				compMthd = JIT_Compile_LocalAllocate			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_InitObject:
				compMthd = JIT_Compile_InitObject				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_SizeOf:
				compMthd = JIT_Compile_SizeOf					(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_LoadFunction:
				compMthd = JIT_Compile_LoadFunction				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Load_Virtual_Function:
				compMthd = JIT_Compile_LoadVirtualFunction		(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Copy_Block:
				compMthd = JIT_Compile_Copy_Block				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Init_Block:
				compMthd = JIT_Compile_Init_Block				(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Call:
				compMthd = JIT_Compile_Call						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Call_Absolute:
				compMthd = JIT_Compile_Call_Absolute			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Call_Internal:
				compMthd = JIT_Compile_Call_Internal			(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Jump:
				compMthd = JIT_Compile_Jump						(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
			case IROpCode_Throw:
				compMthd = JIT_Compile_Throw					(mthd->IRCodes[i], compMthd, mthd, branchRegistry);
				break;
		}
	}


	compMthd = JIT_Emit_Epilogue(mthd, compMthd);

	compMthd = JIT_LinkBranches(compMthd, branchRegistry, mthd->IRCodes[mthd->IRCodesCount - 1]->InstructionLocation);

	BranchRegistry_Destroy(branchRegistry);

	DumpMethod(mthd, mthd->IRCodesCount * 128);
	Log_WriteLine(LogFlags_JIT, "Finished JITing method %s.%s.%s", mthd->MethodDefinition->TypeDefinition->Namespace, mthd->MethodDefinition->TypeDefinition->Name, mthd->MethodDefinition->Name);
}

void DumpMethod(IRMethod* mth, uint32_t len)
{
	/*Console_WriteString("Dumping Compiled Method", 23);
	Console_WriteString((const char*)&len, 4);
	for (uint32_t i = 0; i < len; i++)
	{
		Console_WriteCharacter(((char*)mth->AssembledMethod)[i]);
	}
	Console_WriteLine("FinishedDumpingMethod");*/
}
