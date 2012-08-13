#include <CLR/InternalCalls.h>
#include <CLR/InternalCalls/Proton.IO.PortIO.h>
#include <CLR/InternalCalls/System.Buffer.h>
#include <CLR/InternalCalls/System.Console.h>
#include <CLR/InternalCalls/System.Runtime.CompilerServices.RuntimeHelpers.h>
#include <CLR/InternalCalls/System.String.h>
#include <System/Console.h>


void Mernel_Kernel_Write(AppDomain* pAppDomain, uint16_t* pString)
{
	GCObject* object = *(GCObject**)((size_t)pString - sizeof(void*));
	for (uint32_t index = 0; index < object->String.Length; ++index)
	{
		Console_WriteCharacter((char)pString[index]);
	}
}

const InternalCall InternalCallTable[] = 
{
	{	"Mernel",							"Kernel",			"Write",				SignatureElementType_Void,		1,	{ SignatureElementType_String }, &Mernel_Kernel_Write },

	{	"Proton.IO",						"PortIO",			"InByte",				SignatureElementType_U1,		1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InByte },
	{	NULL,								NULL,				"InUShort",				SignatureElementType_U2,		1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InUShort },
	{	NULL,								NULL,				"InUInt",				SignatureElementType_U4,		1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InUInt },
	{	NULL,								NULL,				"OutByte",				SignatureElementType_Void,		2,	{ SignatureElementType_U4, SignatureElementType_U1 }, &Proton_IO_PortIO_OutByte },
	{	NULL,								NULL,				"OutUShort",			SignatureElementType_Void,		2,	{ SignatureElementType_U4, SignatureElementType_U2 }, &Proton_IO_PortIO_OutUShort },
	{	NULL,								NULL,				"OutUInt",				SignatureElementType_Void,		2,	{ SignatureElementType_U4, SignatureElementType_U4 }, &Proton_IO_PortIO_OutUInt },

	{	"System",							"Buffer",			"ByteLengthInternal",	SignatureElementType_I4,		1,	{ SignatureElementType_Class }, &System_Buffer_ByteLengthInternal },

	{	NULL,								"String",			".ctor",				SignatureElementType_Void,		1,	{ SignatureElementType_Pointer, SignatureElementType_Char }, &System_String_Ctor_CharPointer },
	{	NULL,								NULL,				NULL,					SignatureElementType_Void,		1,	{ SignatureElementType_Pointer, SignatureElementType_I1 }, &System_String_Ctor_SBytePointer },
	{	NULL,								NULL,				NULL,					SignatureElementType_Void,		1,	{ SignatureElementType_SingleDimensionArray, SignatureElementType_Char }, &System_String_Ctor_CharArray },
	{	NULL,								NULL,				NULL,					SignatureElementType_Void,		2,	{ SignatureElementType_Char, SignatureElementType_I4 }, &System_String_Ctor_CharAndCount },
	{	NULL,								NULL,				NULL,					SignatureElementType_Void,		3,	{ SignatureElementType_SingleDimensionArray, SignatureElementType_Char, SignatureElementType_I4, SignatureElementType_I4 }, &System_String_Ctor_CharArrayWithStartAndLength },
	{	NULL,								NULL,				NULL,					SignatureElementType_Void,		3,	{ SignatureElementType_Pointer, SignatureElementType_Char, SignatureElementType_I4, SignatureElementType_I4 }, &System_String_Ctor_CharPointerWithStartAndLength },
	{	NULL,								NULL,				NULL,					SignatureElementType_Void,		3,	{ SignatureElementType_Pointer, SignatureElementType_I1, SignatureElementType_I4, SignatureElementType_I4 }, &System_String_Ctor_SBytePointerWithStartAndLength },

	{	"System.Runtime.CompilerServices",	"RuntimeHelpers",	"InitializeArray",		SignatureElementType_Void,		2,	{ SignatureElementType_Class, SignatureElementType_IPointer }, &System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray },

	{	NULL,								NULL,				NULL,					SignatureElementType_End,		0,	{ }, NULL }
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