#include <CLR/IROptimizer.h>

#include <CLR/Optimizations/UnconditionalBranchTransform.h>

void IROptimizer_Optimize(ILAssembly* asmb)
{
    IROptimizer_UnconditionalBranchTransform(asmb);


}