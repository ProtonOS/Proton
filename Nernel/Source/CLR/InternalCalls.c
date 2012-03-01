#include <CLR/InternalCalls/Proton.IO.MemoryIO.h>
#include <CLR/InternalCalls/Proton.IO.PortIO.h>
#include <CLR/InternalCalls/System.Array.h>
#include <CLR/InternalCalls/System.Console.h>
#include <CLR/InternalCalls/System.DateTime.h>
#include <CLR/InternalCalls/System.Environment.h>
#include <CLR/InternalCalls/System.Math.h>
#include <CLR/InternalCalls/System.Object.h>
#include <CLR/InternalCalls/System.String.h>

#include <CLR/InternalCalls.h>
#include <stdio.h>
#include <string.h>


void Panic(const char* msg);
const InternalCall InternalCallTable[] = 
{
	{	"Proton.IO",		"MemoryIO",			"InByte",				Signature_ElementType_U1,		1,	{ Signature_ElementType_U4 }, &Proton_IO_MemoryIO_InByte },
	{	NULL,				NULL,				"InUShort",				Signature_ElementType_U2,		1,	{ Signature_ElementType_U4 }, &Proton_IO_MemoryIO_InUShort },
	{	NULL,				NULL,				"InUInt",				Signature_ElementType_U4,		1,	{ Signature_ElementType_U4 }, &Proton_IO_MemoryIO_InUInt },
	{	NULL,				NULL,				"OutByte",				Signature_ElementType_Void,		2,	{ Signature_ElementType_U4, Signature_ElementType_U1 }, &Proton_IO_MemoryIO_OutByte },
	{	NULL,				NULL,				"OutUShort",			Signature_ElementType_Void,		2,	{ Signature_ElementType_U4, Signature_ElementType_U2 }, &Proton_IO_MemoryIO_OutUShort },
	{	NULL,				NULL,				"OutUInt",				Signature_ElementType_Void,		2,	{ Signature_ElementType_U4, Signature_ElementType_U4 }, &Proton_IO_MemoryIO_OutUInt },

	{	NULL,				"PortIO",			"InByte",				Signature_ElementType_U1,		1,	{ Signature_ElementType_U4 }, &Proton_IO_PortIO_InByte },
	{	NULL,				NULL,				"InUShort",				Signature_ElementType_U2,		1,	{ Signature_ElementType_U4 }, &Proton_IO_PortIO_InUShort },
	{	NULL,				NULL,				"InUInt",				Signature_ElementType_U4,		1,	{ Signature_ElementType_U4 }, &Proton_IO_PortIO_InUInt },
	{	NULL,				NULL,				"OutByte",				Signature_ElementType_Void,		2,	{ Signature_ElementType_U4, Signature_ElementType_U1 }, &Proton_IO_PortIO_OutByte },
	{	NULL,				NULL,				"OutUShort",			Signature_ElementType_Void,		2,	{ Signature_ElementType_U4, Signature_ElementType_U2 }, &Proton_IO_PortIO_OutUShort },
	{	NULL,				NULL,				"OutUInt",				Signature_ElementType_Void,		2,	{ Signature_ElementType_U4, Signature_ElementType_U4 }, &Proton_IO_PortIO_OutUInt },


	{	"System",			"Math",				"Sin",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Sin },
	{	NULL,				NULL,				"Sinh",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Sinh },
	{	NULL,				NULL,				"Asin",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Asin },
	{	NULL,				NULL,				"Cos",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Cos },
	{	NULL,				NULL,				"Cosh",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Cosh },
	{	NULL,				NULL,				"Acos",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Acos },
	{	NULL,				NULL,				"Tan",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Tan },
	{	NULL,				NULL,				"Tanh",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Tanh },
	{	NULL,				NULL,				"Atan",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Atan },
	{	NULL,				NULL,				"Atan2",				Signature_ElementType_R8,		2,	{ Signature_ElementType_R8, Signature_ElementType_R8 }, &System_Math_Atan2 },
	{	NULL,				NULL,				"Exp",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Exp },
	{	NULL,				NULL,				"Pow",					Signature_ElementType_R8,		2,	{ Signature_ElementType_R8, Signature_ElementType_R8 }, &System_Math_Pow },
	{	NULL,				NULL,				"Sqrt",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Sqrt },
	{	NULL,				NULL,				"Log",					Signature_ElementType_R8,		2,	{ Signature_ElementType_R8, Signature_ElementType_R8 }, &System_Math_Log },

	{	NULL,				"Console",			"Write",				Signature_ElementType_Void,		1,	{ Signature_ElementType_String }, &System_Console_Write },

	{	NULL,				"Object",			"Equals",				Signature_ElementType_Boolean,	1,	{ Signature_ElementType_Object }, &System_Object_Equals },
	{	NULL,				NULL,				"GetHashCode",			Signature_ElementType_I4,		0,	{ }, &System_Object_GetHashCode },

	{	NULL,				"DateTime",			"InternalUtcNow",		Signature_ElementType_I8,		0,	{ }, &System_DateTime_InternalUtcNow },

	{	NULL,				"Environment",		"get_TickCount",		Signature_ElementType_I4,		0,	{ }, &System_Environment_getTickCount },
	{	NULL,				NULL,				"GetOSVersionString",	Signature_ElementType_String,	0,	{ }, &System_Environment_GetOSVersionString },
	{	NULL,				NULL,				"get_Platform",			Signature_ElementType_ValueType,0,	{ }, &System_Environment_getPlatform },

	{	NULL,				"String",			".ctor",				Signature_ElementType_Void,		1,	{ Signature_ElementType_Pointer, Signature_ElementType_Char }, &System_String_Ctor_CharPtr },
	{	NULL,				NULL,				NULL,					Signature_ElementType_Void,		1,	{ Signature_ElementType_Pointer, Signature_ElementType_I1 }, &System_String_Ctor_SBytePtr },
	{	NULL,				NULL,				NULL,					Signature_ElementType_Void,		1,	{ Signature_ElementType_SingleDimensionArray, Signature_ElementType_Char }, &System_String_Ctor_CharArray },
	{	NULL,				NULL,				NULL,					Signature_ElementType_Void,		2,	{ Signature_ElementType_Char, Signature_ElementType_I4 }, &System_String_Ctor_CharAndCount },
	{	NULL,				NULL,				NULL,					Signature_ElementType_Void,		3,	{ Signature_ElementType_SingleDimensionArray, Signature_ElementType_Char, Signature_ElementType_I4, Signature_ElementType_I4 }, &System_String_Ctor_CharArrayAndStartAndLength },
	{	NULL,				NULL,				NULL,					Signature_ElementType_Void,		3,	{ Signature_ElementType_Pointer, Signature_ElementType_Char, Signature_ElementType_I4, Signature_ElementType_I4 }, &System_String_Ctor_CharPtrAndStartAndLength },
	{	NULL,				NULL,				NULL,					Signature_ElementType_Void,		3,	{ Signature_ElementType_Pointer, Signature_ElementType_I1, Signature_ElementType_I4, Signature_ElementType_I4 }, &System_String_Ctor_SBytePtrAndStartAndLength },
	{	NULL,				NULL,				NULL,					Signature_ElementType_Void,		3,	{ Signature_ElementType_String, Signature_ElementType_I4, Signature_ElementType_I4 }, &System_String_Ctor_StringAndStartAndLength },
	{	NULL,				NULL,				"InternalConcat",		Signature_ElementType_String,	2,	{ Signature_ElementType_String, Signature_ElementType_String }, &System_String_InternalConcat },
	{	NULL,				NULL,				"InternalReplace",		Signature_ElementType_String,	2,	{ Signature_ElementType_String, Signature_ElementType_String }, &System_String_InternalReplace },
	{	NULL,				NULL,				"InternalTrim",			Signature_ElementType_String,	2,	{ Signature_ElementType_SingleDimensionArray, Signature_ElementType_Char, Signature_ElementType_I4 }, &System_String_InternalTrim },
	{	NULL,				NULL,				"InternalIndexOf",		Signature_ElementType_I4,		4,	{ Signature_ElementType_Char, Signature_ElementType_I4, Signature_ElementType_I4, Signature_ElementType_Boolean }, &System_String_InternalIndexOf },
	{	NULL,				NULL,				"InternalIndexOfAny",	Signature_ElementType_I4,		4,	{ Signature_ElementType_SingleDimensionArray, Signature_ElementType_Char, Signature_ElementType_I4, Signature_ElementType_I4, Signature_ElementType_Boolean }, &System_String_InternalIndexOfAny },
	{	NULL,				NULL,				"Equals",				Signature_ElementType_Boolean,	2,	{ Signature_ElementType_String, Signature_ElementType_String }, &System_String_Equals },
	{	NULL,				NULL,				"GetHashCode",			Signature_ElementType_I4,		0,	{ }, &System_String_GetHashCode },
	{	NULL,				NULL,				"get_Chars",			Signature_ElementType_Char,		1,	{ Signature_ElementType_I4 }, &System_String_getChars },

	{	NULL,				"Array",			"Internal_GetValue",	Signature_ElementType_Object,	1,	{ Signature_ElementType_I4 }, &System_Array_InternalGetValue },
	{	NULL,				NULL,				"Internal_SetValue",	Signature_ElementType_Boolean,	2,	{ Signature_ElementType_Object, Signature_ElementType_I4 }, &System_Array_InternalSetValue },
	{	NULL,				NULL,				"Clear",				Signature_ElementType_Void,		3,	{ Signature_ElementType_Class, Signature_ElementType_I4, Signature_ElementType_I4 }, &System_Array_Clear },
	{	NULL,				NULL,				"Internal_Copy",		Signature_ElementType_Boolean,	5,	{ Signature_ElementType_Class, Signature_ElementType_I4, Signature_ElementType_Class, Signature_ElementType_I4, Signature_ElementType_I4 }, &System_Array_InternalCopy },
	{	NULL,				NULL,				"Reverse",				Signature_ElementType_Void,		3,	{ Signature_ElementType_Class, Signature_ElementType_I4, Signature_ElementType_I4 }, &System_Array_Reverse },

	{	NULL,				NULL,				NULL,					Signature_ElementType_End,		0,	{ }, NULL }
};

