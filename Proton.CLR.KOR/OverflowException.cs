namespace System
{
    public class OverflowException : ArithmeticException
    {
        public OverflowException() : base("Number overflow.") { }

        public OverflowException(string message) : base(message) { }
    }
}
