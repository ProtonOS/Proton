#include <CLR/InternalCalls/Proton.IO.MemoryIO.h>
#include <CLR/InternalCalls/Proton.IO.PortIO.h>

#include <CLR/InternalCalls.h>
#include <stdio.h>
#include <string.h>


void Panic(const char* msg);
const InternalCall InternalCallTable[] = 
{
	{	"Proton.IO",		"MemoryIO",			"InByte",			1,	{ Signature_ElementType_U4 }, &Proton_IO_MemoryIO_InByte },
	{	NULL,				NULL,				"InUShort",			1,	{ Signature_ElementType_U4 }, &Proton_IO_MemoryIO_InUShort },
	{	NULL,				NULL,				"InUInt",			1,	{ Signature_ElementType_U4 }, &Proton_IO_MemoryIO_InUInt },
	{	NULL,				NULL,				"OutByte",			2,	{ Signature_ElementType_U4, Signature_ElementType_U1 }, &Proton_IO_MemoryIO_OutByte },
	{	NULL,				NULL,				"OutUShort",		2,	{ Signature_ElementType_U4, Signature_ElementType_U2 }, &Proton_IO_MemoryIO_OutUShort },
	{	NULL,				NULL,				"OutUInt",			2,	{ Signature_ElementType_U4, Signature_ElementType_U4 }, &Proton_IO_MemoryIO_OutUInt },
	{	NULL,				"PortIO",			"InByte",			1,	{ Signature_ElementType_U4 }, &Proton_IO_PortIO_InByte },
	{	NULL,				NULL,				"InUShort",			1,	{ Signature_ElementType_U4 }, &Proton_IO_PortIO_InUShort },
	{	NULL,				NULL,				"InUInt",			1,	{ Signature_ElementType_U4 }, &Proton_IO_PortIO_InUInt },
	{	NULL,				NULL,				"OutByte",			2,	{ Signature_ElementType_U4, Signature_ElementType_U1 }, &Proton_IO_PortIO_OutByte },
	{	NULL,				NULL,				"OutUShort",		2,	{ Signature_ElementType_U4, Signature_ElementType_U2 }, &Proton_IO_PortIO_OutUShort },
	{	NULL,				NULL,				"OutUInt",			2,	{ Signature_ElementType_U4, Signature_ElementType_U4 }, &Proton_IO_PortIO_OutUInt },
	{	NULL,				NULL,				NULL,				0,	{ }, NULL }
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
				uint32_t i = 0;
				for (; i < ic->ArgCount; i++)
				{
					if (sig->Parameters[i]->Type->ElementType != ic->Args[i])
						break;
				}
				MethodSignature_Destroy(sig);
				if (i == ic->ArgCount)
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


