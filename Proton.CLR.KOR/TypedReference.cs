namespace System
{
	public struct TypedReference
	{
		private RuntimeTypeHandle mType;
		private IntPtr mValue;

		public override bool Equals(object obj)
		{
			throw new NotSupportedException();
		}
		public override int GetHashCode()
		{
			if (mType.Value == IntPtr.Zero) return 0;
			return Type.GetTypeFromHandle(mType).GetHashCode();
		}
	}
}
