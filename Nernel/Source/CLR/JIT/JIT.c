#include <CLR/JIT/JIT.h>
#include <stdio.h>
#include <stdlib.h>


void JIT_CompileMethod(IRMethod* mthd)
{
	char* AssmbledMthd = calloc(1, mthd->IRCodesCount * 128);
	char* mthPtr = AssmbledMthd;
	for (uint32_t i = 0; i < mthd->IRCodesCount; i++)
	{
		switch(mthd->IRCodes[i]->OpCode)
		{
			case IROpCode_Nop:
				JIT_Compile_Nop(mthd->IRCodes[i], AssmbledMthd);
				break;

			case IROpCode_Convert_Unchecked:
				JIT_Compile_Convert_Unchecked(mthd->IRCodes[i], AssmbledMthd);
				break;

		}
	}
	mthd->AssembledMethod = ((void(*)())mthPtr);
}