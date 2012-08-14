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
