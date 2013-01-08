namespace Proton.VM.IR
{
	public enum IROpcode : byte
	{
		/*
			Description:
				Perform no operation.
		
			Stack:
				... -> ...

			Arguments:
				bool						True to emit this Nop, False to prevent emitting.
		*/
		Nop,

		/*
			Description:
				Stop execution and pass control off to a debugger.
		
			Stack:
				... -> ...

			Arguments:
		*/
		Break,

		/*
			Description:
				Returns from the current method, with an optional return value.
				Callee and Caller do not share evaluation stacks, so the stack analysis
				is not entirely accurate.

			Stack:
				value (callee stack, present if returns non void) -> ... value (caller stack, present if returns non void)

			Arguments:
		*/
		Return,

		/*
			Description:
				Load the value from the parameter at the specified index.

			Stack:
				... -> ..., value

			Arguments:
				uint						The index of the parameter.
		*/
		LoadParameter,

		/*
			Description:
				Load the address of the parameter at the specified index.

			Stack:
				... -> ..., address

			Arguments:
				uint						The index of the parameter.
		*/
		LoadParameterAddress,

		/*
			Description:
				Store the value to the parameter at the specified index.

			Stack:
				..., value -> ...

			Arguments:
				uint						The index of the parameter.
		*/
		StoreParameter,

		/*
			Description:
				Load the value from the local at the specified index.

			Stack:
				... -> ..., value

			Arguments:
				uint						The index of the local.
		*/
		LoadLocal,

		/*
			Description:
				Load the address of the local at the specified index.

			Stack:
				... -> ..., address

			Arguments:
				uint						The index of the local.
		*/
		LoadLocalAddress,


		/*
			Description:
				Store the value to the local at the specified index.

			Stack:
				..., value -> ...

			Arguments:
				uint						The index of the local.
		*/
		StoreLocal,

		/*
			Description:
				Load the value from the field at the specified index in object.

			Stack:
				..., object -> ..., value

			Arguments:
				IRType					  The type of the object.
				uint						The index of the field.
		*/
		LoadField,

		/*
			Description:
				Load the address of the field at the specified index in object.

			Stack:
				..., object -> ..., address

			Arguments:
				IRType					  The type of the object.
				uint						The index of the field.
		*/
		LoadFieldAddress,


		/*
			Description:
				Store the value to the field at the specified index in object.

			Stack:
				..., object, value -> ...

			Arguments:
				IRType					  The type of the object.
				uint						The index of the field.
		*/
		StoreField,

		/*
			Description:
				Load the value from the static field.

			Stack:
				... -> ..., value

			Arguments:
				IRField					 The static field.
		*/
		LoadStaticField,

		/*
			Description:
				Load the address of the static field.

			Stack:
				... -> ..., address

			Arguments:
				IRField					 The static field.
		*/
		LoadStaticFieldAddress,


		/*
			Description:
				Store the value to the static field.

			Stack:
				..., value -> ...

			Arguments:
				IRField					 The static field.
		*/
		StoreStaticField,

		/*
			Description:
				Load the value from the address.

			Stack:
				..., address -> ..., value

			Arguments:
				IRType					  The type of the value.
		*/
		LoadIndirect,

		/*
			Description:
				Store the value to the address.

			Stack:
				..., address, value -> ...

			Arguments:
				IRType					  The type of the value.
		*/
		StoreIndirect,

		/*
			Description:
				Create a new object of the specified type, and call the specified constructor.

			Stack:
				..., arg0, ..., argN -> ..., object

			Arguments:
				IRType					  The type of the object.
				IRMethod					The constructor method.
		*/
		NewObject,

		/*
			Description:
				Load the object from the address.

			Stack:
				..., address -> ..., object

			Arguments:
				IRType					  The type of the object.
		*/
		LoadObject,

		/*
			Description:
				Store the object to the address.

			Stack:
				..., address, object -> ...

			Arguments:
				IRType					  The type of the object.
		*/
		StoreObject,

		/*
			Description:
				Initialize the value at an address to the default value.

			Stack:
				..., address -> ...

			Arguments:
				IRType					  The type of the value.
		*/
		InitializeObject,

		/*
			Description:
				Copy an object from one address to another.

			Stack:
				..., destination, source -> ...

			Arguments:
				IRType					  The type of the object.
		*/
		CopyObject,

		/*
			Description:
				Create a new array of the specified element type and length.

			Stack:
				..., length -> ..., array

			Arguments:
				IRType					  The type of the elements.
		*/
		NewArray,

		/*
			Description:
				Load the length of the array.

			Stack:
				..., array -> ..., length

			Arguments:
		*/
		LoadArrayLength,

		/*
			Description:
				Load the value of the element in the array at the specified index.

			Stack:
				..., array, index -> ..., value

			Arguments:
		*/
		LoadArrayElement,

		/*
			Description:
				Load the address of the element in the array at the specified index.

			Stack:
				..., array, index -> ..., address

			Arguments:
		*/
		LoadArrayElementAddress,

		/*
			Description:
				Store the value to the element in the array at the specified index.

			Stack:
				..., array, index, value -> ...

			Arguments:
		*/
		StoreArrayElement,

		/*
			Description:
				Initialize a specified length of bytes at the address to the value.

			Stack:
				..., destination, source, length -> ...

			Arguments:
		*/
		InitializeBlock,

		/*
			Description:
				Copy a specified length of bytes from one address to another.

			Stack:
				..., destination, source, length -> ...

			Arguments:
		*/
		CopyBlock,

		/*
			Description:
				Load a constant null object reference.

			Stack:
				... -> ..., null

			Arguments:
		*/
		LoadNull,

		/*
			Description:
				Load a literal unicode string.

			Stack:
				... -> ..., string

			Arguments:
				string					  The literal unicode string.
		*/
		LoadString,

		/*
			Description:
				Load a constant signed 32bit integer value.

			Stack:
				... -> ..., value

			Arguments:
				int						 The constant signed 32bit integer value.
		*/
		LoadInteger32,

		/*
			Description:
				Load a constant signed 64bit integer value.

			Stack:
				... -> ..., value

			Arguments:
				long						The constant signed 64bit integer value.
		*/
		LoadInteger64,

		/*
			Description:
				Load a constant signed 32bit real value.

			Stack:
				... -> ..., value

			Arguments:
				float					   The constant signed 32bit real value.
		*/
		LoadReal32,

		/*
			Description:
				Load a constant signed 64bit real value.

			Stack:
				... -> ..., value

			Arguments:
				double					  The constant signed 64bit real value.
		*/
		LoadReal64,

		/*
			Description:
				Load the size of the specified type.

			Stack:
				... -> ..., size

			Arguments:
				IRType					  The type.
		*/
		SizeOf,

		/*
			Description:
				Duplicate a value.

			Stack:
				..., value -> ..., value, value

			Arguments:
				IRType					  The type of the value.
		*/
		Duplicate,

		/*
			Description:
				Pop a value.

			Stack:
				..., value -> ...

			Arguments:
				IRType					  The type of the value.
		*/
		Pop,

		/*
			Description:
				Add two values.

			Stack:
				..., value1, value2 -> ..., result

			Arguments:
				OverflowType				The type of overflow checks to perform.
				ElementType				 The element type of value1.
				ElementType				 The element type of value2.
		*/
		Add,

		/*
			Description:
				Subtract two values.

			Stack:
				..., value1, value2 -> ..., result

			Arguments:
				OverflowType				The type of overflow checks to perform.
				ElementType				 The element type of value1.
				ElementType				 The element type of value2.
		*/
		Subtract,

		/*
			Description:
				Multiply two values.

			Stack:
				..., value1, value2 -> ..., result

			Arguments:
				OverflowType				The type of overflow checks to perform.
				ElementType				 The element type of value1.
				ElementType				 The element type of value2.
		*/
		Multiply,

		/*
			Description:
				Divide two values.

			Stack:
				..., dividend, divisor -> ..., result

			Arguments:
				OverflowType				The type of overflow checks to perform.
				ElementType				 The element type of dividend value.
				ElementType				 The element type of divisor value.
		*/
		Divide,

		/*
			Description:
				Divide two values, producing remainder.

			Stack:
				..., dividend, divisor -> ..., result

			Arguments:
				OverflowType				The type of overflow checks to perform.
				ElementType				 The element type of dividend value.
				ElementType				 The element type of divisor value.
		*/
		Remainder,

		/*
			Description:
				Bitwise AND two values.

			Stack:
				..., value1, value2 -> ..., result

			Arguments:
				ElementType				 The element type of value1.
				ElementType				 The element type of value2.
		*/
		And,

		/*
			Description:
				Bitwise OR two values.

			Stack:
				..., value1, value2 -> ..., result

			Arguments:
				ElementType				 The element type of value1.
				ElementType				 The element type of value2.
		*/
		Or,

		/*
			Description:
				Bitwise XOR two values.

			Stack:
				..., value1, value2 -> ..., result

			Arguments:
				ElementType				 The element type of value1.
				ElementType				 The element type of value2.
		*/
		Xor,

		/*
			Description:
				Negate value, producing twos-compliment result.

			Stack:
				..., value -> ..., result

			Arguments:
				ElementType				 The element type of value.
		*/
		Negate,

		/*
			Description:
				Not value, producing ones-compliment result.

			Stack:
				..., value -> ..., result

			Arguments:
				ElementType				 The element type of value.
		*/
		Not,

		/*
			Description:
				Shift a value the specified direction by a number of bits.

			Stack:
				..., value, bits -> ..., result

			Arguments:
				ShiftNumericOperation	   The type of shift operation to perform.
				ElementType				 The element type of value.
				ElementType				 The element type of bits.
		*/
		Shift,

		/*
			Description:
				Convert a value to the specified type, without overflow checks.

			Stack:
				..., value -> ..., result

			Arguments:
				ElementType				 The element type of value.
				ElementType				 The element type of result.
		*/
		ConvertUnchecked,

		/*
			Description:
				Convert a value to the specified type, with overflow checks.

			Stack:
				..., value -> ..., result

			Arguments:
				OverflowType				The type of overflow checks to perform.
				ElementType				 The element type of value.
				ElementType				 The element type of result.
		*/
		ConvertChecked,

		/*
			Description:
				Throw an exception if value is NaN, positive infinite, or negative infinite.

			Stack:
				..., value -> ..., value

			Arguments:
		*/
		CheckFinite,

		/*
			Description:
				Cast an object to the specified type.

			Stack:
				..., object -> ..., result

			Arguments:
				IRType					  The type of the result.
				bool						True to throw exception on failure, False for null result.
		*/
		Cast,

		/*
			Description:
				Box a value into an object.

			Stack:
				..., value -> ..., object

			Arguments:
				IRType					  The type of the boxed value.
		*/
		Box,

		/*
			Description:
				Unbox a value from an object, or the boxed address.

			Stack:
				..., object -> ..., value or address

			Arguments:
				IRType					  The type of the boxed value.
				bool						True to produce unboxed value, False to produce boxed address.
		*/
		Unbox,

		/*
			Description:
				Allocate aligned local stack space of the specified size.

			Stack:
				..., size -> ..., address

			Arguments:
		*/
		StackAllocate,

		/*
			Description:
				Throw an exception.

			Stack:
				..., exception -> ...

			Arguments:
		*/
		Throw,

		/*
			Description:
				Rethrow an exception.

			Stack:
				... -> ...

			Arguments:
		*/
		Rethrow,

		/*
			Description:
				Jump execution immediately to the start of the specified method.

			Stack:
				... -> ...

			Arguments:
				IRMethod					The method.
		*/
		Jump,

		/*
			Description:
				Jumps execution to the specified instruction unconditionally, ensures calling of related finally blocks

			Stack:
				..., value1 (may not be present based on condition), value2 (may not be present based on condition) -> ...

			Arguments:
				uint/IRInstruction		  The ILOffset of the target, resolves to an IRInstruction.
		*/
		Leave,

		/*
			Description:
				Call the specified method of the specified type, without overload checking.

			Stack:
				..., arg0, ..., argN -> ..., value (present if returning non void)

			Arguments:
				MetadataToken			   The method to call on the type.
				bool						True for virtual callsite based on the actual type, False for callsite based on token.
		*/
		Call,

		/*
			Description:
				Branch based on the specified condition to the specified instruction.

			Stack:
				..., value1 (may not be present based on condition), value2 (may not be present based on condition) -> ...

			Arguments:
				BranchCondition			 The condition of the branch.
				uint/IRInstruction		  The ILOffset of the target, resolves to an IRInstruction.
				ElementType				 The element type of value1 if present.
				ElementType				 The element type of value2 if present.
		*/
		Branch,

		/*
			Description:
				Switch based on the value to jump to the appropriate target.

			Stack:
				..., value -> ...

			Arguments:
				uint[]/IRInstruction[]	  The ILOffsets of the targets, resolves to an IRInstruction array.
		*/
		Switch,

		/*
			Description:
				Load the address of the specified method, without overload checking.

			Stack:
				... -> ..., address

			Arguments:
				IRMethod					The method.
				bool						True for virtual callsite based on the actual type, False for callsite based on token.
		*/
		LoadFunction,

		/*
			Description:
				Compare two values based on the specified condition.

			Stack:
				..., value1, value2 -> ..., result

			Arguments:
				CompareCondition			The condition of the comparison.
				ElementType				 The element type of value1 if present.
				ElementType				 The element type of value2 if present.
		*/
		Compare,

		/*
			Description:
				Load a runtime handle based on the specified metadata.

			Stack:
				... -> ..., handle

			Arguments:
				IRType					  The type of the runtime handle.
				IRType|IRField|IRMethod	 The metadata.
		*/
		LoadRuntimeHandle,

		/*
			Description:
				Load a typed reference from an address with the specified type.

			Stack:
				..., address -> ..., typedRef

			Arguments:
				IRType					  The type.
		*/
		LoadTypedReference,

		/*
			Description:
				Load the address from a typed reference.

			Stack:
				..., typedRef -> ..., address

			Arguments:
				IRType					  The type.
		*/
		LoadTypedReferenceAddress,

		/*
			Description:
				Load the RuntimeTypeHandle from a typed reference.

			Stack:
				..., typedRef -> ..., handle

			Arguments:
		*/
		LoadTypedReferenceType,

		/*
			Description:
				Return from a finally clause of an exception handler block.

			Stack:
				... -> ...

			Arguments:
		*/
		EndFinally,

		/*
			Description:
				Load the RuntimeArgumentHandle, some crazy stuff here to do later.

			Stack:
				... -> ..., handle

			Arguments:
		*/
		ArgList,

		// Instruction Linearization
		/*
			Description:
				Copy the value from the source target to destination target.

			Stack:
				..., typedRef -> ..., IRType or address

			Arguments:
		*/
		Move,
	}
}
