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
		switch(mthd->IRCodes[i]->OpCode)
		{
			case IROpCode_Nop:
				compMthd = JIT_Compile_Nop(mthd->IRCodes[i], compMthd);
				break;

			case IROpCode_Convert_Unchecked:
				compMthd = JIT_Compile_Convert_Unchecked(mthd->IRCodes[i], compMthd);
				break;

		}
	}

	compMthd = JIT_Emit_Epilogue(mthd, compMthd);

	mthd->AssembledMethod = ((void(*)())mthPtr);
}