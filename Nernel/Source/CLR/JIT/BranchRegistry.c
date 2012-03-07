#include <CLR/JIT/BranchRegistry.h>
#include <stdlib.h>
#include <stdio.h>

BranchRegistry* BranchRegistry_Create(uint32_t pInstructionCount)
{
	BranchRegistry* branchRegistry = (BranchRegistry*)calloc(1, sizeof(BranchRegistry));
	if (pInstructionCount > 0)
	{
		branchRegistry->InstructionLocations = (size_t*)calloc(1, sizeof(size_t) * (pInstructionCount + 1));
		branchRegistry->TargetLocations = (uint32_t*)calloc(1, sizeof(uint32_t) * (pInstructionCount + 1));
		branchRegistry->BranchLocations = (char**)calloc(1, sizeof(char*) * (pInstructionCount + 1));
		branchRegistry->SpecialBranch = (bool_t*)calloc(1, sizeof(bool_t) * (pInstructionCount + 1));
	}
	return branchRegistry;
}

void BranchRegistry_Destroy(BranchRegistry* pBranchRegistry)
{
	if (pBranchRegistry->InstructionLocations)
	{
		free(pBranchRegistry->InstructionLocations);
		free(pBranchRegistry->TargetLocations);
		free(pBranchRegistry->BranchLocations);
		free(pBranchRegistry->SpecialBranch);
	}
	free(pBranchRegistry);
}


void BranchRegistry_RegisterBranchForLink(BranchRegistry* pBranchReg, uint32_t pCurrentLocation, uint32_t pTargetLocation, char* pBranchLocation)
{
	pBranchReg->BranchLocations[pCurrentLocation] = pBranchLocation;
	pBranchReg->TargetLocations[pCurrentLocation] = pTargetLocation;
}


void BranchRegistry_RegisterSpecialBranchForLink(BranchRegistry* pBranchReg, uint32_t pCurrentLocation, uint32_t pTargetLocation, char* pBranchLocation)
{
	pBranchReg->BranchLocations[pCurrentLocation] = pBranchLocation;
	pBranchReg->TargetLocations[pCurrentLocation] = pTargetLocation;
	pBranchReg->SpecialBranch[pCurrentLocation] = TRUE;
}

char* BranchRegistry_GetInstructionLocation(BranchRegistry* pBranchReg, uint32_t pInstructionLocation)
{
	return (char*)pBranchReg->InstructionLocations[pInstructionLocation];
}