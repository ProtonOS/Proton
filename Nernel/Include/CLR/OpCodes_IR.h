#pragma once

typedef const enum IROpCode
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

} IROpCode;