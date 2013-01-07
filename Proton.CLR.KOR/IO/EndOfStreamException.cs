namespace System.IO
{
	public class EndOfStreamException : IOException
	{
		public EndOfStreamException() : base("Failed to read past end of stream.") { }

		public EndOfStreamException(string message) : base(message, null) { }

		public EndOfStreamException(string message, Exception innerException) : base(message, innerException) { }
	}
}
