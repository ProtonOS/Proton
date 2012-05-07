#pragma once

typedef struct _GCObjectHeader GCObjectHeader;

#include <CLR/IRStructures.h>

typedef enum GCObjectHeaderFlags
{
	GCObjectHeaderFlags_None = 0 << 0,
	GCObjectHeaderFlags_String = 1 << 0,
	GCObjectHeaderFlags_Array = 1 << 1,
} GCObjectHeaderFlags;

struct _GCObjectHeader
{
	IRType* Type;
	GCObjectHeaderFlags Flags;

	union
	{
		struct String
		{
			uint32_t Length;
			UT_hash_handle HashHandle;
		} String;
		struct Array
		{
			uint32_t Length;
			IRType* ElementType;
		} Array;
	};
};

