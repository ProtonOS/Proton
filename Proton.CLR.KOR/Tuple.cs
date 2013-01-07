namespace System
{
	public class Tuple<T1, T2>
	{
		T1 mItem1;
		T2 mItem2;

		public Tuple(T1 item1, T2 item2)
		{
			mItem1 = item1;
			mItem2 = item2;
		}

		public T1 Item1 { get { return mItem1; } }
		public T2 Item2 { get { return mItem2; } }

		public override int GetHashCode()
		{
			int hash = 0;
			if (!Object.Equals(mItem1, null)) hash ^= mItem1.GetHashCode();
			if (!Object.Equals(mItem2, null)) hash ^= mItem2.GetHashCode();
			return hash;
		}
	}
}
