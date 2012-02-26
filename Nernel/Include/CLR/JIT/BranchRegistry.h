#pragma once

typedef struct _BranchRegistry BranchRegistry;

struct _BranchRegistry
{
	size_t*	InstructionLocations;

	uint32_t*	TargetLocations;
	char**		BranchLocations;
};


BranchRegistry* BranchRegistry_Create(uint32_t pInstructionCount);
void BranchRegistry_Destroy(BranchRegistry* pBranchRegistry);
void BranchRegistry_RegisterBranchForLink(BranchRegistry* pBranchReg, uint32_t pCurrentLocation, uint32_t pTargetLocation, char* pBranchLocation);
char* BranchRegistry_GetInstructionLocation(BranchRegistry* pBranchReg, uint32_t pInstructionLocation);