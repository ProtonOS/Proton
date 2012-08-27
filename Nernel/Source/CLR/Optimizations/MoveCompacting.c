#include <CLR/Optimizations/MoveCompacting.h>


void ProcessSourceTypeData(SourceTypeData* sDat, uint32_t** localUseCount, bool_t** addressLoaded)
{
	switch(sDat->Type)
	{
		case SourceType_ArrayElement:
			ProcessSourceTypeData(sDat->Data.ArrayElement.ArraySource, localUseCount, addressLoaded);
			ProcessSourceTypeData(sDat->Data.ArrayElement.IndexSource, localUseCount, addressLoaded);
			break;
		case SourceType_ArrayElementAddress:
			ProcessSourceTypeData(sDat->Data.ArrayElementAddress.ArraySource, localUseCount, addressLoaded);
			ProcessSourceTypeData(sDat->Data.ArrayElementAddress.IndexSource, localUseCount, addressLoaded);
			break;
		case SourceType_ArrayLength:
			ProcessSourceTypeData(sDat->Data.ArrayLength.ArraySource, localUseCount, addressLoaded);
			break;
		case SourceType_Field:
			ProcessSourceTypeData(sDat->Data.Field.FieldSource, localUseCount, addressLoaded);
			break;
		case SourceType_FieldAddress:
			ProcessSourceTypeData(sDat->Data.FieldAddress.FieldSource, localUseCount, addressLoaded);
			break;
		case SourceType_Indirect:
			ProcessSourceTypeData(sDat->Data.Indirect.AddressSource, localUseCount, addressLoaded);
			break;
			
		case SourceType_Local:
			(*localUseCount)[sDat->Data.LocalVariableAddress.LocalVariableIndex]++;
			break;
		case SourceType_LocalAddress:
			(*localUseCount)[sDat->Data.LocalVariableAddress.LocalVariableIndex]++;
			(*addressLoaded)[sDat->Data.LocalVariableAddress.LocalVariableIndex] = TRUE;
			break;
		default:
			break;
	}
}

void RetargetSources(SourceTypeData* searchingInType, SourceTypeData* lookingFor, SourceTypeData* replaceWith)
{
	if (!memcmp(searchingInType, lookingFor, sizeof(SourceTypeData)))
	{
		*searchingInType = *replaceWith;
		return;
	}
	switch(searchingInType->Type)
	{
		case SourceType_ArrayElement:
			RetargetSources(searchingInType->Data.ArrayElement.ArraySource, lookingFor, replaceWith);
			RetargetSources(searchingInType->Data.ArrayElement.IndexSource, lookingFor, replaceWith);
			break;
		case SourceType_ArrayElementAddress:
			RetargetSources(searchingInType->Data.ArrayElementAddress.ArraySource, lookingFor, replaceWith);
			RetargetSources(searchingInType->Data.ArrayElementAddress.IndexSource, lookingFor, replaceWith);
			break;
		case SourceType_ArrayLength:
			RetargetSources(searchingInType->Data.ArrayLength.ArraySource, lookingFor, replaceWith);
			break;
		case SourceType_Field:
			RetargetSources(searchingInType->Data.Field.FieldSource, lookingFor, replaceWith);
			break;
		case SourceType_FieldAddress:
			RetargetSources(searchingInType->Data.FieldAddress.FieldSource, lookingFor, replaceWith);
			break;
		case SourceType_Indirect:
			RetargetSources(searchingInType->Data.Indirect.AddressSource, lookingFor, replaceWith);
			break;
		default:
			break;
	}

}

void IROptimizer_MoveCompacting(IRMethod* pMethod)
{
	uint32_t* localUseCount = (uint32_t*)calloc(1, sizeof(uint32_t) * pMethod->LocalVariableCount);
	IRInstruction** localsAssignedAt = (IRInstruction**)calloc(1, sizeof(IRInstruction*) * pMethod->LocalVariableCount);
	bool_t* addressLoadedOf = (bool_t*)calloc(1, sizeof(bool_t) * pMethod->LocalVariableCount);
	for (uint32_t i = 0; i < pMethod->IRCodesCount; i++)
	{
		IRInstruction* instr = pMethod->IRCodes[i];
		ProcessSourceTypeData(&instr->Source1, &localUseCount, &addressLoadedOf);
		ProcessSourceTypeData(&instr->Source2, &localUseCount, &addressLoadedOf);
		ProcessSourceTypeData(&instr->Source3, &localUseCount, &addressLoadedOf);
		ProcessSourceTypeData(&instr->Destination, &localUseCount, &addressLoadedOf);
		if (instr->Destination.Type == SourceType_Local)
		{
			localUseCount[instr->Destination.Data.LocalVariable.LocalVariableIndex]--;
			if (instr->Opcode == IROpcode_Move)
			{
				localsAssignedAt[instr->Destination.Data.LocalVariable.LocalVariableIndex] = instr;
			}
		}
		for (uint32_t i2 = 0; i2 < instr->SourceArrayLength; i2++)
		{
			ProcessSourceTypeData(&instr->SourceArray[i2], &localUseCount, &addressLoadedOf);
		}
	}

	for (uint32_t i = 0; i < pMethod->LocalVariableCount; i++)
	{
		if (addressLoadedOf[i]) continue;
		if (localsAssignedAt[i])
		{
			if (localsAssignedAt[i]->Source1.Type == SourceType_ConstantI4 ||
				localsAssignedAt[i]->Source1.Type == SourceType_ConstantI8 ||
				localsAssignedAt[i]->Source1.Type == SourceType_ConstantR4 ||
				localsAssignedAt[i]->Source1.Type == SourceType_ConstantR8 ||
				localsAssignedAt[i]->Source1.Type == SourceType_Null ||
				localsAssignedAt[i]->Source1.Type == SourceType_Local ||
				localsAssignedAt[i]->Source1.Type == SourceType_LocalAddress)
			{
				// we need to retarget it's loads to be from it instead.
				for (uint32_t i2 = 0; i2 < pMethod->IRCodesCount; i2++)
				{
					IRInstruction* instr = pMethod->IRCodes[i2];
					RetargetSources(&instr->Source1, &localsAssignedAt[i]->Destination, &localsAssignedAt[i]->Source1);
					RetargetSources(&instr->Source2, &localsAssignedAt[i]->Destination, &localsAssignedAt[i]->Source1);
					RetargetSources(&instr->Source3, &localsAssignedAt[i]->Destination, &localsAssignedAt[i]->Source1);
					if (instr->Destination.Type != SourceType_Local || instr->Destination.Data.LocalVariable.LocalVariableIndex != i)
					{
						RetargetSources(&instr->Destination, &localsAssignedAt[i]->Destination, &localsAssignedAt[i]->Source1);
					}
					for (uint32_t i3 = 0; i3 < instr->SourceArrayLength; i3++)
					{
						RetargetSources(&instr->SourceArray[i3], &localsAssignedAt[i]->Destination, &localsAssignedAt[i]->Source1);
					}
				}
				localsAssignedAt[i]->Opcode = IROpcode_Nop;
				localsAssignedAt[i]->Arg1 = NULL;
			}
		}
	}

	free(localUseCount);
	free(localsAssignedAt);
	free(addressLoadedOf);
}