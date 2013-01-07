namespace System
{
	public class ArrayTypeMismatchException : SystemException
	{
		public ArrayTypeMismatchException() : base("Source array type cannot be assigned to destination array type.") { }

		public ArrayTypeMismatchException(string message) : base(message) { }

		public ArrayTypeMismatchException(string message, Exception innerException) : base(message, innerException) { }
	}
}
