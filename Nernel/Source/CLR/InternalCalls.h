#pragma once

typedef struct _InternalCall InternalCall;

#include <CLR/IRStructures.h>


#define MAX_INTERNAL_ARGS 16

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


extern const InternalCall InternalCallTable[];