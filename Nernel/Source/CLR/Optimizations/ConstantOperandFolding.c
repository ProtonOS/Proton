#include <CLR/OpCodes_IR.h>
#include <CLR/Optimizations/ConstantOperandFolding.h>
#include <String_Format.h>
#include <stdlib.h>
#include <stdio.h>


#define VariableCount 0x50

static const signed char OpCodeStackTable[] =
{
//	 PushCount		 PopCount
		0,				0,					//  0
		0,				0,					//  1
		0,		  VariableCount,			//  2	(Return)
		1,				0,					//  3
		1,				0,					//  4
		1,				0,					//  5
		1,				0,					//  6
		0,		  VariableCount,			//  7	(Branch)
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
		1,				0,					// 32
		1,		  VariableCount,			// 33	(NewObject)
		1,				1,					// 34
		1,				0,					// 35
		1,				2,					// 36
		1,				2,					// 37
		1,				2,					// 38
		1,				1,					// 39
		1,				1,					// 40
  VariableCount,  VariableCount,			// 41	(Call)
  VariableCount,  VariableCount,			// 42	(Call_Absolute)
  VariableCount,  VariableCount,			// 43	(Call_Internal)
		1,				0,					// 44
		1,				1,					// 45
		1,				1,					// 46
		0,				2,					// 47
		1,				0,					// 48
		1,				0,					// 49
		0,				1,					// 50
		1,				1,					// 51
		0,				2,					// 52
		0,				2,					// 53
  VariableCount,  VariableCount,			// 54	(Jump)
		0,				1,					// 55
		1,				1,					// 56
		1,				1,					// 57
		1,				1,					// 58
		1,				1,					// 59
		1,				1,					// 60
		1,				2,					// 61
		0,				0,					// 62
		1,				1,					// 63
		0,				1,					// 64
		1,				0,					// 65
		1,				0,					// 66
		1,				0,					// 67
		0,				3,					// 68
		0,				3,					// 69
		0,				1,					// 70
};

typedef struct _ConstantStackObject ConstantStackObject;
typedef struct _Stack Stack;

struct _ConstantStackObject
{
	uint32_t Value32;
	uint64_t Value64;
	bool_t IsConstant32;
	bool_t IsConstant64;
	uint32_t IROpLoc; // Only valid if this object's value is constant.
	ConstantStackObject* NextObj;
};

struct _Stack
{
	uint32_t StackDepth;
	ConstantStackObject* TopObj;
};

void Destroy_StackObj(ConstantStackObject* obj)
{
	free(obj);
}

void Stack_PushConstant32(Stack* stack, uint32_t val, uint32_t IRLoc)
{
	ConstantStackObject* obj = (ConstantStackObject*)calloc(1, sizeof(ConstantStackObject));
	obj->IsConstant32 = TRUE;
	obj->Value32 = val;
	obj->NextObj = stack->TopObj;
	obj->IROpLoc = IRLoc;
	stack->TopObj = obj;
	stack->StackDepth++;
	//printf("Pushing a Constant32 to the stack, stack depth: %i\n", (int)stack->StackDepth);
}

void Stack_PushConstant64(Stack* stack, uint64_t val, uint32_t IRLoc)
{
	ConstantStackObject* obj = (ConstantStackObject*)calloc(1, sizeof(ConstantStackObject));
	obj->IsConstant64 = TRUE;
	obj->Value64 = val;
	obj->NextObj = stack->TopObj;
	obj->IROpLoc = IRLoc;
	stack->TopObj = obj;
	stack->StackDepth++;
	//printf("Pushing a Constant64 to the stack, stack depth: %i\n", (int)stack->StackDepth);
}

void Stack_PushAny(Stack* stack)
{
	ConstantStackObject* obj = (ConstantStackObject*)calloc(1, sizeof(ConstantStackObject));
	obj->NextObj = stack->TopObj;
	stack->TopObj = obj;
	stack->StackDepth++;
	//printf("Pushing any to the stack, stack depth: %i\n", (int)stack->StackDepth);
}

void Stack_PopAny(Stack* stack)
{
	if (stack->StackDepth <= 0)
		Panic("Tried to pop from an empty stack!");
	ConstantStackObject* obj = stack->TopObj;
	stack->TopObj = obj->NextObj;
	Destroy_StackObj(obj);
	stack->StackDepth--;
	//printf("Popping any from the stack, stack depth: %i\n", (int)stack->StackDepth);
}

ConstantStackObject* Stack_Pop(Stack* stack)
{
	if (stack->StackDepth <= 0)
		Panic("Tried to pop from an empty stack!");
	ConstantStackObject* obj = stack->TopObj;
	stack->TopObj = obj->NextObj;
	stack->StackDepth--;
	//printf("Popping from the stack, stack depth: %i\n", (int)stack->StackDepth);
	return obj;
}

