#include <CLR/InternalCalls.h>
#include <CLR/InternalCalls/Mono.Interop.ComInteropProxy.h>
#include <CLR/InternalCalls/Proton.IO.PortIO.h>
#include <CLR/InternalCalls/System.ArgIterator.h>
#include <CLR/InternalCalls/System.Array.h>
#include <CLR/InternalCalls/System.Buffer.h>
#include <CLR/InternalCalls/System.Console.h>
#include <CLR/InternalCalls/System.Convert.h>
#include <CLR/InternalCalls/System.DateTime.h>
#include <CLR/InternalCalls/System.GC.h>
#include <CLR/InternalCalls/System.MonoEnumInfo.h>
#include <CLR/InternalCalls/System.Runtime.CompilerServices.RuntimeHelpers.h>
#include <CLR/InternalCalls/System.String.h>
#include <System/Console.h>


void Mernel_Kernel_Write(AppDomain* pAppDomain, void* pString)
{
	GCObject* object = *(GCObject**)((size_t)pString - sizeof(void*));
	uint32_t length = *(uint32_t*)object->Data;
	uint16_t* stringData = (uint16_t*)((uint8_t*)object->Data + 4);
	for (uint32_t index = 0; index < length; ++index)
	{
		Console_WriteCharacter((char)stringData[index]);
	}
}

