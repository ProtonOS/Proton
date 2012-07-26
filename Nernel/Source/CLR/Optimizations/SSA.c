#include <CLR/Optimizations/SSA.h>

void IROptimizer_RetargetLocalSources(IRMethod* pMethod, IRCodeNode* pCodeNode, uint32_t pStartIndex, IRLocalVariable* pOldLocalVariable, IRLocalVariable* pNewLocalVariable, bool_t* pRetargettedNodes)
{
	IRInstruction* instruction = NULL;
	IRPhi* phi = NULL;
	for (uint32_t index = 0; index < pCodeNode->PhiFunctionsCount; ++index)
	{
		phi = pCodeNode->PhiFunctions[index];
		for (uint32_t argIndex = 0; argIndex < phi->ArgumentsCount; ++argIndex)
		{
			if (phi->Arguments[argIndex] == pOldLocalVariable)
				phi->Arguments[argIndex] = pNewLocalVariable;
		}
	}
	for (uint32_t index = pStartIndex; index < pCodeNode->InstructionsCount; ++index)
	{
		instruction = pMethod->IRCodes[pCodeNode->Instructions[index]];
		if (instruction->Destination.Type == SourceType_Local &&
			instruction->Destination.Data.LocalVariable.LocalVariableIndex == pOldLocalVariable->LocalVariableIndex)
		{
			instruction->Destination.Data.LocalVariable.LocalVariableIndex = pNewLocalVariable->LocalVariableIndex;
		}
		else if (instruction->Destination.Type == SourceType_LocalAddress &&
				 instruction->Destination.Data.LocalVariableAddress.LocalVariableIndex == pOldLocalVariable->LocalVariableIndex)
		{
			instruction->Destination.Data.LocalVariableAddress.LocalVariableIndex = pNewLocalVariable->LocalVariableIndex;
		}
		if (instruction->Source1.Type == SourceType_Local &&
			instruction->Source1.Data.LocalVariable.LocalVariableIndex == pOldLocalVariable->LocalVariableIndex)
		{
			instruction->Source1.Data.LocalVariable.LocalVariableIndex = pNewLocalVariable->LocalVariableIndex;
		}
		else if (instruction->Source1.Type == SourceType_LocalAddress &&
				 instruction->Source1.Data.LocalVariableAddress.LocalVariableIndex == pOldLocalVariable->LocalVariableIndex)
		{
			instruction->Source1.Data.LocalVariableAddress.LocalVariableIndex = pNewLocalVariable->LocalVariableIndex;
		}
		if (instruction->Source2.Type == SourceType_Local &&
			instruction->Source2.Data.LocalVariable.LocalVariableIndex == pOldLocalVariable->LocalVariableIndex)
		{
			instruction->Source2.Data.LocalVariable.LocalVariableIndex = pNewLocalVariable->LocalVariableIndex;
		}
		else if (instruction->Source2.Type == SourceType_LocalAddress &&
				 instruction->Source2.Data.LocalVariableAddress.LocalVariableIndex == pOldLocalVariable->LocalVariableIndex)
		{
			instruction->Source2.Data.LocalVariableAddress.LocalVariableIndex = pNewLocalVariable->LocalVariableIndex;
		}
		if (instruction->Source3.Type == SourceType_Local &&
			instruction->Source3.Data.LocalVariable.LocalVariableIndex == pOldLocalVariable->LocalVariableIndex)
		{
			instruction->Source3.Data.LocalVariable.LocalVariableIndex = pNewLocalVariable->LocalVariableIndex;
		}
		else if (instruction->Source3.Type == SourceType_LocalAddress &&
				 instruction->Source3.Data.LocalVariableAddress.LocalVariableIndex == pOldLocalVariable->LocalVariableIndex)
		{
			instruction->Source3.Data.LocalVariableAddress.LocalVariableIndex = pNewLocalVariable->LocalVariableIndex;
		}
		// TODO: Also handle indirects that may go to other indirects, but eventually to locals
	}
	pRetargettedNodes[pCodeNode->Index] = TRUE;
	for (uint32_t childIndex = 0; childIndex < pCodeNode->ChildrenCount; ++childIndex)
	{
		if (!pRetargettedNodes[pCodeNode->Children[childIndex]->Index])
			IROptimizer_RetargetLocalSources(pMethod, pCodeNode->Children[childIndex], 0, pOldLocalVariable, pNewLocalVariable, pRetargettedNodes);
	}
}

