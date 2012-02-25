#include <CLR/InternalCalls/Proton.IO.MemoryIO.h>
#include <CLR/InternalCalls/Proton.IO.PortIO.h>
#include <CLR/InternalCalls/System.Math.h>
#include <CLR/InternalCalls/System.Console.h>
#include <CLR/InternalCalls/System.Object.h>
#include <CLR/InternalCalls/System.DateTime.h>
#include <CLR/InternalCalls/System.Environment.h>
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
	{	NULL,				NULL,				".ctor",				Signature_ElementType_Void,		3,	{ Signature_ElementType_Pointer, Signature_ElementType_Char, Signature_ElementType_I4, Signature_ElementType_I4 }, &System_String_Ctor_CharPtrAndStartAndLength },
	{	NULL,				NULL,				".ctor",				Signature_ElementType_Void,		1,	{ Signature_ElementType_Pointer, Signature_ElementType_I1 }, &System_String_Ctor_SBytePtr },
	{	NULL,				NULL,				".ctor",				Signature_ElementType_Void,		3,	{ Signature_ElementType_Pointer, Signature_ElementType_I1, Signature_ElementType_I4, Signature_ElementType_I4 }, &System_String_Ctor_SBytePtrAndStartAndLength },
	{	NULL,				NULL,				".ctor",				Signature_ElementType_Void,		2,	{ Signature_ElementType_Char, Signature_ElementType_I4 }, &System_String_Ctor_CharAndCount },
	{	NULL,				NULL,				".ctor",				Signature_ElementType_Void,		3,	{ Signature_ElementType_String, Signature_ElementType_I4, Signature_ElementType_I4 }, &System_String_Ctor_StringAndStartAndLength },
	{	NULL,				NULL,				"InternalConcat",		Signature_ElementType_String,	2,	{ Signature_ElementType_String, Signature_ElementType_String }, &System_String_InternalConcat },
	{	NULL,				NULL,				"InternalReplace",		Signature_ElementType_String,	2,	{ Signature_ElementType_String, Signature_ElementType_String }, &System_String_InternalReplace },
	{	NULL,				NULL,				"InternalIndexOf",		Signature_ElementType_I4,		4,	{ Signature_ElementType_Char, Signature_ElementType_I4, Signature_ElementType_I4, Signature_ElementType_Boolean }, &System_String_InternalIndexOf },
	{	NULL,				NULL,				"Equals",				Signature_ElementType_Boolean,	2,	{ Signature_ElementType_String, Signature_ElementType_String }, &System_String_Equals },

	{	NULL,				NULL,				NULL,					Signature_ElementType_End,		0,	{ }, NULL }
};

InternalCallPointer ResolveInternalCall(MethodDefinition* methodDef, CLIFile* fil)
{
	const InternalCall* ic = NULL;
	const char* lastNamespace = NULL;
	const char* lastTypeName = NULL;
	const char* lastName = NULL;
	for (uint32_t index = 0; InternalCallTable[index].TargetMethod != NULL; ++index)
	{
		ic = &InternalCallTable[index];
		if (lastNamespace == NULL)
		{
			if (ic->Namespace && !strcmp(ic->Namespace, methodDef->TypeDefinition->Namespace))
			{
				lastNamespace = ic->Namespace;
			}
		}
		else if (ic->Namespace != NULL) goto LookupFailed;

		if (lastTypeName == NULL)
		{
			if (ic->TypeName && !strcmp(ic->TypeName, methodDef->TypeDefinition->Name))
			{
				lastTypeName = ic->Name;
			}
		}
		else if (ic->TypeName != NULL) goto LookupFailed;

		if (lastName == NULL)
		{
			if (ic->Name && !strcmp(ic->Name, methodDef->Name) && ic->ArgCount == methodDef->ParameterListCount)
			{
				MethodSignature* sig = MethodSignature_Expand(methodDef->Signature, fil);
				bool_t sigMatch = TRUE;
				if (sig->ReturnType->Type->ElementType == ic->ReturnType ||
					(ic->ReturnType == Signature_ElementType_Void && sig->ReturnType->Void))
				{
					for (uint32_t i = 0; i < ic->ArgCount; i++)
					{
						if (sig->Parameters[i]->Type->ElementType != ic->Args[i])
						{
							if (ic->Args[i] == Signature_ElementType_Pointer && sig->Parameters[i]->Type->ElementType == Signature_ElementType_Pointer)
							{
								if (i == ic->ArgCount - 1) Panic("Missing pointer type for internal method parameters");

								if (sig->Parameters[i]->Type->PtrType->ElementType == ic->Args[i + 1] ||
									(ic->Args[i + 1] == Signature_ElementType_Void && sig->Parameters[i]->Type->PtrVoid))
								{
									++i;
								}
								else
								{
									sigMatch = FALSE;
									break;
								}
							}
							else if (ic->Args[i] == Signature_ElementType_SingleDimensionArray && sig->Parameters[i]->Type->ElementType == Signature_ElementType_SingleDimensionArray)
							{
								if (i == ic->ArgCount - 1) Panic("Missing single dimension array type for internal method parameters");

								if (sig->Parameters[i]->Type->SZArrayType->ElementType == ic->Args[i + 1])
								{
									++i;
								}
								else
								{
									sigMatch = FALSE;
									break;
								}
							}
							else
							{
								sigMatch = FALSE;
								break;
							}
						}
					}
				}
				else sigMatch = FALSE;
				MethodSignature_Destroy(sig);
				if (sigMatch)
					return ic->TargetMethod;
			}
		}
		else if (ic->Name != NULL) goto LookupFailed;
	}

LookupFailed:
	printf("ResolveInternalCall Missing: %s.%s.%s\n", methodDef->TypeDefinition->Namespace, methodDef->TypeDefinition->Name, methodDef->Name);
    //Panic("Unable to resolve internal call!");
    return 0;
}


