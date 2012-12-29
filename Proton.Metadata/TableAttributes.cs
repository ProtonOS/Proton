using System;

namespace Proton.Metadata
{
	public enum AssemblyHashAlgorithm : uint
	{
		None = 0x0000,
		MD5 = 0x8003,
		SHA1 = 0x8004
	}

	[Flags]
	public enum AssemblyFlags : uint
	{
		None = 0x0000,

		PublicKey = 0x0001,
		Retargetable = 0x0100,
		DisableJITCompileOptimizer = 0x4000,
		EnableJITCompileTracking = 0x8000
	}

	[Flags]
	public enum EventAttributes : ushort
	{
		None = 0x0000,
		SpecialName = 0x0200,
		RTSpecialName = 0x0400
	}

	[Flags]
	public enum FieldAttributes : ushort
	{
		None = 0,

		CompilerControlled = 0x0000,
		Private = 0x0001,
		FamilyAndAssembly = 0x0002,
		Assembly = 0x0003,
		Family = 0x0004,
		FamilyOrAssembly = 0x0005,
		Public = 0x0006,
		FieldAccessMask = 0x0007,

		Static = 0x0010,
		InitOnly = 0x0020,
		Literal = 0x0040,
		NotSerialized = 0x0080,
		SpecialName = 0x0200,

		PInvokeImplementation = 0x2000,

		RTSpecialName = 0x0400,
		HasFieldMarshal = 0x1000,
		HasDefault = 0x8000,
		HasFieldRVA = 0x0100,
	}

	[Flags]
	public enum FileAttributes : uint
	{
		None = 0x0000,

		ContainsMetadata = 0x0000,
		ContainsNoMetadata = 0x0001
	}

	[Flags]
	public enum GenericParamAttributes : ushort
	{
		None = 0x0000,
		Covariant = 0x0001,
		Contravariant = 0x0002,
		VarianceMask = 0x0003,

		ReferenceTypeConstraint = 0x0004,
		NotNullableValueTypeConstraint = 0x0008,
		DefaultConstructorConstraint = 0x0010,
		SpecialConstraintMask = 0x001C
	}

	[Flags]
	public enum PInvokeAttributes : ushort
	{
		None = 0x0000,

		NoMangle = 0x0001,

		CharSetNotSpecified = 0x0000,
		CharSetAnsi = 0x0002,
		CharSetUnicode = 0x0004,
		CharSetAuto = 0x0006,
		CharSetMask = 0x0006,

		SupportsLastError = 0x0040,

		CallConventionPlatformAPI = 0x0100,
		CallConventionCDecl = 0x0200,
		CallConventionSTDCall = 0x0300,
		CallConventionThisCall = 0x0400,
		CallConventionFastCall = 0x0500,
		CallConventionMask = 0x0700,
	}

	[Flags]
	public enum ManifestResourceAttributes : uint
	{
		None = 0x0000,

		Public = 0x0001,
		Private = 0x0002,
		VisibilityMask = 0x0007
	}

	[Flags]
	public enum MethodAttributes : ushort
	{
		None = 0x0000,

		CompilerControlled = 0x0000,
		Private = 0x0001,
		FamANDAssem = 0x0002,
		Assem = 0x0003,
		Family = 0x0004,
		FamORAssem = 0x0005,
		Public = 0x0006,
		MemberAccessMask = 0x0007,

		Static = 0x0010,
		Final = 0x0020,
		Virtual = 0x0040,
		HideBySig = 0x0080,
		VtableLayoutMask = 0x0100,

		ReuseSlot = 0x0000,
		NewSlot = 0x0100,

		Strict = 0x0200,
		Abstract = 0x0400,
		SpecialName = 0x0800,

		PInvokeImpl = 0x2000,
		UnmanagedExport = 0x0008,

		RTSpecialName = 0x1000,
		HasSecurity = 0x4000,
		RequireSecObject = 0x8000,
	}

	[Flags]
	public enum MethodImplAttributes : ushort
	{
		None = 0x0000,

		IL = 0x0000,
		Native = 0x0001,
		OPTIL = 0x0002,
		Runtime = 0x0003,
		CodeTypeMask = 0x0003,

		Managed = 0x0000,
		Unmanaged = 0x0004,
		ManagedMask = 0x0004,

		NoInlining = 0x0008,
		ForwardReference = 0x0010,
		Synchronized = 0x0020,
		NoOptimization = 0x0040,
		PreserveSignature = 0x0080,
		InternalCall = 0x1000,
		MaxMethodImplementationValue = 0xFFFF,
	}

	[Flags]
	public enum MethodSemanticsAttributes : ushort
	{
		None = 0x0000,

		Setter = 0x0001,
		Getter = 0x0002,
		Other = 0x0004,
		AddOn = 0x0008,
		RemoveOn = 0x0010,
		Fire = 0x0020
	}

	[Flags]
	public enum ParamAttributes : ushort
	{
		None = 0x0000,

		In = 0x0001,
		Out = 0x0002,
		Optional = 0x0010,
		HasDefault = 0x1000,
		HasFieldMarshal = 0x2000
	}

	[Flags]
	public enum PropertyAttributes : ushort
	{
		None = 0x0000,

		SpecialName = 0x0200,
		RTSpecialName = 0x0400,
		HasDefault = 0x1000
	}

	[Flags]
    public enum TypeAttributes : uint
    {
		None = 0,

        NotPublic = 0x00000000,
        Public = 0x00000001,
        NestedPublic = 0x00000002,
        NestedPrivate = 0x00000003,
        NestedFamily = 0x00000004,
        NestedAssembly = 0x00000005,
        NestedFamilyAndAssembly = 0x00000006,
        NestedFamilyOrAssembly = 0x00000007,
		VisibilityMask = 0x00000007,

        AutoLayout = 0x00000000,
        SequentialLayout = 0x00000008,
        ExplicitLayout = 0x00000010,
		LayoutMask = 0x00000018,

        Class = 0x00000000,
        Interface = 0x00000020,
		ClassSemanticsMask = 0x00000020,

		Abstract = 0x00000080,
        Sealed = 0x00000100,
        SpecialName = 0x00000400,

        Import = 0x00001000,
        Serializable = 0x00002000,

        AnsiClass = 0x00000000,
        UnicodeClass = 0x00010000,
        AutoClass = 0x00020000,
        CustomFormatClass = 0x00030000,
		StringFormatMask = 0x00030000,
		CustomStringFormatMask = 0x00C00000,

        BeforeFieldInit = 0x00100000,
        RTSpecialName = 0x00000800,
        HasSecurity = 0x00040000,
        IsTypeForwarder = 0x00200000,
    }
}
