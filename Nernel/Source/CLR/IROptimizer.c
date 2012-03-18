#include <CLR/IROptimizer.h>

#include <CLR/Optimizations/UnconditionalBranchTransform.h>
#include <CLR/Optimizations/ConstantFolding.h>

void IROptimizer_Optimize(IRMethod* mth)
{

    IROptimizer_UnconditionalBranchTransform(mth);
	IROptimizer_ConstantFolding(mth);

}