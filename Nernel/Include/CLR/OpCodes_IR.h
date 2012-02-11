#pragma once

typedef enum IROpCode
{
    /*
        Performs no operation.
        This is only here to allow for
        future debugger integration.

        Arg1:
            N/A

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Nop,
    /*
        Stops execution and passes
        control off to the debugger.

        Arg1:
            N/A

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_BreakForDebugger,
    /*
        Returns from the current method.

        Arg1:
            A boolean representing whether
			there is actually a value to return.

        Arg2:
            The type of value to return.
			Of the type ElementType.
			Only valid if Arg1 is true.

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Return,
    /*
        Loads an int32 value onto the 
        top of the stack.

        Arg1:
            The value to load.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_LoadInt32_Val,
    /*
        Loads an int64 value onto the 
        top of the stack.

        Arg1:
            The value to load.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_LoadInt64_Val,
    /*
        Loads an float32 value onto the 
        top of the stack.

        Arg1:
            The value to load.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_LoadFloat32_Val,
    /*
        Loads an float64 value onto the 
        top of the stack.

        Arg1:
            The value to load.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_LoadFloat64_Val,
    /*
        Branches to a specific instruction.
        Takes it's arguments from the stack.

        Arg1:
            The condition of the branch,
            of the type BranchCondition.

        Arg2:
            The branch target, a pointer to 
            a structure of the type IRInstruction.

        Arg3:
            Provided that Arg1 is not
			BranchCondition_Always, this
			is the type of object that
			is the first argument for 
			the branch operation.
			Of the type ElementType.

        Arg4:
            Provided that Arg2 is not
			BranchCondition_Always,
			BranchCondition_True, or
			BranchCondition_False,
			this is the type of object that
			is the first argument for 
			the branch operation.
			Of the type ElementType.

     */
    IROpCode_Branch,
    /*
        Unconditionally jumps to a specific
        instruction. This is created by the
        optimization phase.

        Arg1:
            The jump target, a pointer to 
            a structure of the type IRInstruction.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Jump,
    /*
        Stores the value at the top of the
        stack to the specified local variable.

        Arg1:
            The index of the local variable
            to store the data in.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Store_LocalVar,
    /*
        Loads the value in the specified 
        local variable and pushes it to 
        the top of the stack.

        Arg1:
            The index of the local variable
            to load the data from.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Load_LocalVar,
    /*
        Loads the address of the specified 
        local variable and pushes it to 
        the top of the stack.

        Arg1:
            The index of the local variable
            to load the address of.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Load_LocalVar_Address,
    /*
        Converts the object on the top of
        the stack from one type to another,
        and performs overflow checks before
        doing the actual conversion.

        Arg1:
            The type to convert from.
            Of the type ConversionArgumentType.

        Arg2:
            The type to convert to.
            Of the type ConversionArgumentType.

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Convert_OverflowCheck,
    /*
        Converts the object on the top of
        the stack from one type to another.

        Arg1:
            The type to convert from.
            Of the type ConversionArgumentType.

        Arg2:
            The type to convert to.
            Of the type ConversionArgumentType.

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Convert_Unchecked,
    /*
        Loads the value in the specified 
        argument to the top of the stack.

        Arg1:
            The index of the parameter to
            load to the top of the stack.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Load_Parameter,
    /*
        Loads the address of the specified
        argument to the top of the stack.

        Arg1:
            The index of the parameter who's
			address is going to be loaded to
            load to the top of the stack.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Load_Parameter_Address,
    /*
        Stores the value on the top of the
		stack in the specified parameter.

        Arg1:
            The index of the parameter to 
			store the value into.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Store_Parameter,
    /*
        Loads the element in the array almost
		on the top of the stack, at the index
		on the top of the stack, to the top of
		the stack.

        Arg1:
            The type of object to load.
			Of the type ElementType.

        Arg2:
            The type of object that represents
			the index at which to get the element
			from.
			Of the type StackObjectType.

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Load_Element,
    /*
        Stores the element on the top of the 
		stack, in the array about to be almost
		on the top of the stack, at the index
		almost on the top of the stack.

        Arg1:
            The type of object to store.
			Of the type ElementType.

        Arg2:
            The type of object that represents
			the index at which to set the element.
			Of the type StackObjectType.

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Store_Element,
    /*
        Pushes the length of the array on the top
		of the stack, to the top of the stack.

        Arg1:
            N/A

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Load_Array_Length,
    /*
        Pops an object off of the top of the stack.

        Arg1:
            The type of object to pop.
			Of the type ElementType.

        Arg2:
            The IRType of the structure
			to pop. Only valid if Arg1 is
			ElementType_DataType.
			Of the type IRType.

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Pop,
    /*
        Shifts the object almost on the top of the stack
        in the specified way, by an amount specified
        by the value at the top of the stack.

        Arg1:
            The type of shift to perform.
            Of the type ShiftType.

        Arg2:
            The type of object to shift.
			Of the type StackObjectType.

        Arg3:
            The type of object to shift by.
			Of the type StackObjectType.

        Arg4:
            N/A

     */
    IROpCode_Shift,
    /*
        Adds the 2 items on the top of the stack,
		and pushes the result to the top of the stack.

        Arg1:
            The type of overflow checks to perform.
			Of the type OverflowType.

        Arg2:
            The type of the first argument.
			Of the type ElementType.

        Arg3:
            The type of the second argument.
			Of the type ElementType.

        Arg4:
            N/A

     */
    IROpCode_Add,
    /*
        Subtracts the 2 items on the top of the stack,
		and pushes the result to the top of the stack.

        Arg1:
            The type of overflow checks to perform.
			Of the type OverflowType.

        Arg2:
            The type of the first argument.
			Of the type ElementType.

        Arg3:
            The type of the second argument.
			Of the type ElementType.

        Arg4:
            N/A

     */
    IROpCode_Sub,
    /*
        Multiplies the 2 items on the top of the stack,
		and pushes the result to the top of the stack.

        Arg1:
            The type of overflow checks to perform.
			Of the type OverflowType.

        Arg2:
            The type of the first argument.
			Of the type ElementType.

        Arg3:
            The type of the second argument.
			Of the type ElementType.

        Arg4:
            N/A

     */
    IROpCode_Mul,
    /*
        Divides the 2 items on the top of the stack,
		and pushes the result to the top of the stack.

        Arg1:
            The type of overflow checks to perform.
			Of the type OverflowType.

        Arg2:
            The type of the first argument.
			Of the type ElementType.

        Arg3:
            The type of the second argument.
			Of the type ElementType.

        Arg4:
            N/A

     */
    IROpCode_Div,
    /*
        Divides the 2 items on the top of the stack,
		and pushes the remainder from this operation
		to the top of the stack.

        Arg1:
            The type of overflow checks to perform.
			Of the type OverflowType.

        Arg2:
            The type of the first argument.
			Of the type ElementType.

        Arg3:
            The type of the second argument.
			Of the type ElementType.

        Arg4:
            N/A

     */
    IROpCode_Rem,
    /*
        Loads the value at the address on the top
		of the stack, to the top of the stack.

        Arg1:
            The type of object to load.
			Of the type ElementType.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_LoadIndirect,
    /*
        Stores the value on the top of
		the stack to the address almost
		on the top of the stack.

        Arg1:
            The type of object to store.
			Of the type ElementType.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_StoreIndirect,
    /*
        Calls the specified method.

        Arg1:
            The method to call.
			Of the type IRMethodSpec.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Call,
    /*
        Loads a null reference to the
		top of the stack.

        Arg1:
            N/A

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_LoadNull,
    /*
        Creates a new object, initializes it's
		field, and calls the constructor in 
		Arg1.

        Arg1:
            The constructor to call.
			Of the type IRMethod.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_NewObj,
    /*
        Pushes a duplicate of the object
		on the top of the stack, to the 
		top of the stack.

        Arg1:
            The type of object on
			the top of the stack.
			Of the type ElementType.

        Arg2:
            N/A

        Arg3:
            N/A

        Arg4:
            N/A

     */
    IROpCode_Dup,
    /*
        Computes the result of 'And'ing the 2 items on the 
		top of the stack, and pushes the result to 
		the top of the stack.
		
        Arg1:
            The type of the first argument.
			Of the type ElementType.

        Arg2:
            The type of the second argument.
			Of the type ElementType.

		Arg3:
			N/A

        Arg4:
            N/A

     */
    IROpCode_And,
    /*
        Computes the result of 'Or'ing the 2 items on the 
		top of the stack, and pushes the result to 
		the top of the stack.
		
        Arg1:
            The type of the first argument.
			Of the type ElementType.

        Arg2:
            The type of the second argument.
			Of the type ElementType.

		Arg3:
			N/A

        Arg4:
            N/A

     */
    IROpCode_Or,
    /*
        Computes the result of 'XOr'ing the 2 items on the 
		top of the stack, and pushes the result to 
		the top of the stack.
		
        Arg1:
            The type of the first argument.
			Of the type ElementType.

        Arg2:
            The type of the second argument.
			Of the type ElementType.

		Arg3:
			N/A

        Arg4:
            N/A

     */
    IROpCode_XOr,
    /*
        Computes the 'Neg' value of the item on the
		top of the stack, and pushes the result to 
		the top of the stack.
		
        Arg1:
            The type of the only argument.
			Of the type ElementType.

        Arg2:
            N/A

		Arg3:
			N/A

        Arg4:
            N/A

     */
    IROpCode_Neg,
    /*
        Computes the 'Not' value of the item on the
		top of the stack, and pushes the result to 
		the top of the stack.
		
        Arg1:
            The type of the only argument.
			Of the type ElementType.

        Arg2:
            N/A

		Arg3:
			N/A

        Arg4:
            N/A

     */
    IROpCode_Not,
	
} IROpCode;


