#include <CLR/IROptimizer.h>
#include <CLR/Optimizations/EnterSSA.h>
#include <CLR/Optimizations/LeaveSSA.h>
#include <CLR/Optimizations/LinearizeStack.h>

void IROptimizer_Optimize(IRMethod* pMethod)
{
	IRBranch* branches = NULL;
	uint32_t branchesCount = 0;
	IROptimizer_ProcessBranches(pMethod, 0, &branches, &branchesCount);
	Log_WriteLine(LOGLEVEL__Optimize, "Found Convergences for %u Branches", (unsigned int)branchesCount);
	for (uint32_t index = 0; index < branchesCount; ++index)
	{
		IRBranch* branch = &branches[index];
		Log_WriteLine(LOGLEVEL__Optimize, "    Left 0x%x/0x%x:0x%x/0x%x, Right 0x%x/0x%x:0x%x/0x%x", (unsigned int)branch->FirstLeftInstruction, (unsigned int)branch->LeftConvergence, (unsigned int)pMethod->IRCodes[branch->FirstLeftInstruction]->ILLocation, (unsigned int)pMethod->IRCodes[branch->LeftConvergence]->ILLocation, (unsigned int)branch->FirstRightInstruction, (unsigned int)branch->RightConvergence, (unsigned int)pMethod->IRCodes[branch->FirstRightInstruction]->ILLocation, (unsigned int)pMethod->IRCodes[branch->RightConvergence]->ILLocation);
	}

	//IROptimizer_LinearizeStack(pMethod, branches, branchesCount);
	IROptimizer_EnterSSA(pMethod, branches, branchesCount);

	IROptimizer_LeaveSSA(pMethod, branches, branchesCount);

	free(branches);
}

uint32_t IROptimizer_ProcessBranches(IRMethod* pMethod, uint32_t pStartIndex, IRBranch** pBranches, uint32_t* pBranchesCount)
{
	IRInstruction* instruction = NULL;
	uint32_t index = pStartIndex;
	while (index < pMethod->IRCodesCount)
	{
		instruction = pMethod->IRCodes[index];
		if (instruction->Opcode == IROpcode_Return) return index;
		else if (instruction->Opcode == IROpcode_Branch)
		{
			if ((BranchCondition)(uint32_t)instruction->Arg1 == BranchCondition_Always) return ((IRInstruction*)instruction->Arg2)->IRLocation;
			else
			{
				bool_t alreadyAdded = FALSE;
				for (uint32_t searchIndex = 0; searchIndex < *pBranchesCount; ++searchIndex)
				{
					if ((*pBranches)[searchIndex].FirstLeftInstruction == index + 1 &&
						(*pBranches)[searchIndex].FirstRightInstruction == ((IRInstruction*)instruction->Arg2)->IRLocation)
					{
						alreadyAdded = TRUE;
						break;
					}
				}
				if (!alreadyAdded)
				{
					uint32_t branchIndex = *pBranchesCount;
					*pBranchesCount += 1;
					*pBranches = (IRBranch*)realloc(*pBranches, *pBranchesCount * sizeof(IRBranch));
					(*pBranches)[branchIndex].FirstLeftInstruction = index + 1;
					(*pBranches)[branchIndex].FirstRightInstruction = ((IRInstruction*)instruction->Arg2)->IRLocation;
					(*pBranches)[branchIndex].LeftConvergence = IROptimizer_ProcessBranches(pMethod, index + 1, pBranches, pBranchesCount);
					(*pBranches)[branchIndex].RightConvergence = IROptimizer_ProcessBranches(pMethod, ((IRInstruction*)instruction->Arg2)->IRLocation, pBranches, pBranchesCount);
				}
			}
		}
		else if (instruction->Opcode == IROpcode_Jump) Panic("Don't know how to handle this yet!");
		// Handle IROpcode_Throw as well, but not yet because we have throws in corlib
		++index;
	}
	return index;
}