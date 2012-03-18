#include <CLR/OpCodes_IR.h>
#include <CLR/Optimizations/ConstantFolding.h>
#include <stdlib.h>


#define VariableCount 0xFF

// This table is in the format of:
//		PushCount,			PopCount
static const char OpCodeStackTable[] =
{
		0,			VariableCount
};

void IROptimizer_ConstantFolding(IRMethod* mth)
{

}