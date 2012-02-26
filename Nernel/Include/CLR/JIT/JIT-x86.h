#pragma once

#include <CLR/OpCodes_IR.h>

#define GetSizeOfElementType(dest, value) \
	switch (value) \
	{ \
		case ElementType_I1: \
		case ElementType_U1: \
			dest = 1; \
			break; \
		case ElementType_I2: \
		case ElementType_U2: \
			dest = 2; \
			break; \
		case ElementType_I: \
		case ElementType_U: \
		case ElementType_I4: \
		case ElementType_U4: \
		case ElementType_R4: \
		case ElementType_Ref: \
		case ElementType_ManagedPointer: \
			dest = 4; \
			break; \
		case ElementType_I8: \
		case ElementType_U8: \
		case ElementType_R8: \
			dest = 8; \
			break; \
		default: Panic("Unknown ElementType to GetSizeOfElementType"); break; \
	}
