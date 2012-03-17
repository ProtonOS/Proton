#include <CLR/IROptimizer.h>

#include <CLR/Optimizations/UnconditionalBranchTransform.h>

void IROptimizer_Optimize(IRMethod* mth)
{

    IROptimizer_UnconditionalBranchTransform(mth);


}