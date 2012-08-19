#include <CLR/InternalCalls.h>
#include <CLR/InternalCalls/Mono.Interop.ComInteropProxy.h>
#include <CLR/InternalCalls/Proton.IO.PortIO.h>
#include <CLR/InternalCalls/System.ArgIterator.h>
#include <CLR/InternalCalls/System.Array.h>
#include <CLR/InternalCalls/System.Buffer.h>
#include <CLR/InternalCalls/System.Console.h>
#include <CLR/InternalCalls/System.Convert.h>
#include <CLR/InternalCalls/System.DateTime.h>
#include <CLR/InternalCalls/System.Enum.h>
#include <CLR/InternalCalls/System.Environment.h>
#include <CLR/InternalCalls/System.GC.h>
#include <CLR/InternalCalls/System.Math.h>
#include <CLR/InternalCalls/System.MonoEnumInfo.h>
#include <CLR/InternalCalls/System.NumberFormatter.h>
#include <CLR/InternalCalls/System.Object.h>
#include <CLR/InternalCalls/System.Runtime.CompilerServices.RuntimeHelpers.h>
#include <CLR/InternalCalls/System.String.h>
#include <CLR/InternalCalls/System.Threading.Thread.h>
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

void Mernel_Kernel_Write_Long(AppDomain* pAppDomain, int64_t pValue)
{
	printf("TotalAllocated = %llu\n", (unsigned long long)pValue);
}

