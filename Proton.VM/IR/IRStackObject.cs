namespace Proton.VM.IR
{
    public sealed class IRStackObject
    {
        public IRType Type;
        public IRType BoxedType;
        public IRLinearizedLocation LinearizedTarget;
    }
}
