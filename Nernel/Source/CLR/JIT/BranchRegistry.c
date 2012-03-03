#include <CLR/JIT/BranchRegistry.h>
#include <stdlib.h>
#include <stdio.h>

BranchRegistry* BranchRegistry_Create(uint32_t pInstructionCount)
{
	BranchRegistry* branchRegistry = (BranchRegistry*)calloc(1, sizeof(BranchRegistry));
	if (pInstructionCount > 0)
	{
		//branchRegistry->InstructionLocations = (size_t*)calloc(1, 0);
		//branchRegistry->InstructionLocations = (size_t*)calloc(1, sizeof(size_t) * pInstructionCount);
		//printf("InstructionLocations: 0x%x\n", (unsigned int)branchRegistry->InstructionLocations);
		////branchRegistry->InstructionLocations = branchRegistry->InstructionLocations;
		//free(branchRegistry->InstructionLocations);
		branchRegistry->InstructionLocations = (size_t*)calloc(1, sizeof(size_t) * pInstructionCount);
		branchRegistry->TargetLocations = (uint32_t*)calloc(1, sizeof(uint32_t) * pInstructionCount);
		branchRegistry->BranchLocations = (char**)calloc(1, sizeof(char*) * pInstructionCount);
		free(branchRegistry->InstructionLocations);
		printf("Freed Instructions\n");
		free(branchRegistry->TargetLocations);
		printf("Freed Targets\n");
		free(branchRegistry->BranchLocations);
		printf("Freed Branches\n");
		branchRegistry->InstructionLocations = (size_t*)calloc(1, sizeof(size_t) * pInstructionCount);
		printf("InstructionLocations: 0x%x\n", (unsigned int)branchRegistry->InstructionLocations);
		////branchRegistry->InstructionLocations = branchRegistry->InstructionLocations;
		//free(branchRegistry->InstructionLocations);
		branchRegistry->TargetLocations = (uint32_t*)calloc(1, sizeof(uint32_t) * pInstructionCount);
		printf("TargetLocations: 0x%x\n", (unsigned int)branchRegistry->TargetLocations);
		branchRegistry->BranchLocations = (char**)calloc(1, sizeof(char*) * pInstructionCount);
		printf("BranchLocations: 0x%x\n", (unsigned int)branchRegistry->BranchLocations);
	}
	printf("Returning BranchReg @ 0x%x, InstructionCount = %u\n", (unsigned int)branchRegistry, (unsigned int)pInstructionCount);
	return branchRegistry;
}

void BranchRegistry_Destroy(BranchRegistry* pBranchRegistry)
{
	printf("Destroying BranchReg @ 0x%x\n", (unsigned int)pBranchRegistry);
	if (pBranchRegistry->InstructionLocations)
	{
		printf("Free InstructionLocations: 0x%x\n", (unsigned int)pBranchRegistry->InstructionLocations);
		free(pBranchRegistry->InstructionLocations);
		printf("Free TargetLocations: 0x%x\n", (unsigned int)pBranchRegistry->TargetLocations);
		free(pBranchRegistry->TargetLocations);
		printf("Free BranchLocations: 0x%x\n", (unsigned int)pBranchRegistry->BranchLocations);
		free(pBranchRegistry->BranchLocations);
	}
	free(pBranchRegistry);
}


void BranchRegistry_RegisterBranchForLink(BranchRegistry* pBranchReg, uint32_t pCurrentLocation, uint32_t pTargetLocation, char* pBranchLocation)
{
	pBranchReg->BranchLocations[pCurrentLocation] = pBranchLocation;
	pBranchReg->TargetLocations[pCurrentLocation] = pTargetLocation;
}

char* BranchRegistry_GetInstructionLocation(BranchRegistry* pBranchReg, uint32_t pInstructionLocation)
{
	return (char*)pBranchReg->InstructionLocations[pInstructionLocation];
}