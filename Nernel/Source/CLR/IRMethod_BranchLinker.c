#include <CLR/IRMethod_BranchLinker.h>
#include <CLR/OpCodes_IR.h>
#include <CLR/Log.h>
#include <stdlib.h>

void Panic(const char* msg);

#define FIND_IF_NEAR(cond) \
    if (mthd->IRCodes[curLoc cond 1]->InstructionLocation == targetInstrPos) \
    { \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "Located Branch. curLoc: %i, targetInstrPos: %i, AddAmnt: %i", (int)(curLoc cond 1), (int)targetInstrPos, (int)AddAmnt); \
        targetInstruction = mthd->IRCodes[curLoc cond 1]; \
        foundTarget = TRUE; \
    } \
    else if (mthd->IRCodes[curLoc cond 2]->InstructionLocation == targetInstrPos) \
    { \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "Located Branch. curLoc: %i, targetInstrPos: %i, AddAmnt: %i", (int)(curLoc cond 2), (int)targetInstrPos, (int)AddAmnt); \
        targetInstruction = mthd->IRCodes[curLoc cond 2]; \
        foundTarget = TRUE; \
    } \
    else if (mthd->IRCodes[curLoc cond 3]->InstructionLocation == targetInstrPos) \
    { \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "Located Branch. curLoc: %i, targetInstrPos: %i, AddAmnt: %i", (int)(curLoc cond 3), (int)targetInstrPos, (int)AddAmnt); \
        targetInstruction = mthd->IRCodes[curLoc cond 3]; \
        foundTarget = TRUE; \
    } \
    else if (mthd->IRCodes[curLoc cond 4]->InstructionLocation == targetInstrPos) \
    { \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "Located Branch. curLoc: %i, targetInstrPos: %i, AddAmnt: %i", (int)(curLoc cond 4), (int)targetInstrPos, (int)AddAmnt); \
        targetInstruction = mthd->IRCodes[curLoc cond 4]; \
        foundTarget = TRUE; \
    } \
    else \
    { \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "Failed to locate branch. cond: " #cond); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 1]->InstructionLocation: %i", (int)(mthd->IRCodes[curLoc cond 1]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 2]->InstructionLocation: %i", (int)(mthd->IRCodes[curLoc cond 2]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 3]->InstructionLocation: %i", (int)(mthd->IRCodes[curLoc cond 3]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "mthd->IRCodes[curLoc " #cond " 4]->InstructionLocation: %i", (int)(mthd->IRCodes[curLoc cond 4]->InstructionLocation)); \
        Log_WriteLine(LogFlags_ILReading_BranchLinker, "curLoc: %i, MethodLength: %i, targetInstrPos: %i, AddAmnt: %i", (int)curLoc, (int)mthd->IRCodesCount, (int)targetInstrPos, (int)AddAmnt); \
        Panic("Unable to resolve target of branch!"); \
    }

void IRMethod_BranchLinker_LinkMethod(IRMethod* mthd)
{
    for (uint32_t i = 0; i < mthd->IRCodesCount; i++)
    {
        if (mthd->IRCodes[i]->OpCode == IROpCode_Branch)
        {
            uint32_t targetInstrPos = *((uint32_t*)(mthd->IRCodes[i]->Arg2));
            uint32_t AddAmnt = mthd->IRCodesCount / 4;
            uint32_t curLoc = mthd->IRCodesCount / 2;
            IRInstruction* targetInstruction;
            bool_t foundTarget = FALSE;
            Log_WriteLine(LogFlags_ILReading_BranchLinker, "Starting Link attempt, target Instruction Position: %i", (int)targetInstrPos);
            while (!foundTarget)
            {
                if (mthd->IRCodes[curLoc]->InstructionLocation > targetInstrPos)
                {
                    Log_WriteLine(LogFlags_ILReading_BranchLinker, "Instruction Location (%i) greater than target position, CurLoc: %i", (int)mthd->IRCodes[curLoc]->InstructionLocation, (int)curLoc);
                    if (AddAmnt < 3)
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
                    Log_WriteLine(LogFlags_ILReading_BranchLinker, "Instruction Location (%i) less than target position, CurLoc: %i", (int)mthd->IRCodes[curLoc]->InstructionLocation, (int)curLoc);
                    if (AddAmnt < 3)
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
                    Log_WriteLine(LogFlags_ILReading_BranchLinker, "Located Branch. curLoc: %i, targetInstrPos: %i, AddAmnt: %i", (int)curLoc, (int)targetInstrPos, (int)AddAmnt);
                    targetInstruction = mthd->IRCodes[curLoc];
                    foundTarget = TRUE;
                }
            }
            targetInstruction->IsTargetOfBranch = TRUE;
            free(mthd->IRCodes[i]->Arg2);
            mthd->IRCodes[i]->Arg2 = targetInstruction;

        }
    }
}