void IROptimizer_ConstantOperandFolding(IRMethod* mth)
{
	Stack* stack = (Stack*)calloc(1, sizeof(Stack));
	// Create the root object.
	stack->TopObj = (ConstantStackObject*)calloc(1, sizeof(ConstantStackObject));

	for (uint32_t i = 0; i < mth->IRCodesCount; i++)
	{
		IRInstruction* instr = mth->IRCodes[i];
		printf("Constant folding for op-code: %i\n", (int)instr->OpCode);
		switch(instr->OpCode)
		{
			case IROpCode_LoadInt32_Val:
				Stack_PushConstant32(stack, *((uint32_t*)instr->Arg1), i);
				break;
			case IROpCode_LoadInt64_Val:
				Stack_PushConstant64(stack, *((uint64_t*)instr->Arg1), i);
				break;
				
			case IROpCode_Add:
			case IROpCode_Sub:
			{
				ConstantStackObject* objA = Stack_Pop(stack);
				ConstantStackObject* objB = Stack_Pop(stack);
				if (objA->IsConstant32)
				{
					instr->HasConstant32Arg1 = TRUE;
					instr->Constant32Arg1 = objA->Value32;
					// Now deal with the op-code that pushed this value.
					free(mth->IRCodes[objA->IROpLoc]->Arg1);
					mth->IRCodes[objA->IROpLoc]->OpCode = IROpCode_Nop;
					mth->IRCodes[objA->IROpLoc]->Arg1NeedsDisposing = FALSE;
					mth->IRCodes[objA->IROpLoc]->Arg1 = (uint32_t*)1;
				}
				else if (objB->IsConstant32)
				{
					instr->HasConstant32Arg2 = TRUE;
					instr->Constant32Arg2 = objB->Value32;
					// Now deal with the op-code that pushed this value.
					free(mth->IRCodes[objB->IROpLoc]->Arg1);
					mth->IRCodes[objB->IROpLoc]->OpCode = IROpCode_Nop;
					mth->IRCodes[objB->IROpLoc]->Arg1NeedsDisposing = FALSE;
					mth->IRCodes[objB->IROpLoc]->Arg1 = (uint32_t*)1;
				}
				// Cleanup a bit.
				Destroy_StackObj(objA);
				Destroy_StackObj(objB);

				Stack_PushAny(stack); // And push for the result.
				break;
			}

			default: // Otherwise it's not one we deal with as a constant.
			{
				uint32_t pushCount = OpCodeStackTable[((uint32_t)instr->OpCode << 1)];
				uint32_t popCount = OpCodeStackTable[((uint32_t)instr->OpCode << 1) + 1];
				// Adjust any push/pop counts.
				switch(instr->OpCode)
				{
					case IROpCode_Return:
						if (mth->Returns)
							popCount = 1;
						else
							popCount = 0;
						break;

					case IROpCode_Branch:
						if ((*(BranchCondition*)instr->Arg1) == BranchCondition_False || (*(BranchCondition*)instr->Arg1) == BranchCondition_True)
						{
							popCount = 1;
						}
						else // We don't need to deal with always here, because it's already optimized out.
						{
							popCount = 2;
						}
						break;
					
					case IROpCode_NewObject:
						popCount = (((IRMethod*)instr->Arg1)->ParameterCount) - 1;
						break;

					case IROpCode_Call:
						popCount = (((IRMethodSpec*)instr->Arg1)->ParentType->Methods[((IRMethodSpec*)instr->Arg1)->MethodIndex]->ParameterCount);
						pushCount = (((IRMethodSpec*)instr->Arg1)->ParentType->Methods[((IRMethodSpec*)instr->Arg1)->MethodIndex]->Returns);
						break;

					case IROpCode_Call_Absolute:
						popCount = ((IRMethod*)instr->Arg1)->ParameterCount;
						pushCount = ((IRMethod*)instr->Arg1)->Returns;
						break;
					
					case IROpCode_Call_Internal:
						popCount = ((IRMethod*)instr->Arg2)->ParameterCount;
						pushCount = ((IRMethod*)instr->Arg2)->Returns;
						break;

					case IROpCode_Jump:
						popCount = ((IRMethod*)instr->Arg1)->ParameterCount;
						pushCount = ((IRMethod*)instr->Arg1)->Returns;
						break;

					default:
						break;
				}
				for (uint32_t i2 = 0; i2 < popCount; i2++)
				{
					Stack_PopAny(stack);
				}
				for (uint32_t i2 = 0; i2 < pushCount; i2++)
				{
					Stack_PushAny(stack);
				}
				break;
			}
		}

	}
	
	if (stack->StackDepth > 0)
		Panic(String_Format("Something went wrong in the constant folding, there are/is %i object(s) left on the stack!", (int)stack->StackDepth));

	Destroy_StackObj(stack->TopObj); // destroy the root object.

}
