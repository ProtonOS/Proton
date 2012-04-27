#include <CLR/IROptimizer.h>
#include <CLR/Optimizations/EnterSSA.h>
#include <CLR/Optimizations/LeaveSSA.h>

void IROptimizer_Optimize(IRMethod* pMethod)
{


	IROptimizer_EnterSSA(pMethod);

	IROptimizer_LeaveSSA(pMethod);
}
