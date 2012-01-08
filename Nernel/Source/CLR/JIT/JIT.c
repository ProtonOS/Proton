#include <CLR/JIT/JIT.h>
#include <stdio.h>
#include <stdlib.h>


void JIT_CompileMethod(IRMethod* mthd)
{
	char* AssmbledMthd = calloc(1, mthd->IRCodesCount * 128);
	for (uint32_t i = 0; i < mthd->IRCodesCount; i++)
	{

	}
}