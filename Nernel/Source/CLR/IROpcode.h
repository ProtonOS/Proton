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
	/*
		Description:
			Bitwise ANDs two values.
		
		Stack:
			..., value1, value2 -> ..., result

		Arg1: IRType*
			A pointer to the IRType of the first value.

		Arg2: IRType*
			A pointer to the IRType of the second value.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_And,
	/*
		Description:
			Bitwise ORs two values.
		
		Stack:
			..., value1, value2 -> ..., result

		Arg1: IRType*
			A pointer to the IRType of the first value.

		Arg2: IRType*
			A pointer to the IRType of the second value.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Or,
	/*
		Description:
			Bitwise XORs two values.
		
		Stack:
			..., value1, value2 -> ..., result

		Arg1: IRType*
			A pointer to the IRType of the first value.

		Arg2: IRType*
			A pointer to the IRType of the second value.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Xor,
	/*
		Description:
			Negate value, producing twos-compliment result.
		
		Stack:
			..., value -> ..., result

		Arg1: IRType*
			A pointer to the IRType of the value.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Neg,
	/*
		Description:
			Not value, producing ones-compliment result.
		
		Stack:
			..., value -> ..., result

		Arg1: IRType*
			A pointer to the IRType of the value.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Not,
	/*
		Description:
			Shift a value the specified direction by the specified amount.
		
		Stack:
			..., value, amount -> ..., result

		Arg1: ShiftNumericOperation
			The type of the shift to handle.

		Arg2: IRType*
			A pointer to the general IRType of the amount.

		Arg3: IRType*
			A pointer to the general IRType of the value.

		Arg4:
			N/A

	 */
	IROpcode_Shift,
	/*
		Description:
			Convert a value the specified type.
		
		Stack:
			..., value -> ..., result

		Arg1: IRType*
			A pointer to the IRType of the value.

		Arg2: IRType*
			A pointer to the IRType of the result.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Convert_Unchecked,

	/*
		Description:
			Casts an object to the specified type, failure results in exception.
		
		Stack:
			..., object -> ..., object

		Arg1: IRType*
			A pointer to the IRType of the type being casted to.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_CastClass,
	/*
		Description:
			Casts an object to the specified type, failure results in null.
		
		Stack:
			..., object -> ..., object

		Arg1: IRType*
			A pointer to the IRType of the type being casted to.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_IsInst,
	/*
		Description:
			Unboxes an object, producing the address.
		
		Stack:
			..., object -> ..., address

		Arg1: IRType*
			A pointer to the IRType of the object being unboxed.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Unbox,
	/*
		Description:
			Unboxes an object, producing the value.
		
		Stack:
			..., object -> ..., value

		Arg1: IRType*
			A pointer to the IRType of the object being unboxed.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Unbox_Any,
	/*
		Description:
			Boxes a value into an object.
		
		Stack:
			..., value -> ..., object

		Arg1: IRType*
			A pointer to the IRType of the value being boxed.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Box,
	/*
		Description:
			Throws an exception.
		
		Stack:
			..., exception -> ...

		Arg1:
			N/A

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Throw,
	/*
		Description:
			Loads an object from an address.
		
		Stack:
			..., address -> ..., object

		Arg1: IRType*
			A pointer to the IRType of the address.

		Arg2: IRType*
			A pointer to the IRType of the object.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Object,
	/*
		Description:
			Stores an object to an address.
		
		Stack:
			..., address, object -> ...

		Arg1: IRType*
			A pointer to the IRType of the address.

		Arg2: IRType*
			A pointer to the IRType of the object.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Store_Object,
	/*
		Description:
			Copies an object from one address to another.
		
		Stack:
			..., dest, src -> ...

		Arg1: IRType*
			A pointer to the IRType of the addresses.

		Arg2: IRType*
			A pointer to the IRType of the object.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Copy_Object,
	/*
		Description:
			Creates a new object and calls the specified constructor.
		
		Stack:
			..., arg1, ..., argN -> ..., object

		Arg1: IRMethod*
			A pointer to the IRMethod of the constructor.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_New_Object,
	/*
		Description:
			Loads the length of an array.
		
		Stack:
			..., array -> ..., length

		Arg1:
			N/A

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_ArrayLength,
	/*
		Description:
			Creates a new array with elements of the specified type.
		
		Stack:
			..., size -> ..., array

		Arg1: IRType*
			A pointer to the IRType of the elements.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_New_Array,
	/*
		Description:
			Checks if value is NaN or +/- infinite.
		
		Stack:
			..., value -> ..., value

		Arg1:
			N/A

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_CheckFinite,
	/*
		Description:
			Loads the specified field from the object.
		
		Stack:
			..., object -> ..., value

		Arg1: IRType*
			A pointer to the IRType of the object.

		Arg2: IRType*
			A pointer to the IRType of the field.

		Arg3: uint32_t
			The index to the field of the object.

		Arg4:
			N/A

	 */
	IROpcode_Load_Field,
	/*
		Description:
			Loads the specified field address from the object.
		
		Stack:
			..., object -> ..., address

		Arg1: IRType*
			A pointer to the IRType of the object.

		Arg2: IRType*
			A pointer to the IRType of the field.

		Arg3: uint32_t
			The index to the field of the object.

		Arg4:
			N/A

	 */
	IROpcode_Load_FieldAddress,
	/*
		Description:
			Stores a value to the specified field in the object.
		
		Stack:
			..., object, value -> ...

		Arg1: IRType*
			A pointer to the IRType of the object.

		Arg2: IRType*
			A pointer to the IRType of the field.

		Arg3: uint32_t
			The index to the field of the object.

		Arg4:
			N/A

	 */
	IROpcode_Store_Field,
	/*
		Description:
			Loads the specified static field.
		
		Stack:
			... -> ..., value

		Arg1: IRType*
			A pointer to the IRType containing the static field.

		Arg2: uint32_t
			The index to the static field.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_StaticField,
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

typedef enum BitwiseNumericOperation
{
	BitwiseNumericOperation_And,
	BitwiseNumericOperation_Or,
	BitwiseNumericOperation_Xor,
} BitwiseNumericOperation;

typedef enum UnaryNumericOperation
{
	UnaryNumericOperation_Neg,
	UnaryNumericOperation_Not,
} UnaryNumericOperation;

typedef enum ShiftNumericOperation
{
    ShiftNumericOperation_Right,
    ShiftNumericOperation_Right_Sign_Extended,
    ShiftNumericOperation_Left,
} ShiftNumericOperation;

typedef enum ElementType
{
	ElementType_I1,
	ElementType_U1,
	ElementType_I2,
	ElementType_U2,
	ElementType_I4,
	ElementType_U4,
	ElementType_I8,
	ElementType_U8,
	ElementType_I,
	ElementType_U,
	ElementType_R4,
	ElementType_R8,
} ElementType;
