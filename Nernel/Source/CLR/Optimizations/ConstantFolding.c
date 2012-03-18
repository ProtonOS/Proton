#include <CLR/OpCodes_IR.h>
#include <CLR/Optimizations/ConstantFolding.h>
#include <stdlib.h>


// This must be a valid positive
// signed char, so as to not cause
// issues farther down when comparing.
#define VariableCount 0x50

static const signed char OpCodeStackTable[] =
{
//	 PushCount		 PopCount
		0,				0,					//  0
		0,				0,					//  1
		0,		  VariableCount,			//  2
		1,				0,					//  3
		1,				0,					//  4
		1,				0,					//  5
		1,				0,					//  6
		0,		  VariableCount,			//  7
		0,				0,					//  8
		0,				1,					//  9
		1,				0,					// 10
		1,				0,					// 11
		1,				1,					// 12
		1,				1,					// 13
		1,				0,					// 14
		1,				0,					// 15
		0,				1,					// 16
		1,				2,					// 17
		1,				2,					// 18
		1,				2,					// 19
		0,				3,					// 20
		0,				3,					// 21
		1,				1,					// 22
		0,				1,					// 23
		1,				2,					// 24
		1,				2,					// 25
		1,				2,					// 26
		1,				2,					// 27
		1,				2,					// 28
		1,				2,					// 29
		1,				1,					// 30
		0,				2,					// 31
		0,				1,					// 32
		1,		  VariableCount,			// 33
		1,				1,					// 34
		1,				0,					// 35
		1,				2,					// 36
		1,				2,					// 37
		1,				2,					// 38
		1,				1,					// 39
		1,				1,					// 40
  VariableCount,  VariableCount,			// 41
  VariableCount,  VariableCount,			// 42
  VariableCount,  VariableCount,			// 43
  // Load String
};

void IROptimizer_ConstantFolding(IRMethod* mth)
{

}