#pragma once

#include <CLR/IROptimizer.h>

typedef enum IntrinsicCallType
{
	IntrinsicCallType___NULL = 0,
	IntrinsicCallType___System_Runtime_CompilerServices___RuntimeHelpers___get_OffsetToStringData,
} IntrinsicCallType;

void IROptimizer_IntrinsicSubstitution(IRMethod* pMethod);