const InternalCall InternalCallTable[] = 
{
	{	"Mernel",								"Kernel",				"Write",						SignatureElementType_Void,					1,	{ SignatureElementType_String }, &Mernel_Kernel_Write },


	{	"Mono.Interop",							"ComInteropProxy",		"AddProxy",						SignatureElementType_Void,					2,	{ SignatureElementType_IPointer, SignatureElementType_Class }, &Mono_Interop_ComInteropProxy_AddProxy },
	{	NULL,									NULL,					"FindProxy",					SignatureElementType_Class,					1,	{ SignatureElementType_IPointer }, &Mono_Interop_ComInteropProxy_FindProxy },


	{	"Proton.IO",							"PortIO",				"InByte",						SignatureElementType_U1,					1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InByte },
	{	NULL,									NULL,					"InUShort",						SignatureElementType_U2,					1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InUShort },
	{	NULL,									NULL,					"InUInt",						SignatureElementType_U4,					1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InUInt },
	{	NULL,									NULL,					"OutByte",						SignatureElementType_Void,					2,	{ SignatureElementType_U4, SignatureElementType_U1 }, &Proton_IO_PortIO_OutByte },
	{	NULL,									NULL,					"OutUShort",					SignatureElementType_Void,					2,	{ SignatureElementType_U4, SignatureElementType_U2 }, &Proton_IO_PortIO_OutUShort },
	{	NULL,									NULL,					"OutUInt",						SignatureElementType_Void,					2,	{ SignatureElementType_U4, SignatureElementType_U4 }, &Proton_IO_PortIO_OutUInt },


	{	"System",								"ArgIterator",			"Setup",						SignatureElementType_Void,					2,	{ SignatureElementType_IPointer, SignatureElementType_IPointer }, &System_ArgIterator_Setup },
	{	NULL,									NULL,					"IntGetNextArg",				SignatureElementType_TypedByReference,		0,	{ }, &System_ArgIterator_IntGetNextArg },
	{	NULL,									NULL,					NULL,							SignatureElementType_TypedByReference,		1,	{ SignatureElementType_IPointer }, &System_ArgIterator_IntGetNextArg_RuntimeTypeHandle },
	{	NULL,									NULL,					"IntGetNextArgType",			SignatureElementType_IPointer,				0,	{ }, &System_ArgIterator_IntGetNextArgType },

	{	NULL,									"Array",				"FastCopy",						SignatureElementType_Boolean,				5,	{ SignatureElementType_Class, SignatureElementType_I4, SignatureElementType_Class, SignatureElementType_I4, SignatureElementType_I4 }, &System_Array_FastCopy },
	{	NULL,									NULL,					"GetRank",						SignatureElementType_I4,					0,	{ }, &System_Array_GetRank },
	{	NULL,									NULL,					"GetLength",					SignatureElementType_I4,					1,	{ SignatureElementType_I4 }, &System_Array_GetLength },
	{	NULL,									NULL,					"GetLowerBound",				SignatureElementType_I4,					1,	{ SignatureElementType_I4 }, &System_Array_GetLowerBound },
	{	NULL,									NULL,					"GetValueImpl",					SignatureElementType_Object,				1,	{ SignatureElementType_I4 }, &System_Array_GetValueImpl },
	{	NULL,									NULL,					"SetValueImpl",					SignatureElementType_Void,					2,	{ SignatureElementType_Object, SignatureElementType_I4 }, &System_Array_SetValueImpl },
	{	NULL,									NULL,					"ClearInternal",				SignatureElementType_Void,					3,	{ SignatureElementType_Class, SignatureElementType_I4, SignatureElementType_I4 }, &System_Array_ClearInternal },
	{	NULL,									NULL,					"Clone",						SignatureElementType_Object,				0,	{ }, &System_Array_Clone },

	{	NULL,									"Buffer",				"ByteLengthInternal",			SignatureElementType_I4,					1,	{ SignatureElementType_Class }, &System_Buffer_ByteLengthInternal },
	{	NULL,									NULL,					"GetByteInternal",				SignatureElementType_U1,					2,	{ SignatureElementType_Class, SignatureElementType_I4 }, &System_Buffer_GetByteInternal },
	{	NULL,									NULL,					"SetByteInternal",				SignatureElementType_Void,					3,	{ SignatureElementType_Class, SignatureElementType_I4, SignatureElementType_I4 }, &System_Buffer_SetByteInternal },
	{	NULL,									NULL,					"BlockCopyInternal",			SignatureElementType_Boolean,				5,	{ SignatureElementType_Class, SignatureElementType_I4, SignatureElementType_Class, SignatureElementType_I4, SignatureElementType_I4 }, &System_Buffer_BlockCopyInternal },

	{	NULL,									"Convert",				"InternalFromBase64String",		SignatureElementType_SingleDimensionArray,	2,	{ SignatureElementType_String, SignatureElementType_Boolean }, &System_Convert_InternalFromBase64String },
	{	NULL,									NULL,					"InternalFromBase64CharArray",	SignatureElementType_SingleDimensionArray,	3,	{ SignatureElementType_SingleDimensionArray, SignatureElementType_Char, SignatureElementType_I4, SignatureElementType_I4 }, &System_Convert_InternalFromBase64CharArray },

	{	NULL,									"DateTime",				"GetTimeMonotonic",				SignatureElementType_I8,					0,	{ }, &System_DateTime_GetTimeMonotonic },
	{	NULL,									NULL,					"GetNow",						SignatureElementType_I8,					0,	{ }, &System_DateTime_GetNow },

	{	NULL,									"GC",					"InternalCollect",				SignatureElementType_Void,					1,	{ SignatureElementType_I4 }, &System_GC_InternalCollect },

	{	NULL,									"MonoEnumInfo",			"get_enum_info",				SignatureElementType_Void,					2,	{ SignatureElementType_Class, SignatureElementType_ValueType }, &System_MonoEnumInfo_get_enum_info },

	{	NULL,									"String",				".ctor",						SignatureElementType_Void,					1,	{ SignatureElementType_Pointer, SignatureElementType_Char }, &System_String_Ctor_CharPointer },
	{	NULL,									NULL,					NULL,							SignatureElementType_Void,					1,	{ SignatureElementType_Pointer, SignatureElementType_I1 }, &System_String_Ctor_SBytePointer },
	{	NULL,									NULL,					NULL,							SignatureElementType_Void,					1,	{ SignatureElementType_SingleDimensionArray, SignatureElementType_Char }, &System_String_Ctor_CharArray },
	{	NULL,									NULL,					NULL,							SignatureElementType_Void,					2,	{ SignatureElementType_Char, SignatureElementType_I4 }, &System_String_Ctor_CharAndCount },
	{	NULL,									NULL,					NULL,							SignatureElementType_Void,					3,	{ SignatureElementType_SingleDimensionArray, SignatureElementType_Char, SignatureElementType_I4, SignatureElementType_I4 }, &System_String_Ctor_CharArrayWithStartAndLength },
	{	NULL,									NULL,					NULL,							SignatureElementType_Void,					3,	{ SignatureElementType_Pointer, SignatureElementType_Char, SignatureElementType_I4, SignatureElementType_I4 }, &System_String_Ctor_CharPointerWithStartAndLength },
	{	NULL,									NULL,					NULL,							SignatureElementType_Void,					3,	{ SignatureElementType_Pointer, SignatureElementType_I1, SignatureElementType_I4, SignatureElementType_I4 }, &System_String_Ctor_SBytePointerWithStartAndLength },

	{	"System.Runtime.CompilerServices",		"RuntimeHelpers",		"InitializeArray",				SignatureElementType_Void,					2,	{ SignatureElementType_Class, SignatureElementType_IPointer }, &System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray },
	{	NULL,									NULL,					"get_OffsetToStringData",		SignatureElementType_I4,					0,	{ }, &System_Runtime_CompilerServices_RuntimeHelpers_get_OffsetToStringData },

	{	NULL,									NULL,					NULL,							SignatureElementType_End,					0,	{ }, NULL }
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
				(ic->ReturnType == SignatureElementType_TypedByReference && sig->ReturnType->TypedByReference) ||
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