#include <CLR/Optimizations/IntrinsicSubstitution.h>

typedef struct _IntrinsicSubstitution
{
	const char* Namespace;
	const char* TypeName;
	const char* Name;
	IntrinsicCallType CallType;
} IntrinsicSubstitution;

const IntrinsicSubstitution IntrinsicSubstitutionTable[] = 
{
	{	"System",								"Buffer",				"ByteLengthInternal",			IntrinsicCallType___System___Buffer___ByteLengthInternal},
	{	NULL,									NULL,					"GetByteInternal",				IntrinsicCallType___System___Buffer___GetByteInternal},
	{	NULL,									NULL,					"SetByteInternal",				IntrinsicCallType___System___Buffer___SetByteInternal},

	{	NULL,									"GC",					"get_MaxGeneration",			IntrinsicCallType___System___GC___get_MaxGeneration},
	{	NULL,									NULL,					"GetGeneration",				IntrinsicCallType___System___GC___GetGeneration},

	{	"System.Runtime.CompilerServices",		"RuntimeHelpers",		"get_OffsetToStringData",		IntrinsicCallType___System_Runtime_CompilerServices___RuntimeHelpers___get_OffsetToStringData},

	{	NULL,									NULL,					NULL,							IntrinsicCallType___NULL }
};

void IROptimizer_IntrinsicSubstitution(IRMethod* pMethod)
{
	for (uint32_t instructionIndex = 0; instructionIndex < pMethod->IRCodesCount; ++instructionIndex)
	{
		IRInstruction* instruction = pMethod->IRCodes[instructionIndex];
		if (instruction->Opcode == IROpcode_Call_Internal)
		{
			const IntrinsicSubstitution* is = NULL;
			const char* foundNamespace = NULL;
			const char* foundTypeName = NULL;
			MethodDefinition* methodDefinition = ((IRMethod*)instruction->Arg1)->MethodDefinition;
			for (uint32_t index = 0; IntrinsicSubstitutionTable[index].CallType; ++index)
			{
				is = &IntrinsicSubstitutionTable[index];
				if (!foundNamespace)
				{
					if (is->Namespace && !strcmp(is->Namespace, methodDefinition->TypeDefinition->Namespace)) foundNamespace = is->Namespace;
					else continue;
				}
				else if (is->Namespace) break;

				if (!foundTypeName)
				{
					if (is->TypeName && !strcmp(is->TypeName, methodDefinition->TypeDefinition->Name)) foundTypeName = is->TypeName;
					else continue;
				}
				else if (is->TypeName) break;

				if (is->Name && !strcmp(is->Name, methodDefinition->Name))
				{
					switch (is->CallType)
					{
						case IntrinsicCallType___System_Runtime_CompilerServices___RuntimeHelpers___get_OffsetToStringData:
						{
							instruction->Opcode = IROpcode_Move;
							instruction->Source1.Type = SourceType_ConstantI4;
							instruction->Source1.Data.ConstantI4.Value = 4;
							instruction->Arg1 = NULL;
							break;
						}
						case IntrinsicCallType___System___Buffer___ByteLengthInternal:
						{
							instruction->Opcode = IROpcode_Move;
							instruction->Source1.Type = SourceType_ArrayLength;
							instruction->Source1.Data.ArrayLength.ArraySource = &instruction->SourceArray[0];
							instruction->SourceArray = NULL;
							instruction->SourceArrayLength = 0;
							instruction->Arg1 = NULL;
							break;
						}
						case IntrinsicCallType___System___Buffer___GetByteInternal:
						{
							instruction->Opcode = IROpcode_Move;
							instruction->Source1.Type = SourceType_ArrayElement;
							instruction->Source1.Data.ArrayElement.ArraySource = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
							*instruction->Source1.Data.ArrayElement.ArraySource = instruction->SourceArray[1];
							instruction->Source1.Data.ArrayElement.ElementType = pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_Byte->TableIndex - 1];
							instruction->Source1.Data.ArrayElement.IndexSource = &instruction->SourceArray[0];
							instruction->Source1.Data.ArrayElement.NoChecksRequired = TRUE;
							instruction->SourceArray = NULL;
							instruction->SourceArrayLength = 0;
							instruction->Arg1 = NULL;
							break;
						}
						case IntrinsicCallType___System___Buffer___SetByteInternal:
						{
							instruction->Opcode = IROpcode_Move;
							instruction->Source1 = instruction->SourceArray[0];
							instruction->Destination.Type = SourceType_ArrayElement;
							instruction->Destination.Data.ArrayElement.ArraySource = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
							*instruction->Destination.Data.ArrayElement.ArraySource = instruction->SourceArray[2];
							instruction->Destination.Data.ArrayElement.ElementType = pMethod->ParentAssembly->ParentDomain->IRAssemblies[0]->Types[pMethod->ParentAssembly->ParentDomain->CachedType___System_Byte->TableIndex - 1];
							instruction->Destination.Data.ArrayElement.IndexSource = (SourceTypeData*)calloc(1, sizeof(SourceTypeData));
							*instruction->Destination.Data.ArrayElement.IndexSource = instruction->SourceArray[1];
							instruction->Destination.Data.ArrayElement.NoChecksRequired = TRUE;

							free(instruction->SourceArray);
							instruction->SourceArray = NULL;
							instruction->SourceArrayLength = 0;
							instruction->Arg1 = NULL;
							break;
						}
						case IntrinsicCallType___System___GC___get_MaxGeneration:
						{
							instruction->Opcode = IROpcode_Move;
							instruction->Source1.Type = SourceType_ConstantI4;
							instruction->Source1.Data.ConstantI4.Value = 0;
							instruction->Arg1 = NULL;
							break;
						}
						case IntrinsicCallType___System___GC___GetGeneration:
						{
							instruction->Opcode = IROpcode_Move;
							instruction->Source1.Type = SourceType_ConstantI4;
							instruction->Source1.Data.ConstantI4.Value = 0;
							instruction->Arg1 = NULL;
							break;
						}
						default:
						{
							instruction->Opcode = IROpcode_Call_Intrinsic;
							instruction->Arg1 = (void*)is->CallType;
							break;
						}
					}
					break;
				}
			}
		}
	}
}
