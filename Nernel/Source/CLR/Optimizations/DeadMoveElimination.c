#include <CLR/Optimizations/DeadMoveElimination.h>

void IROptimizer_DeadMoveElimination(IRMethod* pMethod)
{
	for (uint32_t i = 0; i < pMethod->IRCodesCount; i++)
	{
		IRInstruction* instr = pMethod->IRCodes[i];
		if (instr->Opcode == IROpcode_Move)
		{
			if (!memcmp(&instr->Source1, &instr->Destination, sizeof(SourceTypeData)))
			{
				instr->Opcode = IROpcode_Nop;
			}
		}
	}
}