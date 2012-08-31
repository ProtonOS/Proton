#pragma once

typedef enum IROpcode
{
	/*
		Description:
			Performs no operation.
		
		Stack:
			... -> ...

		Arg1: bool_t
			A value to determine if this nop should be emitted.

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
			Returns from the current method, with an optional return value.
			Due to implementation details, the callee and caller do not share
			evaluation stacks, and as a result this stack analysis is not
			entirely concise.
		
		Stack:
			value (present for return values, callee stack) -> ..., value (present for return values, caller stack)

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
	IROpcode_Load_ParameterAddress,
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
	IROpcode_Load_LocalAddress,
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
			The length of the string in characters.

		Arg2: uint8_t*
			A pointer to the unicode character data.

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

		Source1:
			Value to duplicate.

		Source2:
			N/A

		Source3:
			This is actually used as
			a second destination.

		Destination:
			The location to store the
			first copy of the value.

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

		Arg2: ElementType
			The type of the first value.

		Arg3: ElementType
			The type of the second value.

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

		Arg2: ElementType
			The type of the first value.

		Arg3: ElementType
			The type of the second value.

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

		Arg2: ElementType
			The type of the first value.

		Arg3: ElementType
			The type of the second value.

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

		Arg2: ElementType
			The type of the first value.

		Arg3: ElementType
			The type of the second value.

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

		Arg2: ElementType
			The type of the first value.

		Arg3: ElementType
			The type of the second value.

		Arg4:
			N/A

	 */
	IROpcode_Rem,
	/*
		Description:
			Bitwise ANDs two values.
		
		Stack:
			..., value1, value2 -> ..., result

		Arg1: ElementType
			The type of the first value.

		Arg2: ElementType
			The type of the second value.

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

		Arg1: ElementType
			The type of the first value.

		Arg2: ElementType
			The type of the second value.

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

		Arg1: ElementType
			The type of the first value.

		Arg2: ElementType
			The type of the second value.

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

		Arg1: ElementType
			The type of the value.

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

		Arg1: ElementType
			The type of the value.

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

		Arg2: ElementType
			The type of the amount being shifted by.

		Arg3: ElementType
			The type of the value.

		Arg4:
			N/A

	 */
	IROpcode_Shift,
	/*
		Description:
			Convert a value the specified type.
		
		Stack:
			..., value -> ..., result

		Arg1: ElementType
			The type of the value.

		Arg2: ElementType
			The type of the result.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Convert_Unchecked,
	/*
		Description:
			Convert a value the specified type, with overflow checking.
		
		Stack:
			..., value -> ..., result

		Arg1: ElementType
			The type of the value.

		Arg2: ElementType
			The type of the result.

		Arg3: OverflowType
			The type over overflow checks to handle.

		Arg4:
			N/A

	 */
	IROpcode_Convert_Checked,
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
	IROpcode_UnboxAny,
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
			..., destination, source -> ...

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

		Arg2: IRType*
			A pointer to the type of object to create.

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
			Loads the specified field from an object.
		
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
			Loads the specified field address from an object.
		
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
			Stores a value to the specified field in an object.
		
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
			Loads the specified static field value.
		
		Stack:
			... -> ..., value

		Arg1: IRField*
			A pointer to the IRField of the static field.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_StaticField,
	/*
		Description:
			Loads the specified static field address.
		
		Stack:
			... -> ..., address

		Arg1: IRField*
			A pointer to the IRField of the static field.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_StaticFieldAddress,
	/*
		Description:
			Stores a value to the specified static field.
		
		Stack:
			..., value -> ...

		Arg1: IRField*
			A pointer to the IRField of the static field.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Store_StaticField,
	/*
		Description:
			Loads the value of an element at an index within an array.
		
		Stack:
			..., array, index -> ..., value

		Arg1: IRType*
			A pointer to the IRType of the array.

		Arg2: IRType*
			A pointer to the IRType of the element.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Element,
	/*
		Description:
			Loads the address of an element at an index within an array.
		
		Stack:
			..., array, index -> ..., address

		Arg1: IRType*
			A pointer to the IRType of the array.

		Arg2: IRType*
			A pointer to the IRType of the element.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_ElementAddress,
	/*
		Description:
			Stores a value into an element at an index within an array.
		
		Stack:
			..., array, index, value -> ...

		Arg1: IRType*
			A pointer to the IRType of the array.

		Arg2: IRType*
			A pointer to the IRType of the element.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Store_Element,
	/*
		Description:
			Allocates an aligned chunk of memory within local space,
			of a specified size and loads the address.
		
		Stack:
			..., size -> ..., address

		Arg1: IRType*
			A pointer to the IRType of the size.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Allocate_Local,
	/*
		Description:
			Initializes a value at an address.
		
		Stack:
			..., address -> ...

		Arg1: IRType*
			A pointer to the IRType of the address.

		Arg2: IRType*
			A pointer to the IRType of the value being initialized.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Initialize_Object,
	/*
		Description:
			Copies a specified number of bytes from source to destination.
		
		Stack:
			..., destination, source, size -> ...

		Arg1:
			N/A

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Copy_Block,
	/*
		Description:
			Sets a specified number of bytes at an address to a value.
		
		Stack:
			..., address, value, size -> ...

		Arg1:
			N/A

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Initialize_Block,
	/*
		Description:
			Loads the size of the specified type.
		
		Stack:
			... -> ..., size

		Arg1: IRType*
			A pointer to the IRType to get the size of.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_SizeOf,
	/*
		Description:
			Transfers immediate control to the specified method.
		
		Stack:
			... -> ...

		Arg1: IRMethod*
			A pointer to the IRMethod to jump to.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Jump,
	/*
		Description:
			Call a method, with overload checking.
		
		Stack:
			..., arg0, arg1, ..., argN -> ..., value (present for return values)

		Arg1: IRType*
			A pointer to the IRType that contains the method.

		Arg2: uint32_t
			The index to the IRMethod to be called.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Call_Virtual,
	/*
		Description:
			Call a method, with overload checking.
		
		Stack:
			..., arg0, arg1, ..., argN -> ..., value (present for return values)

		Arg1: IRType*
			A pointer to the IRType that contains the method.

		Arg2: uint32_t
			The index to the IRMethod to be called.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Call_Constrained,
	/*
		Description:
			Call a method, without overload checking.
		
		Stack:
			..., arg0, arg1, ..., argN -> ..., value (present for return values)

		Arg1: IRMethod*
			A pointer to the IRMethod to be called.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Call_Absolute,
	/*
		Description:
			Call an internal native method, without overload checking.
		
		Stack:
			..., arg0, arg1, ..., argN -> ..., value (present for return values)

		Arg1: IRMethod*
			A pointer to the IRMethod to be called.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Call_Internal,
    /*
        Substitutes an internal call with an intrinsic version based on
		the architecture.

		Stack:
			..., arg0, arg1, ..., argN -> ..., value (present for return values)

		Arg1: IntrinsicCallType
			The internal call to substitute.

        Arg2:
            N/A

        Arg3:
			N/A

        Arg4:
			N/A

     */
    IROpcode_Call_Intrinsic,
	/*
		Description:
			Performs a conditional branch to the specified target based on
			the specified condition.
		
		Stack:
			..., arg0 (may be present), arg1 (may be present) -> ...

		Arg1: BranchCondition
			The condition of the branch.

		Arg2: uint32_t/IRInstruction*
			The absolute location in memory to target of branch.
			Resolves to IRInstruction* directly.

		Arg3: ElementType
			The ElementType of arg0 if present.

		Arg4: ElementType
			The ElementType of arg1 if present.

	 */
	IROpcode_Branch,
	/*
		Description:
			Performs a table switch based on a value.
		
		Stack:
			..., value -> ...

		Arg1: uint32_t
			The number of jump targets.

		Arg2: IRInstruction*[]
			The array of jump target instructions.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Switch,
	/*
		Description:
			Loads a native method pointer from the specified method.
		
		Stack:
			... -> ..., pointer

		Arg1: IRMethod*
			A pointer to the IRMethod to load pointer from.

		Arg2:
			N/A

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_Function,
	/*
		Description:
			Loads a native method pointer from the specified method.
		
		Stack:
			..., object -> ..., pointer

		Arg1: IRType*
			A pointer to the IRType containing the method.

		Arg2: uint32_t
			The index to the IRMethod to be loaded.

		Arg3:
			N/A

		Arg4:
			N/A

	 */
	IROpcode_Load_VirtualFunction,
    /*
        Compares two values based on the specified condition.

		Stack:
			..., value1, value2 -> ..., result

        Arg1: CompareCondition
            The condition of the comparison.

        Arg2: ElementType
			The type of value1.

        Arg3: ElementType
			The type of value2.

        Arg4:
            N/A

     */
    IROpcode_Compare,
    /*
        Loads a token into a handle.

		Stack:
			... -> ..., handle

		Arg1: IRType*
			A pointer to the IRType of the handle.

        Arg2: RuntimeHandleType
            The type of metadata.

        Arg3: IRType* / IRField* / IRMethod*
			A pointer to the metadata based on the type.

        Arg4:
			N/A

     */
    IROpcode_Load_Token,
    /*
        Loads a TypedReference from a pointer.

		Stack:
			..., pointer -> ..., typedRef

		Arg1: IRType*
			A pointer to the IRType of the pointer.

        Arg2: IRType*
            A pointer to the IRType of the class being loaded.

        Arg3: uint32_t
			The original metadata token of the class being loaded.

        Arg4:
			N/A

     */
    IROpcode_MkRefAny,
    /*
        Loads a pointer from a TypedReference.

		Stack:
			..., typedRef -> ..., pointer

		Arg1: IRType*
			A pointer to the IRType of the class being loaded.

        Arg2:
            N/A

        Arg3:
			N/A

        Arg4:
			N/A

     */
    IROpcode_RefAnyVal,
    /*
        Loads the metadata token of the type from a TypedReference.

		Stack:
			..., typedRef -> ..., token

		Arg1:
			N/A

        Arg2:
            N/A

        Arg3:
			N/A

        Arg4:
			N/A

     */
    IROpcode_RefAnyType,
    /*
        Moves a value from a source to a destination.

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

		Source1:
			Value to move.

		Source2:
			N/A

		Source3:
			N/A

		Destination:
			The location to move into.

     */
    IROpcode_Move,
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

typedef enum CompareCondition
{
	CompareCondition_Equal,
	CompareCondition_Greater_Than,
	CompareCondition_Greater_Than_Unsigned,
	CompareCondition_Less_Than,
	CompareCondition_Less_Than_Unsigned,
} CompareCondition;

typedef enum RuntimeHandleType
{
	RuntimeHandleType_IRField,
	RuntimeHandleType_IRMethod,
	RuntimeHandleType_IRType,
} RuntimeHandleType;