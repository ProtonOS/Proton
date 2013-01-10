namespace System
{
	public class ObjectDisposedException : InvalidOperationException
	{
		private string mObjectName;

		public ObjectDisposedException(string objectName) : base("The object was used after being disposed")
		{
			mObjectName = objectName;
		}

		public ObjectDisposedException(string objectName, string msg) : base(msg)
		{
			mObjectName = objectName;
		}

		public string ObjectName { get { return mObjectName; } }
	}
}
