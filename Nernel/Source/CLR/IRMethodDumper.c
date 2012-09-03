#include <CLR/IRMethodDumper.h>



#define IDentPerLevel 2

void WriteInfoOnSourceType(SourceTypeData* pDat, uint32_t ident)
{
	switch(pDat->Type)
	{
		case SourceType_ArrayElement:
			printf("%*sArrayElement: %s.%s\n", (int)ident, "", pDat->Data.ArrayElement.ElementType->TypeDefinition->Namespace, pDat->Data.ArrayElement.ElementType->TypeDefinition->Name);
			goto ArrayCommon;
		case SourceType_ArrayElementAddress:
			printf("%*sArrayElementAddress: %s.%s\n", (int)ident, "", pDat->Data.ArrayElementAddress.ElementType->TypeDefinition->Namespace, pDat->Data.ArrayElementAddress.ElementType->TypeDefinition->Name);
		ArrayCommon:
			printf("%*sArraySource:\n", (int)ident + IDentPerLevel, "");
			WriteInfoOnSourceType(pDat->Data.ArrayElement.ArraySource, ident + IDentPerLevel + IDentPerLevel);
			printf("%*sIndexSource:\n", (int)ident + IDentPerLevel, "");
			WriteInfoOnSourceType(pDat->Data.ArrayElement.IndexSource, ident + IDentPerLevel + IDentPerLevel);
			break;
		case SourceType_ArrayLength:
			printf("%*sArrayLength:\n", (int)ident, "");
			printf("%*sArraySource:\n", (int)ident + IDentPerLevel, "");
			WriteInfoOnSourceType(pDat->Data.ArrayLength.ArraySource, ident + IDentPerLevel + IDentPerLevel);
			break;
		case SourceType_Field:
			printf("%*sField: %s\n", (int)ident, "", pDat->Data.Field.ParentType->Fields[pDat->Data.Field.FieldIndex]->FieldDefinition->Name);
			goto FieldCommon;
		case SourceType_FieldAddress:
			printf("%*sFieldAddress: %s\n", (int)ident, "", pDat->Data.Field.ParentType->Fields[pDat->Data.Field.FieldIndex]->FieldDefinition->Name);
		FieldCommon:
			printf("%*sObjectSource:\n", (int)ident + IDentPerLevel, "");
			WriteInfoOnSourceType(pDat->Data.Field.FieldSource, ident + IDentPerLevel + IDentPerLevel);
			break;
		case SourceType_Indirect:
			printf("%*sIndirect:\n", (int)ident, "");
			printf("%*sAddressSource:\n", (int)ident + IDentPerLevel, "");
			WriteInfoOnSourceType(pDat->Data.Indirect.AddressSource, ident + IDentPerLevel + IDentPerLevel);
			break;
		case SourceType_Local:
			printf("%*sLocal: %i\n", (int)ident, "", (int)pDat->Data.LocalVariable.LocalVariableIndex);
			break;
		case SourceType_LocalAddress:
			printf("%*sLocalAddress: %i\n", (int)ident, "", (int)pDat->Data.LocalVariableAddress.LocalVariableIndex);
			break;
		case SourceType_Parameter:
			printf("%*sParameter: %i\n", (int)ident, "", (int)pDat->Data.Parameter.ParameterIndex);
			break;
		case SourceType_ParameterAddress:
			printf("%*sParameterAddress: %i\n", (int)ident, "", (int)pDat->Data.ParameterAddress.ParameterIndex);
			break;
		case SourceType_ConstantI4:
			printf("%*sConstantI4: %i\n", (int)ident, "", (int)pDat->Data.ConstantI4.Value);
			break;
		case SourceType_Null:
			printf("%*sNULL\n", (int)ident, "");
			break;
		default:
			// This is deliberately not indented.
			printf("SourceTypeNotCurrentlySupported\n");
			break;
	}
}


void IRMethodDumper_Dump(IRMethod* pMethod)
{
	for (uint32_t i = 0; i < pMethod->IRCodesCount; i++)
	{
		if (pMethod->IRCodes[i]->Opcode != IROpcode_Nop)
		{
			switch(pMethod->IRCodes[i]->Opcode)
			{
				case IROpcode_Move:
					printf("IRInstruction: %i (IROpcode_Move)\n", (int)i);
					break;
				case IROpcode_Add:
					printf("IRInstruction: %i (IROpcode_Add)\n", (int)i);
					break;
				case IROpcode_Sub:
					printf("IRInstruction: %i (IROpcode_Sub)\n", (int)i);
					break;
				case IROpcode_Return:
					printf("IRInstruction: %i (IROpcode_Return)\n", (int)i);
					break;
				case IROpcode_Branch:
					printf("IRInstruction: %i (IROpcode_Branch)\n", (int)i);
					break;
				case IROpcode_Call_Absolute:
					printf("IRInstruction: %i (IROpcode_Call_Absolute)\n", (int)i);
					break;
				case IROpcode_Call_Virtual:
					printf("IRInstruction: %i (IROpcode_Call_Virtual)\n", (int)i);
					break;
				case IROpcode_Call_Internal:
					printf("IRInstruction: %i (IROpcode_Call_Internal)\n", (int)i);
					break;
				case IROpcode_Load_String:
					printf("IRInstruction: %i (IROpcode_Load_String)\n", (int)i);
					break;
				case IROpcode_Convert_Unchecked:
					printf("IRInstruction: %i (IROpcode_Convert_Unchecked)\n", (int)i);
					break;
				case IROpcode_New_Object:
					printf("IRInstruction: %i (IROpcode_New_Object)\n", (int)i);
					break;
				case IROpcode_New_Array:
					printf("IRInstruction: %i (IROpcode_New_Array)\n", (int)i);
					break;
				default:
					printf("IRInstruction: %i (%i)\n", (int)i, (int)pMethod->IRCodes[i]->Opcode);
					break;
			}
			printf("%*sSource1:\n", IDentPerLevel, "");
			WriteInfoOnSourceType(&pMethod->IRCodes[i]->Source1, IDentPerLevel * 2);
			printf("%*sSource2:\n", IDentPerLevel, "");
			WriteInfoOnSourceType(&pMethod->IRCodes[i]->Source2, IDentPerLevel * 2);
			printf("%*sSource3:\n", IDentPerLevel, "");
			WriteInfoOnSourceType(&pMethod->IRCodes[i]->Source3, IDentPerLevel * 2);
			printf("%*sDestination:\n", IDentPerLevel, "");
			WriteInfoOnSourceType(&pMethod->IRCodes[i]->Destination, IDentPerLevel * 2);
			printf("%*sSourceArray: (%i)\n", IDentPerLevel, "", (int)pMethod->IRCodes[i]->SourceArrayLength);
			for (uint32_t i2 = 0; i2 < pMethod->IRCodes[i]->SourceArrayLength; i2++)
			{
				printf("%*sSourceArray index: %i\n", IDentPerLevel * 2, "", (int)i2);
				WriteInfoOnSourceType(&pMethod->IRCodes[i]->SourceArray[i2], IDentPerLevel * 3);
			}
		}
	}
}