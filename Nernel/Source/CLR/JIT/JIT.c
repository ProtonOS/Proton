#include <CLR/JIT/JIT.h>
#include <stdio.h>
#include <stdlib.h>


void JIT_CompileMethod(IRMethod* mthd)
{
	char* compMthd = calloc(1, mthd->IRCodesCount * 128);
	char* mthPtr = compMthd;

	compMthd = JIT_Emit_Prologue(mthd, compMthd);

	for (uint32_t i = 0; i < mthd->IRCodesCount; i++)
	{
		// Because of the fact that IROpCode is an enum
		// We know that we have all the op-codes at compile-time.
		switch(mthd->IRCodes[i]->OpCode)
		{
			case IROpCode_Nop:
				compMthd = JIT_Compile_Nop						(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_BreakForDebugger:
				compMthd = JIT_Compile_BreakForDebugger			(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Return:
				compMthd = JIT_Compile_Return					(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_LoadInt32_Val:
				compMthd = JIT_Compile_LoadInt32_Val			(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_LoadInt64_Val:
				compMthd = JIT_Compile_LoadInt64_Val			(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_LoadFloat32_Val:
				compMthd = JIT_Compile_LoadFloat32_Val			(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_LoadFloat64_Val:
				compMthd = JIT_Compile_LoadFloat64_Val			(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Branch:
				compMthd = JIT_Compile_Branch					(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Jump:
				compMthd = JIT_Compile_Jump						(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Store_LocalVar:
				compMthd = JIT_Compile_Store_LocalVar			(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Load_LocalVar:
				compMthd = JIT_Compile_Load_LocalVar			(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Load_LocalVar_Address:
				compMthd = JIT_Compile_Load_LocalVar_Address	(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Convert_OverflowCheck:
				compMthd = JIT_Compile_Convert_OverflowCheck	(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Convert_Unchecked:
				compMthd = JIT_Compile_Convert_Unchecked		(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Load_Parameter:
				compMthd = JIT_Compile_Load_Parameter			(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Load_Parameter_Address:
				compMthd = JIT_Compile_Load_Parameter_Address	(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Store_Parameter:
				compMthd = JIT_Compile_Store_Parameter			(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Load_Element:
				compMthd = JIT_Compile_Load_Element				(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Store_Element:
				compMthd = JIT_Compile_Store_Element			(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Load_Array_Length:
				compMthd = JIT_Compile_Load_Array_Length		(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Pop:
				compMthd = JIT_Compile_Pop						(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Shift:
				compMthd = JIT_Compile_Shift					(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Add:
				compMthd = JIT_Compile_Add						(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Sub:
				compMthd = JIT_Compile_Sub						(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Mul:
				compMthd = JIT_Compile_Mul						(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Div:
				compMthd = JIT_Compile_Div						(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Rem:
				compMthd = JIT_Compile_Rem						(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_LoadIndirect:
				compMthd = JIT_Compile_LoadIndirect				(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_StoreIndirect:
				compMthd = JIT_Compile_StoreIndirect			(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_Call:
				compMthd = JIT_Compile_Call						(mthd->IRCodes[i], compMthd, mthd);
				break;
			case IROpCode_LoadNull:
				compMthd = JIT_Compile_LoadNull					(mthd->IRCodes[i], compMthd, mthd);
				break;
		}
	}

	compMthd = JIT_Emit_Epilogue(mthd, compMthd);

	mthd->AssembledMethod = ((void(*)())mthPtr);
}