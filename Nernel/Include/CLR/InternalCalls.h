#pragma once

#include <CLR/CLIFile.h>

typedef struct _InternalCall InternalCall;

#define MAX_INTERNAL_ARGS 6

struct _InternalCall
{
	const char* Namespace;
	const char* TypeName;
	const char* Name;
	const uint8_t ReturnType;
	const uint32_t ParameterCount;
	const uint8_t Args[MAX_INTERNAL_ARGS];
	void* TargetMethod;
};

InternalCallPointer ResolveInternalCall(MethodDefinition* methodDef, CLIFile* fil);