//uint32_t System_Array_InternalCopy(AppDomain* pAppDomain, ReferenceTypeObject* pSource, uint32_t pSourceIndex, ReferenceTypeObject* pDestination, uint32_t pDestinationIndex, uint32_t pLength);
//void System_Array_Reverse(AppDomain* pAppDomain, ReferenceTypeObject* pArray, uint32_t pIndex, uint32_t pLength);

InternalCallPointer ResolveInternalCall(MethodDefinition* methodDef, CLIFile* fil)
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
			if (ic->Namespace && !strcmp(ic->Namespace, methodDef->TypeDefinition->Namespace)) foundNamespace = ic->Namespace;
			else continue;
		}
		else if (ic->Namespace) break;

		if (!foundTypeName)
		{
			if (ic->TypeName && !strcmp(ic->TypeName, methodDef->TypeDefinition->Name)) foundTypeName = ic->TypeName;
			else continue;
		}
		else if (ic->TypeName) break;

		if (!foundName)
		{
			if (ic->Name && !strcmp(ic->Name, methodDef->Name)) foundName = ic->Name;
			else continue;
		}
		else if (ic->Name) break;

		if (foundName && ic->ParameterCount == methodDef->ParameterListCount)
		{
			MethodSignature* sig = MethodSignature_Expand(methodDef->Signature, fil);
			bool_t sigMatch = TRUE;
			if (sig->ReturnType->Type->ElementType == ic->ReturnType ||
				(ic->ReturnType == Signature_ElementType_Void && sig->ReturnType->Void))
			{
				for (uint32_t arg = 0, param = 0; param < ic->ParameterCount; arg++, param++)
				{
					if (ic->Args[arg] == sig->Parameters[param]->Type->ElementType)
					{
						if (ic->Args[arg] == Signature_ElementType_Pointer)
						{
							if (sig->Parameters[param]->Type->PtrType->ElementType == ic->Args[arg + 1] ||
								(ic->Args[arg + 1] == Signature_ElementType_Void && sig->Parameters[param]->Type->PtrVoid))
							{
								++arg;
							}
							else
							{
								sigMatch = FALSE;
								break;
							}
						}
						else if (ic->Args[arg] == Signature_ElementType_SingleDimensionArray)
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
			MethodSignature_Destroy(sig);
			if (sigMatch)
			{
				result = ic;
				break;
			}
		}
	}

	if (result) return result->TargetMethod;

	printf("ResolveInternalCall Missing: %s.%s.%s\n", methodDef->TypeDefinition->Namespace, methodDef->TypeDefinition->Name, methodDef->Name);
	MethodSignature* sig = MethodSignature_Expand(methodDef->Signature, fil);
	for (uint32_t index = 0; index < methodDef->ParameterListCount; ++index)
	{
		printf("  Param[%u] is %s, sig type 0x%x\n", (unsigned int)index, methodDef->ParameterList[index].Name, sig->Parameters[index]->Type->ElementType);
	}
	MethodSignature_Destroy(sig);
    //Panic("Unable to resolve internal call!");
    return 0;
}


