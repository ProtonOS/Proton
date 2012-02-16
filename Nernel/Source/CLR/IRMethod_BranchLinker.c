#include <CLR/IRMethod_BranchLinker.h>
#include <CLR/OpCodes_IR.h>
#include <CLR/Log.h>
#include <stdlib.h>

void Panic(const char* msg);

#define DEFINE_BRANCH(cond, offset) \
	(mthd->IRCodes[curLoc cond offset]->InstructionLocation == targetInstrPos) \
    { \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "Located Branch. curLoc: 0x%x, targetInstrPos: 0x%x, AddAmnt: %i", (unsigned int)(curLoc cond offset), (unsigned int)targetInstrPos, (int)AddAmnt); \
        targetInstruction = mthd->IRCodes[curLoc cond offset]; \
        foundTarget = TRUE; \
    } 

#define FIND_IF_NEAR(cond) \
    if DEFINE_BRANCH(cond, 1) \
    else if DEFINE_BRANCH(cond, 2) \
    else if DEFINE_BRANCH(cond, 3) \
    else if DEFINE_BRANCH(cond, 4) \
    else if DEFINE_BRANCH(cond, 5) \
    else if DEFINE_BRANCH(cond, 6) \
    else if DEFINE_BRANCH(cond, 7) \
    else if DEFINE_BRANCH(cond, 8) \
    else if DEFINE_BRANCH(cond, 9) \
    else \
    { \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "Failed to locate branch. cond: " #cond); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 1]->InstructionLocation: 0x%x", (unsigned int)(mthd->IRCodes[curLoc cond 1]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 2]->InstructionLocation: 0x%x", (unsigned int)(mthd->IRCodes[curLoc cond 2]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 3]->InstructionLocation: 0x%x", (unsigned int)(mthd->IRCodes[curLoc cond 3]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 4]->InstructionLocation: 0x%x", (unsigned int)(mthd->IRCodes[curLoc cond 4]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 5]->InstructionLocation: 0x%x", (unsigned int)(mthd->IRCodes[curLoc cond 5]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 6]->InstructionLocation: 0x%x", (unsigned int)(mthd->IRCodes[curLoc cond 6]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 7]->InstructionLocation: 0x%x", (unsigned int)(mthd->IRCodes[curLoc cond 7]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 8]->InstructionLocation: 0x%x", (unsigned int)(mthd->IRCodes[curLoc cond 8]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 9]->InstructionLocation: 0x%x", (unsigned int)(mthd->IRCodes[curLoc cond 9]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "curLoc: 0x%x, MethodLength (In IR Codes): 0x%x, targetInstrPos: 0x%x, AddAmnt: %i", (unsigned int)curLoc, (unsigned int)mthd->IRCodesCount, (unsigned int)targetInstrPos, (int)AddAmnt); \
        Panic("Unable to resolve target of branch!"); \
    }

void IRMethod_BranchLinker_LinkMethod(IRMethod* mthd)
{
    for (uint32_t i = 0; i < mthd->IRCodesCount; i++)
    {
        if (mthd->IRCodes[i]->OpCode == IROpCode_Branch)
        {
            Log_WriteLine(LogFlags_ILReading_BranchLinker, "Starting Link attempt, Source Instruction Position: 0x%x", (unsigned int)mthd->IRCodes[i]->InstructionLocation);
            uint32_t targetInstrPos = *((uint32_t*)(mthd->IRCodes[i]->Arg2));
            uint32_t AddAmnt = mthd->IRCodesCount / 4;
            uint32_t curLoc = mthd->IRCodesCount / 2;
            IRInstruction* targetInstruction;
            bool_t foundTarget = FALSE;
            Log_WriteLine(LogFlags_ILReading_BranchLinker, "Starting Link attempt, target Instruction Position: 0x%x", (unsigned int)targetInstrPos);
            while (!foundTarget)
            {
                if (mthd->IRCodes[curLoc]->InstructionLocation > targetInstrPos)
                {
                    Log_WriteLine(LogFlags_ILReading_BranchLinker, "Instruction Location (0x%x) greater than target position, CurLoc: 0x%x", (unsigned int)mthd->IRCodes[curLoc]->InstructionLocation, (unsigned int)curLoc);
                    if (AddAmnt < 1)
                    {
                        FIND_IF_NEAR(-);
                    }
                    else
                    {
                        Log_WriteLine(LogFlags_ILReading_BranchLinker, "Subtracting: %i", (int)AddAmnt);
                        curLoc -= AddAmnt;
                        AddAmnt /= 2;
                    }
                }
                else if (mthd->IRCodes[curLoc]->InstructionLocation < targetInstrPos)
                {
                    Log_WriteLine(LogFlags_ILReading_BranchLinker, "Instruction Location (0x%x) less than target position, CurLoc: 0x%x", (unsigned int)mthd->IRCodes[curLoc]->InstructionLocation, (unsigned int)curLoc);
                    if (AddAmnt < 1)
                    {
                        FIND_IF_NEAR(+);
                    }
                    else
                    {
                        Log_WriteLine(LogFlags_ILReading_BranchLinker, "Adding: %i", (int)AddAmnt);
                        curLoc += AddAmnt;
                        AddAmnt /= 2;
                    }
                }
                else if (mthd->IRCodes[curLoc]->InstructionLocation == targetInstrPos)
                {
                    Log_WriteLine(LogFlags_ILReading_BranchLinker, "Located Branch. curLoc: 0x%x, targetInstrPos: 0x%x, AddAmnt: %i", (unsigned int)curLoc, (unsigned int)targetInstrPos, (int)AddAmnt);
                    targetInstruction = mthd->IRCodes[curLoc];
                    foundTarget = TRUE;
                }
            }
            targetInstruction->IsTargetOfBranch = TRUE;
            free(mthd->IRCodes[i]->Arg2);
            mthd->IRCodes[i]->Arg2 = targetInstruction;
			mthd->IRCodes[i]->Arg2NeedsDisposing = FALSE;

        }
    }
}