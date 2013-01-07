namespace System.Collections.Generic
{
	public class HashSet<T> : ICollection<T>, IEnumerable<T>, IEnumerable
	{
		public struct Enumerator : IEnumerator<T>, IEnumerator, IDisposable
		{
			private IEnumerator<T> mEnumerator;

			internal Enumerator(HashSet<T> hashSet) { mEnumerator = hashSet.mDictionary.Keys.GetEnumerator(); }

			public T Current { get { return mEnumerator.Current; } }

			public void Dispose() { }

			object IEnumerator.Current { get { return mEnumerator.Current; } }

			public bool MoveNext() { return mEnumerator.MoveNext(); }

			public void Reset() { mEnumerator.Reset(); }
		}

		private Dictionary<T, object> mDictionary;

		public HashSet() { mDictionary = new Dictionary<T, object>(); }

		public HashSet(IEnumerable<T> collection) : this(collection, null) { }

		public HashSet(IEqualityComparer<T> comparer) { mDictionary = new Dictionary<T, object>(comparer); }

		public HashSet(IEnumerable<T> collection, IEqualityComparer<T> comparer)
		{
			if (collection == null) throw new ArgumentNullException();
			mDictionary = new Dictionary<T, object>(comparer);
			foreach (T item in collection) mDictionary[item] = null;
		}

		public bool Add(T item)
		{
			bool ret = !mDictionary.ContainsKey(item);
			mDictionary[item] = null;
			return ret;
		}

		public void Clear() { mDictionary.Clear(); }

		public bool Contains(T item) { return mDictionary.ContainsKey(item); }

		public void CopyTo(T[] array) { throw new NotImplementedException(); }

		public void CopyTo(T[] array, int arrayIndex) { throw new NotImplementedException(); }

		public void CopyTo(T[] array, int arrayIndex, int count) { throw new NotImplementedException(); }

		public Enumerator GetEnumerator() { return new Enumerator(this); }

		public bool Remove(T item) { return mDictionary.Remove(item); }

		public IEqualityComparer<T> Comparer { get { return mDictionary.Comparer; } }

		public int Count { get { return mDictionary.Count; } }

		void ICollection<T>.Add(T item) { Add(item); }

		bool ICollection<T>.IsReadOnly { get { return false; } }

		IEnumerator<T> IEnumerable<T>.GetEnumerator() { return new Enumerator(this); }

		IEnumerator IEnumerable.GetEnumerator() { return new Enumerator(this); }
	}
}