IRLocalVariable* IROptimizer_GetFinalIteration(IRMethod* pMethod, IRCodeNode* pCodeNode, uint32_t pDerivedIndex)
{
	if (pCodeNode->FinalIterations[pDerivedIndex]) return pCodeNode->FinalIterations[pDerivedIndex];
	return IROptimizer_GetFinalIteration(pMethod, pCodeNode->Dominator, pDerivedIndex);
}

void IROptimizer_EnterSSA(IRMethod* pMethod, IRCodeNode** pNodes, uint32_t pNodesCount)
{
	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Started Entering SSA for %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);

	IRCodeNode* codeNode = NULL;
	IRInstruction* instruction = NULL;
	IRLocalVariable* oldLocalVariable = NULL;
	IRLocalVariable* newLocalVariable = NULL;
	uint32_t derivedCount = pMethod->LocalVariableCount;
	uint32_t* derivedIterations = (uint32_t*)calloc(1, derivedCount * sizeof(uint32_t));
	bool_t* derivedPhis = (bool_t*)calloc(1, derivedCount * sizeof(bool_t));
	bool_t* retargettedNodes = (bool_t*)calloc(1, pNodesCount * sizeof(bool_t));
	for (uint32_t nodeIndex = 0; nodeIndex < pNodesCount; ++nodeIndex)
	{
		codeNode = pNodes[nodeIndex];
		codeNode->FinalIterations = (IRLocalVariable**)calloc(1, derivedCount * sizeof(IRLocalVariable*));
		codeNode->FinalIterationsCount = derivedCount;
		if (nodeIndex == 0)
		{
			for (uint32_t derivedIndex = 0; derivedIndex < derivedCount; ++derivedIndex)
			{
				codeNode->FinalIterations[derivedIndex] = pMethod->LocalVariables[derivedIndex];
			}
		}
		for (uint32_t instructionIndex = 0; instructionIndex < codeNode->InstructionsCount; ++instructionIndex)
		{
			instruction = pMethod->IRCodes[codeNode->Instructions[instructionIndex]];
			if (instruction->Destination.Type == SourceType_Local)
			{
				oldLocalVariable = pMethod->LocalVariables[instruction->Destination.Data.LocalVariable.LocalVariableIndex];
				if (!oldLocalVariable->SSAData)
				{
					oldLocalVariable->SSAData = IRLocalSSAData_Create(oldLocalVariable);
					codeNode->FinalIterations[oldLocalVariable->LocalVariableIndex] = oldLocalVariable;
					oldLocalVariable->SSAData->LifeStarted = pMethod->IRCodes[codeNode->Instructions[instructionIndex]];
					Log_WriteLine(LOGLEVEL__Optimize_SSA, "Initial Local Assignment at instruction %u (Node %u) to %u, iteration 0 (Derived %u)", (unsigned int)codeNode->Instructions[instructionIndex], (unsigned int)codeNode->Index, (unsigned int)oldLocalVariable->LocalVariableIndex, (unsigned int)oldLocalVariable->SSAData->Derived->LocalVariableIndex);
					continue;
				}
				newLocalVariable = IRLocalVariable_Copy(oldLocalVariable->SSAData->Derived);
				IRMethod_AddLocal(pMethod, newLocalVariable);
				if (oldLocalVariable->NonSSADerivable)
				{
					newLocalVariable->SSAData->LifeStarted = pMethod->IRCodes[codeNode->Instructions[instructionIndex]];
					newLocalVariable->SSAData->Iteration = 0;
					instruction->Destination.Data.LocalVariable.LocalVariableIndex = newLocalVariable->LocalVariableIndex;
					Log_WriteLine(LOGLEVEL__Optimize_SSA, "Expanded NonDerivable Local Assignment at instruction %u (Node %u) from %u to %u", (unsigned int)codeNode->Instructions[instructionIndex], (unsigned int)codeNode->Index, (unsigned int)oldLocalVariable->LocalVariableIndex, (unsigned int)newLocalVariable->LocalVariableIndex);
				}
				else
				{
					derivedIterations[newLocalVariable->SSAData->Derived->LocalVariableIndex]++;
					newLocalVariable->SSAData->Iteration = derivedIterations[newLocalVariable->SSAData->Derived->LocalVariableIndex];
					instruction->Destination.Data.LocalVariable.LocalVariableIndex = newLocalVariable->LocalVariableIndex;
					codeNode->FinalIterations[newLocalVariable->SSAData->Derived->LocalVariableIndex] = newLocalVariable;
					newLocalVariable->SSAData->LifeStarted = pMethod->IRCodes[codeNode->Instructions[instructionIndex]];
					Log_WriteLine(LOGLEVEL__Optimize_SSA, "Expanded Local Assignment at instruction %u (Node %u) from %u to %u, iteration %u (Derived %u)", (unsigned int)codeNode->Instructions[instructionIndex], (unsigned int)codeNode->Index, (unsigned int)oldLocalVariable->LocalVariableIndex, (unsigned int)newLocalVariable->LocalVariableIndex, (unsigned int)newLocalVariable->SSAData->Iteration, (unsigned int)newLocalVariable->SSAData->Derived->LocalVariableIndex);
				}
				memset(retargettedNodes, 0x00, pNodesCount * sizeof(bool_t));
				IROptimizer_RetargetLocalSources(pMethod, codeNode, instructionIndex + 1, oldLocalVariable, newLocalVariable, retargettedNodes);
				Log_WriteLine(LOGLEVEL__Optimize_SSA, "Retargeting LocalVariable Index %u to %u", (unsigned int)oldLocalVariable->LocalVariableIndex, (unsigned int)newLocalVariable->LocalVariableIndex);
			}
			else if (instruction->Destination.Type == SourceType_Indirect)
			{
				// TODO: Follow through indirect address sources, until we reach non indirect, if it's local, then we need to work with it here like a local
				Panic("Deal with this later!");
			}
		}
	}

	for (uint32_t nodeIndex = 0; nodeIndex < pNodesCount; ++nodeIndex)
	{
		codeNode = pNodes[nodeIndex];
		if (codeNode->SourceFrontiersCount >= 2)
		{
			memset(derivedPhis, 0x00, derivedCount * sizeof(bool_t));
			for (uint32_t frontierIndex = 0; frontierIndex < codeNode->SourceFrontiersCount; ++frontierIndex)
			{
				for (uint32_t derivedIndex = 0; derivedIndex < derivedCount; ++derivedIndex)
				{
					derivedPhis[derivedIndex] = codeNode->SourceFrontiers[frontierIndex]->FinalIterations[derivedIndex] != NULL;
				}
			}
			for (uint32_t derivedIndex = 0; derivedIndex < derivedCount; ++derivedIndex)
			{
				if (derivedPhis[derivedIndex])
				{
					newLocalVariable = IRLocalVariable_Copy(pMethod->LocalVariables[derivedIndex]);
					IRMethod_AddLocal(pMethod, newLocalVariable);
					derivedIterations[newLocalVariable->SSAData->Derived->LocalVariableIndex]++;
					newLocalVariable->SSAData->Iteration = derivedIterations[newLocalVariable->SSAData->Derived->LocalVariableIndex];
					if (!codeNode->FinalIterations[newLocalVariable->SSAData->Derived->LocalVariableIndex])
						codeNode->FinalIterations[newLocalVariable->SSAData->Derived->LocalVariableIndex] = newLocalVariable;
					newLocalVariable->SSAData->LifeStarted = pMethod->IRCodes[codeNode->Instructions[0]];
					IRPhi* phi = IRPhi_Create(newLocalVariable);
					Log_WriteLine(LOGLEVEL__Optimize_SSA, "Phi Local Assignment at instruction %u (Node %u) to %u, iteration %u (Derived %u)", (unsigned int)codeNode->Instructions[0], (unsigned int)codeNode->Index, (unsigned int)newLocalVariable->LocalVariableIndex, (unsigned int)newLocalVariable->SSAData->Iteration, (unsigned int)newLocalVariable->SSAData->Derived->LocalVariableIndex);
					for (uint32_t frontierIndex = 0; frontierIndex < codeNode->SourceFrontiersCount; ++frontierIndex)
					{
						oldLocalVariable = IROptimizer_GetFinalIteration(pMethod, codeNode->SourceFrontiers[frontierIndex], derivedIndex);
						IRPhi_AddArgument(phi, oldLocalVariable);
						Log_WriteLine(LOGLEVEL__Optimize_SSA, "    From %u, iteration %u", (unsigned int)oldLocalVariable->LocalVariableIndex, (unsigned int)oldLocalVariable->SSAData->Iteration);
						memset(retargettedNodes, 0x00, pNodesCount * sizeof(bool_t));
						IROptimizer_RetargetLocalSources(pMethod, codeNode, 0, oldLocalVariable, newLocalVariable, retargettedNodes);
					}
					IRCodeNode_AddPhi(codeNode, phi);
				}
			}
		}
	}
	free(retargettedNodes);
	free(derivedPhis);
	free(derivedIterations);

	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Finished Entering SSA for %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);
}