const InternalCall InternalCallTable[] = 
{
	{	"Mernel",								"Kernel",				"Write",							SignatureElementType_Void,					1,	{ SignatureElementType_String }, &Mernel_Kernel_Write },
	{	NULL,									NULL,					NULL,								SignatureElementType_Void,					1,	{ SignatureElementType_I8 }, &Mernel_Kernel_Write_Long },


	{	"Mono.Interop",							"ComInteropProxy",		"AddProxy",							SignatureElementType_Void,					2,	{ SignatureElementType_IPointer, SignatureElementType_Class }, &Mono_Interop_ComInteropProxy_AddProxy },
	{	NULL,									NULL,					"FindProxy",						SignatureElementType_Class,					1,	{ SignatureElementType_IPointer }, &Mono_Interop_ComInteropProxy_FindProxy },


	{	"Proton.IO",							"PortIO",				"InByte",							SignatureElementType_U1,					1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InByte },
	{	NULL,									NULL,					"InUShort",							SignatureElementType_U2,					1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InUShort },
	{	NULL,									NULL,					"InUInt",							SignatureElementType_U4,					1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InUInt },
	{	NULL,									NULL,					"OutByte",							SignatureElementType_Void,					2,	{ SignatureElementType_U4, SignatureElementType_U1 }, &Proton_IO_PortIO_OutByte },
	{	NULL,									NULL,					"OutUShort",						SignatureElementType_Void,					2,	{ SignatureElementType_U4, SignatureElementType_U2 }, &Proton_IO_PortIO_OutUShort },
	{	NULL,									NULL,					"OutUInt",							SignatureElementType_Void,					2,	{ SignatureElementType_U4, SignatureElementType_U4 }, &Proton_IO_PortIO_OutUInt },


	{	"System",								"ArgIterator",			"Setup",							SignatureElementType_Void,					2,	{ SignatureElementType_IPointer, SignatureElementType_IPointer }, &System_ArgIterator_Setup },
	{	NULL,									NULL,					"IntGetNextArg",					SignatureElementType_TypedByReference,		0,	{ }, &System_ArgIterator_IntGetNextArg },
	{	NULL,									NULL,					NULL,								SignatureElementType_TypedByReference,		1,	{ SignatureElementType_IPointer }, &System_ArgIterator_IntGetNextArg_RuntimeTypeHandle },
	{	NULL,									NULL,					"IntGetNextArgType",				SignatureElementType_IPointer,				0,	{ }, &System_ArgIterator_IntGetNextArgType },

	{	NULL,									"Array",				"FastCopy",							SignatureElementType_Boolean,				5,	{ SignatureElementType_Class, SignatureElementType_I4, SignatureElementType_Class, SignatureElementType_I4, SignatureElementType_I4 }, &System_Array_FastCopy },
	{	NULL,									NULL,					"GetRank",							SignatureElementType_I4,					0,	{ }, &System_Array_GetRank },
	{	NULL,									NULL,					"GetLength",						SignatureElementType_I4,					1,	{ SignatureElementType_I4 }, &System_Array_GetLength },
	{	NULL,									NULL,					"GetLowerBound",					SignatureElementType_I4,					1,	{ SignatureElementType_I4 }, &System_Array_GetLowerBound },
	{	NULL,									NULL,					"GetValueImpl",						SignatureElementType_Object,				1,	{ SignatureElementType_I4 }, &System_Array_GetValueImpl },
	{	NULL,									NULL,					"SetValueImpl",						SignatureElementType_Void,					2,	{ SignatureElementType_Object, SignatureElementType_I4 }, &System_Array_SetValueImpl },
	{	NULL,									NULL,					"ClearInternal",					SignatureElementType_Void,					3,	{ SignatureElementType_Class, SignatureElementType_I4, SignatureElementType_I4 }, &System_Array_ClearInternal },
	{	NULL,									NULL,					"Clone",							SignatureElementType_Object,				0,	{ }, &System_Array_Clone },

	{	NULL,									"Buffer",				"ByteLengthInternal",				SignatureElementType_I4,					1,	{ SignatureElementType_Class }, &System_Buffer_ByteLengthInternal },
	{	NULL,									NULL,					"GetByteInternal",					SignatureElementType_U1,					2,	{ SignatureElementType_Class, SignatureElementType_I4 }, &System_Buffer_GetByteInternal },
	{	NULL,									NULL,					"SetByteInternal",					SignatureElementType_Void,					3,	{ SignatureElementType_Class, SignatureElementType_I4, SignatureElementType_I4 }, &System_Buffer_SetByteInternal },
	{	NULL,									NULL,					"BlockCopyInternal",				SignatureElementType_Boolean,				5,	{ SignatureElementType_Class, SignatureElementType_I4, SignatureElementType_Class, SignatureElementType_I4, SignatureElementType_I4 }, &System_Buffer_BlockCopyInternal },

	{	NULL,									"Convert",				"InternalFromBase64String",			SignatureElementType_SingleDimensionArray,	2,	{ SignatureElementType_String, SignatureElementType_Boolean }, &System_Convert_InternalFromBase64String },
	{	NULL,									NULL,					"InternalFromBase64CharArray",		SignatureElementType_SingleDimensionArray,	3,	{ SignatureElementType_SingleDimensionArray, SignatureElementType_Char, SignatureElementType_I4, SignatureElementType_I4 }, &System_Convert_InternalFromBase64CharArray },

	{	NULL,									"DateTime",				"GetTimeMonotonic",					SignatureElementType_I8,					0,	{ }, &System_DateTime_GetTimeMonotonic },
	{	NULL,									NULL,					"GetNow",							SignatureElementType_I8,					0,	{ }, &System_DateTime_GetNow },

	{	NULL,									"Enum",					"get_value",						SignatureElementType_Object,				0,	{ }, &System_Enum_get_value },
	{	NULL,									NULL,					"get_underlying_type",				SignatureElementType_Class,					1,	{ SignatureElementType_Class }, &System_Enum_get_underlying_type },
	{	NULL,									NULL,					"compare_value_to",					SignatureElementType_I4,					1,	{ SignatureElementType_Object }, &System_Enum_compare_value_to },
	{	NULL,									NULL,					"ToObject",							SignatureElementType_Object,				2,	{ SignatureElementType_Class, SignatureElementType_Object }, &System_Enum_ToObject },
	{	NULL,									NULL,					"get_hashcode",						SignatureElementType_I4,					0,	{ }, &System_Enum_get_hashcode },

	{	NULL,									"Environment",			"GetNewLine",						SignatureElementType_String,				0,	{ }, &System_Environment_GetNewLine },
	{	NULL,									NULL,					"get_Platform",						SignatureElementType_ValueType,				0,	{ }, &System_Environment_get_Platform },
	{	NULL,									NULL,					"GetOSVersionString",				SignatureElementType_String,				0,	{ }, &System_Environment_GetOSVersionString },
	{	NULL,									NULL,					"get_TickCount",					SignatureElementType_I4,					0,	{ }, &System_Environment_get_TickCount },
	{	NULL,									NULL,					"GetWindowsFolderPath",				SignatureElementType_String,				1,	{ SignatureElementType_I4 }, &System_Environment_GetWindowsFolderPath },
	{	NULL,									NULL,					"get_ProcessorCount",				SignatureElementType_I4,					0,	{ }, &System_Environment_get_ProcessorCount },
	{	NULL,									NULL,					"internalGetGacPath",				SignatureElementType_String,				0,	{ }, &System_Environment_internalGetGacPath },
	{	NULL,									NULL,					"GetPageSize",						SignatureElementType_I4,					0,	{ }, &System_Environment_GetPageSize },

	{	NULL,									"GC",					"get_MaxGeneration",				SignatureElementType_I4,					0,	{ }, &System_GC_get_MaxGeneration },
	{	NULL,									NULL,					"InternalCollect",					SignatureElementType_Void,					1,	{ SignatureElementType_I4 }, &System_GC_InternalCollect },
	{	NULL,									NULL,					"GetGeneration",					SignatureElementType_I4,					1,	{ SignatureElementType_Object }, &System_GC_GetGeneration },
	{	NULL,									NULL,					"GetTotalMemory",					SignatureElementType_I8,					1,	{ SignatureElementType_Boolean }, &System_GC_GetTotalMemory },
	{	NULL,									NULL,					"KeepAlive",						SignatureElementType_Void,					1,	{ SignatureElementType_Object }, &System_GC_KeepAlive },
	{	NULL,									NULL,					"ReRegisterForFinalize",			SignatureElementType_Void,					1,	{ SignatureElementType_Object }, &System_GC_ReRegisterForFinalize },
	{	NULL,									NULL,					"SuppressFinalize",					SignatureElementType_Void,					1,	{ SignatureElementType_Object }, &System_GC_SuppressFinalize },
	{	NULL,									NULL,					"WaitForPendingFinalizers",			SignatureElementType_Void,					0,	{ }, &System_GC_WaitForPendingFinalizers },
	{	NULL,									NULL,					"CollectionCount",					SignatureElementType_I4,					1,	{ SignatureElementType_I4 }, &System_GC_CollectionCount },
	{	NULL,									NULL,					"RecordPressure",					SignatureElementType_Void,					1,	{ SignatureElementType_I8 }, &System_GC_RecordPressure },

	{	NULL,									"Math",					"Floor",							SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Floor },
	{	NULL,									NULL,					"Round",							SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Round },
	{	NULL,									NULL,					"Sin",								SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Sin },
	{	NULL,									NULL,					"Cos",								SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Cos },
	{	NULL,									NULL,					"Tan",								SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Tan },
	{	NULL,									NULL,					"Sinh",								SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Sinh },
	{	NULL,									NULL,					"Cosh",								SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Cosh },
	{	NULL,									NULL,					"Tanh",								SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Tanh },
	{	NULL,									NULL,					"Acos",								SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Acos },
	{	NULL,									NULL,					"Asin",								SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Asin },
	{	NULL,									NULL,					"Atan",								SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Atan },
	{	NULL,									NULL,					"Atan2",							SignatureElementType_R8,					2,	{ SignatureElementType_R8, SignatureElementType_R8 }, &System_Math_Atan2 },
	{	NULL,									NULL,					"Exp",								SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Exp },
	{	NULL,									NULL,					"Log",								SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Log },
	{	NULL,									NULL,					"Log10",							SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Log10 },
	{	NULL,									NULL,					"Pow",								SignatureElementType_R8,					2,	{ SignatureElementType_R8, SignatureElementType_R8 }, &System_Math_Pow },
	{	NULL,									NULL,					"Sqrt",								SignatureElementType_R8,					1,	{ SignatureElementType_R8 }, &System_Math_Sqrt },

	{	NULL,									"MonoEnumInfo",			"get_enum_info",					SignatureElementType_Void,					2,	{ SignatureElementType_Class, SignatureElementType_ValueType }, &System_MonoEnumInfo_get_enum_info },

	{	NULL,									"NumberFormatter",		"GetFormatterTables",				SignatureElementType_Void,					6,	{ SignatureElementType_Pointer, SignatureElementType_U8, SignatureElementType_Pointer, SignatureElementType_I4, SignatureElementType_Pointer, SignatureElementType_Char, SignatureElementType_Pointer, SignatureElementType_Char, SignatureElementType_Pointer, SignatureElementType_I8, SignatureElementType_Pointer, SignatureElementType_I4 }, &System_NumberFormatter_GetFormatterTables },

	{	NULL,									"Object",				"GetType",							SignatureElementType_Class,					0,	{ }, &System_Object_GetType },
	{	NULL,									NULL,					"InternalGetHashCode",				SignatureElementType_I4,					1,	{ SignatureElementType_Object }, &System_Object_InternalGetHashCode },
	{	NULL,									NULL,					"obj_address",						SignatureElementType_IPointer,				0,	{ }, &System_Object_obj_address },

	{	NULL,									"String",				".ctor",							SignatureElementType_Void,					1,	{ SignatureElementType_Pointer, SignatureElementType_Char }, &System_String_Ctor_CharPointer },
	{	NULL,									NULL,					NULL,								SignatureElementType_Void,					1,	{ SignatureElementType_Pointer, SignatureElementType_I1 }, &System_String_Ctor_SBytePointer },
	{	NULL,									NULL,					NULL,								SignatureElementType_Void,					1,	{ SignatureElementType_SingleDimensionArray, SignatureElementType_Char }, &System_String_Ctor_CharArray },
	{	NULL,									NULL,					NULL,								SignatureElementType_Void,					2,	{ SignatureElementType_Char, SignatureElementType_I4 }, &System_String_Ctor_CharAndCount },
	{	NULL,									NULL,					NULL,								SignatureElementType_Void,					3,	{ SignatureElementType_SingleDimensionArray, SignatureElementType_Char, SignatureElementType_I4, SignatureElementType_I4 }, &System_String_Ctor_CharArrayWithStartAndLength },
	{	NULL,									NULL,					NULL,								SignatureElementType_Void,					3,	{ SignatureElementType_Pointer, SignatureElementType_Char, SignatureElementType_I4, SignatureElementType_I4 }, &System_String_Ctor_CharPointerWithStartAndLength },
	{	NULL,									NULL,					NULL,								SignatureElementType_Void,					3,	{ SignatureElementType_Pointer, SignatureElementType_I1, SignatureElementType_I4, SignatureElementType_I4 }, &System_String_Ctor_SBytePointerWithStartAndLength },

	{	"System.Runtime.CompilerServices",		"RuntimeHelpers",		"InitializeArray",					SignatureElementType_Void,					2,	{ SignatureElementType_Class, SignatureElementType_IPointer }, &System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray },
	{	NULL,									NULL,					"get_OffsetToStringData",			SignatureElementType_I4,					0,	{ }, &System_Runtime_CompilerServices_RuntimeHelpers_get_OffsetToStringData },

	{	"System.Threading",						"Thread",				"CurrentInternalThread_internal",	SignatureElementType_Class,					0,	{ }, &System_Threading_Thread_CurrentInternalThread_internal },

	{	NULL,									NULL,					NULL,								SignatureElementType_End,					0,	{ }, NULL }
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