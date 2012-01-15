#include <CLR/IROptimizer.h>

#include <CLR/Optimizations/UnconditionalBranchTransform.h>

void IROptimizer_Optimize(IRAssembly* asmb)
{
    IROptimizer_UnconditionalBranchTransform(asmb);


}