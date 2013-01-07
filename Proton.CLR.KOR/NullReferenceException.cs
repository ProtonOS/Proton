namespace System
{
	public class NullReferenceException : SystemException
	{
		public NullReferenceException() : base("A null value was found where an object instance was required.") { }

		public NullReferenceException(string msg) : base(msg) { }
	}
}