typedef enum BranchCondition
{
    BranchCondition_Always,
    BranchCondition_Equal,
    BranchCondition_False,
    BranchCondition_Greater_Or_Equal,
    BranchCondition_Greater_Or_Equal_Unsigned,
    BranchCondition_Greater,
    BranchCondition_Greater_Unsigned,
    BranchCondition_Less_Or_Equal,
    BranchCondition_Less_Or_Equal_Unsigned,
    BranchCondition_Less,
    BranchCondition_Less_Unsigned,
    BranchCondition_NotEqual_Unsigned,
    BranchCondition_True,

} BranchCondition;

typedef enum ConversionArgumentType
{
    ConversionArgumentType_I1,
    ConversionArgumentType_U1,
    ConversionArgumentType_I2,
    ConversionArgumentType_U2,
    ConversionArgumentType_I4,
    ConversionArgumentType_U4,
    ConversionArgumentType_I8,
    ConversionArgumentType_U8,
    ConversionArgumentType_I,
    ConversionArgumentType_U,
    ConversionArgumentType_R4,
    ConversionArgumentType_R8,

} ConversionArgumentType;

typedef enum ShiftType
{
    ShiftType_Right,
    ShiftType_Right_Sign_Extended,
    ShiftType_Left,
} ShiftType;

typedef enum ElementType
{
	ElementType_I,
	ElementType_I1,
	ElementType_I2,
	ElementType_I4,
	ElementType_I8,
	ElementType_U,
	ElementType_U1,
	ElementType_U2,
	ElementType_U4,
	ElementType_U8,
	ElementType_R4,
	ElementType_R8,
	ElementType_Ref,
	ElementType_DataType,
	ElementType_ManagedPointer,
	ElementType_Generic,
	ElementType_MethodGeneric,
} ElementType;

typedef enum OverflowType
{
	OverflowType_None,
	OverflowType_Signed,
	OverflowType_Unsigned,
} OverflowType;
