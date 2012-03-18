#pragma once

#include <CLR/IRStructures.h>

/*
    Folds constant integers into operands
	for certain operations.
 */
void IROptimizer_ConstantOperandFolding(IRMethod* mth);