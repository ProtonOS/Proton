namespace System
{
    public class FormatException : SystemException
    {
        public FormatException() : base("Invalid Format.") { }

        public FormatException(string message) : base(message) { }
    }
}
