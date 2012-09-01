#include <CLR/Optimizations/GenericCleanup.h>

void CleanupSource(SourceTypeData* pData)
{
	switch(pData->Type)
	{
		case SourceType_ArrayElement:
			CleanupSource(pData->Data.ArrayElement.ArraySource);
			CleanupSource(pData->Data.ArrayElement.IndexSource);
			if (pData->Data.ArrayElement.ElementType->IsStructureType)
			{
				pData->Type = SourceType_ArrayElementAddress;
			}
			break;
		case SourceType_ArrayElementAddress:
			CleanupSource(pData->Data.ArrayElementAddress.ArraySource);
			CleanupSource(pData->Data.ArrayElementAddress.IndexSource);
			break;
		case SourceType_ArrayLength:
			CleanupSource(pData->Data.ArrayLength.ArraySource);
			break;
		case SourceType_Field:
			CleanupSource(pData->Data.Field.FieldSource);
			break;
		case SourceType_FieldAddress:
			CleanupSource(pData->Data.FieldAddress.FieldSource);
			break;
		case SourceType_Indirect:
			CleanupSource(pData->Data.Indirect.AddressSource);
			break;
		default: break;
	}
}

void IROptimizer_GenericCleanup(IRMethod* pMethod)
{
	/*for (uint32_t index = 0; index < pMethod->IRCodesCount; ++index)
	{
		IRInstruction* instruction = pMethod->IRCodes[index];
		if (instruction->Destination.Type == SourceType_ArrayElement)
		{
			CleanupSource(instruction->Destination.Data.ArrayElement.ArraySource);
			CleanupSource(instruction->Destination.Data.ArrayElement.IndexSource);
		}
		else
		{
			CleanupSource(&instruction->Destination);
		}
		CleanupSource(&instruction->Source1);
		CleanupSource(&instruction->Source2);
		CleanupSource(&instruction->Source3);
		for (uint32_t sourceIndex = 0; sourceIndex < instruction->SourceArrayLength; ++sourceIndex)
		{
			CleanupSource(&instruction->SourceArray[sourceIndex]);
		}
	}*/
}