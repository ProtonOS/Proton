#include <CLR/JIT/BranchRegistry.h>

BranchRegistry* BranchRegistry_Create(uint32_t pInstructionCount)
{
	BranchRegistry* branchRegistry = (BranchRegistry*)calloc(1, sizeof(BranchRegistry));
	branchRegistry->InstructionCount = pInstructionCount;
	branchRegistry->InstructionLocations = (size_t*)calloc(1, sizeof(size_t) * (pInstructionCount + 2));
	branchRegistry->TargetLocations = (uint32_t*)calloc(1, sizeof(uint32_t) * (pInstructionCount + 1));
	branchRegistry->BranchLocations = (char**)calloc(1, sizeof(char*) * (pInstructionCount + 1));
	branchRegistry->SpecialBranches = (bool_t*)calloc(1, sizeof(bool_t) * (pInstructionCount + 1));
	return branchRegistry;
}

void BranchRegistry_Destroy(BranchRegistry* pBranchRegistry)
{
	free(pBranchRegistry->InstructionLocations);
	free(pBranchRegistry->TargetLocations);
	free(pBranchRegistry->BranchLocations);
	free(pBranchRegistry->SpecialBranches);
	free(pBranchRegistry);
}


void BranchRegistry_RegisterBranch(BranchRegistry* pBranchRegistry, uint32_t pInstructionLocation, uint32_t pTargetLocation, char* pBranchLocation)
{
	pBranchRegistry->BranchLocations[pInstructionLocation] = pBranchLocation;
	pBranchRegistry->TargetLocations[pInstructionLocation] = pTargetLocation;
}


void BranchRegistry_RegisterSpecialBranch(BranchRegistry* pBranchRegistry, uint32_t pInstructionLocation, uint32_t pTargetLocation, char* pBranchLocation)
{
	pBranchRegistry->BranchLocations[pInstructionLocation] = pBranchLocation;
	pBranchRegistry->TargetLocations[pInstructionLocation] = pTargetLocation;
	pBranchRegistry->SpecialBranches[pInstructionLocation] = TRUE;
}
