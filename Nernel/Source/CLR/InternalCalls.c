#include <CLR/InternalCalls.h>
#include <CLR/InternalCalls/Proton.IO.PortIO.h>
#include <CLR/InternalCalls/System.String.h>


const InternalCall InternalCallTable[] = 
{
	{	"Proton.IO",		"PortIO",			"InByte",				SignatureElementType_U1,		1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InByte },
	{	NULL,				NULL,				"InUShort",				SignatureElementType_U2,		1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InUShort },
	{	NULL,				NULL,				"InUInt",				SignatureElementType_U4,		1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InUInt },
	{	NULL,				NULL,				"OutByte",				SignatureElementType_Void,		2,	{ SignatureElementType_U4, SignatureElementType_U1 }, &Proton_IO_PortIO_OutByte },
	{	NULL,				NULL,				"OutUShort",			SignatureElementType_Void,		2,	{ SignatureElementType_U4, SignatureElementType_U2 }, &Proton_IO_PortIO_OutUShort },
	{	NULL,				NULL,				"OutUInt",				SignatureElementType_Void,		2,	{ SignatureElementType_U4, SignatureElementType_U4 }, &Proton_IO_PortIO_OutUInt },

	{	"System",			"String",			".ctor",				SignatureElementType_Void,		2,	{ SignatureElementType_Char, SignatureElementType_I4 }, &System_String_Ctor_CharAndCount },

	{	NULL,				NULL,				NULL,					SignatureElementType_End,		0,	{ }, NULL }
};

void* ILDecomposition_ResolveInternalCall(MethodDefinition* pMethodDefinition, CLIFile* pFile)
{
	const InternalCall* result = NULL;
	const InternalCall* ic = NULL;
	const char* foundNamespace = NULL;
	const char* foundTypeName = NULL;
	const char* foundName = NULL;
	for (uint32_t index = 0; InternalCallTable[index].TargetMethod; ++index)
	{
		ic = &InternalCallTable[index];
		if (!foundNamespace)
		{
			if (ic->Namespace && !strcmp(ic->Namespace, pMethodDefinition->TypeDefinition->Namespace)) foundNamespace = ic->Namespace;
			else continue;
		}
		else if (ic->Namespace) break;

		if (!foundTypeName)
		{
			if (ic->TypeName && !strcmp(ic->TypeName, pMethodDefinition->TypeDefinition->Name)) foundTypeName = ic->TypeName;
			else continue;
		}
		else if (ic->TypeName) break;

		if (!foundName)
		{
			if (ic->Name && !strcmp(ic->Name, pMethodDefinition->Name)) foundName = ic->Name;
			else continue;
		}
		else if (ic->Name) break;

		if (foundName && ic->ParameterCount == pMethodDefinition->ParameterListCount)
		{
			MethodSignature* sig = NULL;
			if (!(sig = pMethodDefinition->SignatureCache))
			{
				pMethodDefinition->SignatureCache = MethodSignature_Expand(pMethodDefinition->Signature, pFile);
				sig = pMethodDefinition->SignatureCache;
			}
			bool_t sigMatch = TRUE;
			if ((ic->ReturnType == SignatureElementType_Void && sig->ReturnType->Void) ||
				sig->ReturnType->Type->ElementType == ic->ReturnType)
			{
				for (uint32_t arg = 0, param = 0; param < ic->ParameterCount; arg++, param++)
				{
					if (ic->Args[arg] == sig->Parameters[param]->Type->ElementType)
					{
						if (ic->Args[arg] == SignatureElementType_Pointer)
						{
							if (sig->Parameters[param]->Type->PtrType->ElementType == ic->Args[arg + 1] ||
								(ic->Args[arg + 1] == SignatureElementType_Void && sig->Parameters[param]->Type->PtrVoid))
							{
								++arg;
							}
							else
							{
								sigMatch = FALSE;
								break;
							}
						}
						else if (ic->Args[arg] == SignatureElementType_SingleDimensionArray)
						{
							if (sig->Parameters[param]->Type->SZArrayType->ElementType == ic->Args[arg + 1])
							{
								++arg;
							}
							else
							{
								sigMatch = FALSE;
								break;
							}
						}
					}
					else
					{
						sigMatch = FALSE;
						break;
					}
				}
			}
			else sigMatch = FALSE;
			if (sigMatch)
			{
				result = ic;
				break;
			}
		}
	}

	if (result) return result->TargetMethod;

	MethodSignature* sig = NULL;
	if (!(sig = pMethodDefinition->SignatureCache))
	{
		pMethodDefinition->SignatureCache = MethodSignature_Expand(pMethodDefinition->Signature, pFile);
		sig = pMethodDefinition->SignatureCache;
	}

	Log_WriteLine(LOGLEVEL__Link_Internals, "InternalCall Missing: %s.%s.%s", pMethodDefinition->TypeDefinition->Namespace, pMethodDefinition->TypeDefinition->Name, pMethodDefinition->Name);
	if (sig->ReturnType->Void) { Log_WriteLine(LOGLEVEL__Link_Internals, "                      Returns  = Void"); }
	else if (sig->ReturnType->TypedByReference) { Log_WriteLine(LOGLEVEL__Link_Internals, "                      Returns  = TypedReference"); }
	else { Log_WriteLine(LOGLEVEL__Link_Internals, "                      Returns  = type 0x%x", sig->ReturnType->Type->ElementType); }
	for (uint32_t index = 0; index < pMethodDefinition->ParameterListCount; ++index) { Log_WriteLine(LOGLEVEL__Link_Internals, "                      Param[%u] = %s, type 0x%x", (unsigned int)index, pMethodDefinition->ParameterList[index].Name, sig->Parameters[index]->Type->ElementType); }

    return NULL;
}