#pragma once

#include <CLR/IRStructures.h>

/*
    Transforms unconditional branch instructions
    into jump instructions.
 */
void IROptimizer_UnconditionalBranchTransform(ILAssembly* asmb);