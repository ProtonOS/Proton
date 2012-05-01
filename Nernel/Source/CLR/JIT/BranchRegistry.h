#pragma once

typedef struct _BranchRegistry BranchRegistry;

struct _BranchRegistry
{
	uint32_t InstructionCount;

	size_t*	InstructionLocations;

	uint32_t*	TargetLocations;
	char**		BranchLocations;

	bool_t*		SpecialBranches;
};


BranchRegistry* BranchRegistry_Create(uint32_t pInstructionCount);
void BranchRegistry_Destroy(BranchRegistry* pBranchRegistry);
void BranchRegistry_RegisterBranch(BranchRegistry* pBranchRegistry, uint32_t pInstructionLocation, uint32_t pTargetLocation, char* pBranchLocation);
void BranchRegistry_RegisterSpecialBranch(BranchRegistry* pBranchRegistry, uint32_t pInstructionLocation, uint32_t pTargetLocation, char* pBranchLocation);
