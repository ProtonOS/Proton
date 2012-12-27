namespace System
{
	public struct RuntimeArgumentHandle
	{
		private IntPtr mValue;

		internal RuntimeArgumentHandle(IntPtr v) { mValue = v; }

		public IntPtr Value { get { return mValue; } }
	}
}
