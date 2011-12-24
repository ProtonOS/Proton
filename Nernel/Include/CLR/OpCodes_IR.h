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

     */
    IROpCode_BreakForDebugger,
    /*
        Returns from the current method,
        and doesn't return any real value.

        Arg1:
            N/A

        Arg2:
            N/A

        Arg3:
            N/A

     */
    IROpCode_Return_NoRetValue,
    /*
        Loads an int32 value onto the 
        top of the stack.

        Arg1:
            The value to load.

        Arg2:
            N/A

        Arg3:
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
            N/A

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

     */
    IROpCode_Load_Parameter,
    /*
        Loads the specified string to the
        top of the stack.

        Arg1:
            The string to load to the top
            of the stack. ( char* )

        Arg2:
            N/A

        Arg3:
            N/A

     */
    IROpCode_Load_String,
    /*
        Shifts the object almost on the top of the stack
        in the specified way, by an amount specified
        by the value at the top of the stack.

        Arg1:
            The type of shift to perform.
            Of the type ShiftType

        Arg2:
            N/A

        Arg3:
            N/A

     */
    IROpCode_Shift,
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