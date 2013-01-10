namespace System
{
	public class UnauthorizedAccessException : SystemException
	{
		const int Result = unchecked((int)0x80131500);

		public UnauthorizedAccessException() : base("Access to the requested resource is not authorized.")
		{
			HResult = Result;
		}

		public UnauthorizedAccessException(string message) : base(message)
		{
			HResult = Result;
		}

		public UnauthorizedAccessException(string message, Exception inner) : base(message, inner)
		{
			HResult = Result;
		}
	}
}
