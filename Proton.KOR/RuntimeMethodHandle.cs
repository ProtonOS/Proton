namespace System
{
	public struct RuntimeMethodHandle
	{
		private IntPtr mValue;

		internal RuntimeMethodHandle(IntPtr v) { mValue = v; }

		public IntPtr Value { get { return mValue; } }
	}
}
