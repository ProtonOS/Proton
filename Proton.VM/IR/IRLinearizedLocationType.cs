namespace Proton.VM.IR
{
	public enum IRLinearizedLocationType : byte
	{
		Null,
		Local,
		LocalAddress,
		Parameter,
		ParameterAddress,
		ConstantI4,
		ConstantI8,
		ConstantR4,
		ConstantR8,
		Field,
		FieldAddress,
		StaticField,
		StaticFieldAddress,
		Indirect,
		SizeOf,
		ArrayElement,
		ArrayElementAddress,
		ArrayLength,
		FunctionAddress,
		RuntimeHandle,
		String,
		Phi,
	}
}
