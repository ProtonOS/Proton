#pragma once

typedef enum IROpcode
{
	/*
		Description:
			Performs no operation.
		
		Stack:
			... -> ...

		Arg1:
			N/A

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Nop,
	/*
		Description:
			Stops execution and passes control off to the debugger.
		
		Stack:
			... -> ...

		Arg1:
			N/A

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Break,
	/*
		Description:
			Returns from the current method.
		
		Stack:
			returnValue -> ..., returnValue

		Arg1:
			N/A

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
    IROpcode_Return,
	/*
		Description:
			Loads the value from the parameter at the index in Arg1.
		
		Stack:
			... -> ..., value

		Arg1: uint32_t
			The index of the parameter to load.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Parameter,
	/*
		Description:
			Loads the address of the parameter at the index in Arg1.
		
		Stack:
			... -> ..., address

		Arg1: uint32_t
			The index of the parameter to load.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Parameter_Address,
	/*
		Description:
			Stores a value to the parameter at the index in Arg1.
		
		Stack:
			..., value -> ...

		Arg1: uint32_t
			The index of the parameter to store.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Store_Parameter,
	/*
		Description:
			Loads the value from the local variable at the index in Arg1.
		
		Stack:
			... -> ..., value

		Arg1: uint32_t
			The index of the local variable to load.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Local,
	/*
		Description:
			Loads the address of the local variable at the index in Arg1.
		
		Stack:
			... -> ..., address

		Arg1: uint32_t
			The index of the local variable to load.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Local_Address,
	/*
		Description:
			Stores a value to the local variable at the index in Arg1.
		
		Stack:
			..., value -> ...

		Arg1: uint32_t
			The index of the local variable to store.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Store_Local,
	/*
		Description:
			Loads a null reference.
		
		Stack:
			... -> ..., null

		Arg1:
			N/A

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Null,
	/*
		Description:
			Loads a string literal.
		
		Stack:
			... -> ..., string

		Arg1: uint32_t
			The length of the string.

		Arg2: uint8_t*
			A pointer to the string data.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_String,
	/*
		Description:
			Loads an int32 value.
		
		Stack:
			... -> ..., value

		Arg1: uint32_t
			The value to load.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Int32,
	/*
		Description:
			Loads an int64 value.
		
		Stack:
			... -> ..., value

		Arg1: uint64_t*
			A pointer to the value to load.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Int64,
	/*
		Description:
			Loads an float32 value.
		
		Stack:
			... -> ..., value

		Arg1: uint32_t
			The value to load.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Float32,
	/*
		Description:
			Loads an float64 value.
		
		Stack:
			... -> ..., value

		Arg1: uint64_t
			The value to load.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Float64,
	/*
		Description:
			Duplicates a value.
		
		Stack:
			..., value -> ..., value, value

		Arg1: IRType*
			A pointer to the IRType of the value.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Dup,
	/*
		Description:
			Pops a value.
		
		Stack:
			..., value -> ...

		Arg1: IRType*
			A pointer to the IRType of the value.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Pop,
	/*
		Description:
			Loads a value from an address.
		
		Stack:
			..., address -> ..., value

		Arg1: IRType*
			A pointer to the IRType of the value.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Indirect,
	/*
		Description:
			Stores a value to an address.
		
		Stack:
			..., address, value -> ...

		Arg1: IRType*
			A pointer to the IRType of the value.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Store_Indirect,
	/*
		Description:
			Adds two values together.
		
		Stack:
			..., value1, value2 -> ..., result

		Arg1: OverflowType
			The type of the overflow checks to perform.

		Arg2: IRType*
			A pointer to the IRType of the first value.

		Arg3: IRType*
			A pointer to the IRType of the second value.

		Arg4:
			N/A

	 */
	IROpcode_Add,
	/*
		Description:
			Subtracts two values.
		
		Stack:
			..., value1, value2 -> ..., result

		Arg1: OverflowType
			The type of the overflow checks to perform.

		Arg2: IRType*
			A pointer to the IRType of the first value.

		Arg3: IRType*
			A pointer to the IRType of the second value.

		Arg4:
			N/A

	 */
	IROpcode_Sub,
	/*
		Description:
			Multiplies two values together.
		
		Stack:
			..., value1, value2 -> ..., result

		Arg1: OverflowType
			The type of the overflow checks to perform.

		Arg2: IRType*
			A pointer to the IRType of the first value.

		Arg3: IRType*
			A pointer to the IRType of the second value.

		Arg4:
			N/A

	 */
	IROpcode_Mul,
	/*
		Description:
			Divides two values.
		
		Stack:
			..., value1, value2 -> ..., result

		Arg1: OverflowType
			The type of the overflow checks to perform.

		Arg2: IRType*
			A pointer to the IRType of the first value.

		Arg3: IRType*
			A pointer to the IRType of the second value.

		Arg4:
			N/A

	 */
	IROpcode_Div,
	/*
		Description:
			Divides two values, producing remainder.
		
		Stack:
			..., value1, value2 -> ..., result

		Arg1: OverflowType
			The type of the overflow checks to perform.

		Arg2: IRType*
			A pointer to the IRType of the first value.

		Arg3: IRType*
			A pointer to the IRType of the second value.

		Arg4:
			N/A

	 */
	IROpcode_Rem,
} IROpcode;

typedef enum OverflowType
{
	OverflowType_None,
	OverflowType_Signed,
	OverflowType_Unsigned,
} OverflowType;

typedef enum BinaryNumericOperation
{
	BinaryNumericOperation_Add,
	BinaryNumericOperation_Sub,
	BinaryNumericOperation_Div,
	BinaryNumericOperation_Mul,
	BinaryNumericOperation_Rem,
} BinaryNumericOperation;
