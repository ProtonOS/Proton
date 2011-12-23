#include <CLR/OpCodes_IR.h>
#include <CLR/Optimizations/UnconditionalBranchTransform.h>
#include <stdlib.h>

void IROptimizer_UnconditionalBranchTransform(ILAssembly* asmb)
{
    for (uint32_t i = 0; i < asmb->IRAssembly->MethodCount; i++)
    {
        IRMethod* mth = asmb->IRAssembly->Methods[i];
        for (uint32_t i2 = 0; i2 < mth->IRCodesCount; i2++)
        {
            IRInstruction* instr = mth->IRCodes[i2];
            if (instr->OpCode == IROpCode_Branch)
            {
                if ( *((BranchCondition*)(instr->Arg1)) == BranchCondition_Always)
                {
                    instr->OpCode = IROpCode_Jump;
                    free(instr->Arg1);
                    instr->Arg1 = instr->Arg2;
                    instr->Arg2 = (void*)0;
                }
            }
        }
    }
}