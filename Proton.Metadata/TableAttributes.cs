using System;

namespace Proton.Metadata
{
    public enum TypeAttributes : uint
    {
		None = 0,

        VisibilityMask = 0x00000007,
        NotPublic = 0x00000000,
        Public = 0x00000001,
        NestedPublic = 0x00000002,
        NestedPrivate = 0x00000003,
        NestedFamily = 0x00000004,
        NestedAssembly = 0x00000005,
        NestedFamilyAndAssembly = 0x00000006,
        NestedFamilyOrAssembly = 0x00000007,
        LayoutMask = 0x00000018,
        //AutoLayout										= 0x00000000,
        SequentialLayout = 0x00000008,
        ExplicitLayout = 0x00000010,
        ClassSemanticsMask = 0x00000020,
        //Class											= 0x00000000,
        Interface = 0x00000020,
        Abstract = 0x00000080,
        Sealed = 0x00000100,
        SpecialName = 0x00000400,
        Import = 0x00001000,
        Serializable = 0x00002000,
        StringFormatMask = 0x00030000,
        //AnsiClass										= 0x00000000,
        UnicodeClass = 0x00010000,
        AutoClass = 0x00020000,
        CustomFormatClass = 0x00030000,
        CustomStringFormatMask = 0x00C00000,
        BeforeFieldInit = 0x00100000,
        RTSpecialName = 0x00000800,
        HasSecurity = 0x00040000,
        IsTypeForwarder = 0x00200000,
    }

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

        CodeTypeMask = 0x0003,
        //IL										= 0x0000,
        Native = 0x0001,
        OPTIL = 0x0002,
        Runtime = 0x0003,
        ManagedMask = 0x0004,
        Unmanaged = 0x0004,
        //Managed									= 0x0000,
        ForwardReference = 0x0010,
        PreserveSignature = 0x0080,
        InternalCall = 0x1000,
        Synchronized = 0x0020,
        NoInlining = 0x0008,
        MaxMethodImplementationValue = 0xFFFF,
        NoOptimization = 0x0040,
    }
}
