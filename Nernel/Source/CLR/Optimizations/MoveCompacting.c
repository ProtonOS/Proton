#include <CLR/Optimizations/MoveCompacting.h>

void IROptimizer_MoveCompacting(IRMethod* pMethod)
{
	uint32_t* localUseCount = (uint32_t*)calloc(1, sizeof(uint32_t) * pMethod->LocalVariableCount);
	for (uint32_t i = 0; i < pMethod->IRCodesCount; i++)
	{
		IRInstruction* instr = pMethod->IRCodes[i];
		
	}
}