void IROptimizer_LeaveSSA(IRMethod* pMethod, IRCodeNode** pNodes, uint32_t pNodesCount)
{
	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Started Leaving SSA for %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);

	IRCodeNode* codeNode = NULL;
	IRInstruction* instruction = NULL;
	IRPhi* phi = NULL;
	IRCodeNode* sourceNode = NULL;
	for (uint32_t nodeIndex = 0; nodeIndex < pNodesCount; ++nodeIndex)
	{
		codeNode = pNodes[nodeIndex];
		for (uint32_t phiIndex = 0; phiIndex < codeNode->PhiFunctionsCount; ++phiIndex)
		{
			phi = codeNode->PhiFunctions[phiIndex];
			Log_WriteLine(LOGLEVEL__Optimize_SSA, "Reducing Phi at instruction %u (Node %u)", (unsigned int)codeNode->Instructions[0], (unsigned int)codeNode->Index);
			for (uint32_t sourceIndex = 0; sourceIndex  < codeNode->SourceFrontiersCount; ++sourceIndex)
			{
				sourceNode = codeNode->SourceFrontiers[sourceIndex];
				instruction = IRInstruction_Create(pMethod->IRCodes[sourceNode->Instructions[sourceNode->InstructionsCount - 1]]->ILLocation, IROpcode_Move);
				instruction->Destination.Type = SourceType_Local;
				instruction->Destination.Data.LocalVariable.LocalVariableIndex = phi->Result->LocalVariableIndex;
				instruction->Source1.Type = SourceType_Local;
				instruction->Source1.Data.LocalVariable.LocalVariableIndex = phi->Arguments[sourceIndex]->LocalVariableIndex;
				IRMethod_InsertInstruction(pMethod, sourceNode->Instructions[sourceNode->InstructionsCount - 1], instruction);
				sourceNode->InstructionsCount++;
				sourceNode->Instructions = (uint32_t*)realloc(sourceNode->Instructions, sourceNode->InstructionsCount * sizeof(uint32_t));
				sourceNode->Instructions[sourceNode->InstructionsCount - 1] = instruction->IRLocation;
				for (uint32_t fixNodeIndex = nodeIndex; fixNodeIndex < pNodesCount; ++fixNodeIndex)
				{
					for (uint32_t fixInstructionIndex = 0; fixInstructionIndex < pNodes[fixNodeIndex]->InstructionsCount; ++fixInstructionIndex)
					{
						pNodes[fixNodeIndex]->Instructions[fixInstructionIndex]++;
					}
				}
				if (phi->Arguments[sourceIndex]->SSAData->LifeStarted->IRLocation < phi->Result->SSAData->LifeStarted->IRLocation)
				{
					phi->Result->SSAData->LifeStarted = phi->Arguments[sourceIndex]->SSAData->LifeStarted;
				}
				
				Log_WriteLine(LOGLEVEL__Optimize_SSA, "Reducing Phi at instruction %u (Node %u) from %u to %u, iteration %u (Derived %u)", (unsigned int)codeNode->Instructions[0], (unsigned int)codeNode->Index, (unsigned int)phi->Arguments[sourceIndex]->LocalVariableIndex, (unsigned int)phi->Result->LocalVariableIndex, (unsigned int)phi->Result->SSAData->Iteration, (unsigned int)phi->Result->SSAData->Derived->LocalVariableIndex);
			}
		}
	}

	IRLocalVariable* localVariable = NULL;
	for (uint32_t localIndex = 0; localIndex < pMethod->LocalVariableCount; ++localIndex)
	{
		localVariable = pMethod->LocalVariables[localIndex];
		localVariable->SSAData->LifeEnded = localVariable->SSAData->LifeStarted;
		for (uint32_t instructionIndex = pMethod->IRCodesCount - 1; instructionIndex > localVariable->SSAData->LifeStarted->IRLocation; --instructionIndex)
		{
			instruction = pMethod->IRCodes[instructionIndex];
			if (instruction->Source1.Type == SourceType_Local && instruction->Source1.Data.LocalVariable.LocalVariableIndex == localVariable->LocalVariableIndex)
			{
				localVariable->SSAData->LifeEnded = instruction;
				break;
			}
			else if (instruction->Source1.Type == SourceType_LocalAddress && instruction->Source1.Data.LocalVariableAddress.LocalVariableIndex == localVariable->LocalVariableIndex)
			{
				localVariable->SSAData->LifeEnded = instruction;
				break;
			}
			else if (instruction->Source2.Type == SourceType_Local && instruction->Source2.Data.LocalVariable.LocalVariableIndex == localVariable->LocalVariableIndex)
			{
				localVariable->SSAData->LifeEnded = instruction;
				break;
			}
			else if (instruction->Source2.Type == SourceType_LocalAddress && instruction->Source2.Data.LocalVariableAddress.LocalVariableIndex == localVariable->LocalVariableIndex)
			{
				localVariable->SSAData->LifeEnded = instruction;
				break;
			}
			else if (instruction->Source3.Type == SourceType_Local && instruction->Source3.Data.LocalVariable.LocalVariableIndex == localVariable->LocalVariableIndex)
			{
				localVariable->SSAData->LifeEnded = instruction;
				break;
			}
			else if (instruction->Source3.Type == SourceType_LocalAddress && instruction->Source3.Data.LocalVariableAddress.LocalVariableIndex == localVariable->LocalVariableIndex)
			{
				localVariable->SSAData->LifeEnded = instruction;
				break;
			}
			//else if (instructionIndex == localVariable->SSAData->LifeStarted->IRLocation + 1)
			//{
			//	break;
			//}

			//Log_WriteLine(LOGLEVEL__Optimize_SSA, "ILLocation[%u] Source1Type = %u, Data = %u, Source2Type = %u, Data = %u, DestType = %u, Data = %u", (unsigned int)instruction->ILLocation, (unsigned int)instruction->Source1.Type, (unsigned int)instruction->Source1.Data.ConstantI4.Value, (unsigned int)instruction->Source2.Type, (unsigned int)instruction->Source2.Data.ConstantI4.Value, (unsigned int)instruction->Destination.Type, (unsigned int)instruction->Destination.Data.ConstantI4.Value);
		}

		if (localVariable->SSAData->LifeStarted->IRLocation == localVariable->SSAData->LifeEnded->IRLocation)
		{
			if (localVariable->NonSSADerivable)
			{
				Log_WriteLine(LOGLEVEL__Optimize_SSA, "LocalVariable[%u] Lifespan = Dead @ %u Source %u, %u - Dest %u, %u", (unsigned int)localVariable->LocalVariableIndex, (unsigned int)localVariable->SSAData->LifeStarted->ILLocation, (unsigned int)localVariable->SSAData->LifeStarted->Source1.Type, (unsigned int)localVariable->SSAData->LifeStarted->Source1.Data.ConstantI4.Value, (unsigned int)localVariable->SSAData->LifeStarted->Destination.Type, (unsigned int)localVariable->SSAData->LifeStarted->Destination.Data.ConstantI4.Value);
			}
			else
			{
				Log_WriteLine(LOGLEVEL__Optimize_SSA, "LocalVariable[%u] Lifespan = Dead", (unsigned int)localVariable->LocalVariableIndex);
			}

		}
		else
		{
			Log_WriteLine(LOGLEVEL__Optimize_SSA, "LocalVariable[%u] Lifespan = %u to %u", (unsigned int)localVariable->LocalVariableIndex, (unsigned int)localVariable->SSAData->LifeStarted->IRLocation, (unsigned int)localVariable->SSAData->LifeEnded->IRLocation);
		}
	}

	Log_WriteLine(LOGLEVEL__Optimize_SSA, "Finished Leaving SSA for %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);
}
