#include <CLR/Optimizations/GenericCleanup.h>

void IROptimizer_GenericCleanup(IRMethod* pMethod)
{
	for (uint32_t i = 0; i < pMethod->IRCodesCount; i++)
	{
		IRInstruction* ins = pMethod->IRCodes[i];
		switch(ins->Opcode)
		{
			case IROpcode_Move:
				if (ins->Source1.Type == SourceType_ArrayElement)
				{
					if (ins->Source1.Data.ArrayElement.ElementType->IsStructureType)
					{
						ins->Source1.Type = SourceType_ArrayElementAddress;
					}
				}
				break;
			default: break;
		}
	}
}