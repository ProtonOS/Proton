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