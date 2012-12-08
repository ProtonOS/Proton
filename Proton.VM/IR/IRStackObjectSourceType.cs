namespace Proton.VM.IR
{
    public enum IRStackObjectSourceType : byte
    {
        Constant,
        Parameter,
        Local,
        PinnedLocal,
        StaticField,
        Stack,
    }
}
