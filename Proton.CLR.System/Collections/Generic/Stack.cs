using System;

namespace System.Collections.Generic
{
	public class Stack<T> : IEnumerable<T>, ICollection, IEnumerable
	{
		private T[] mArray;
		private int mSize;
		private int mVersion;

		private const int INITIAL_SIZE = 16;

		public Stack() { }

		public Stack(int count)
		{
			if (count < 0) throw new ArgumentOutOfRangeException("count");
			mArray = new T[count];
		}

		public Stack(IEnumerable<T> collection)
		{
			if (collection == null) throw new ArgumentNullException("collection");
			ICollection<T> col = collection as ICollection<T>;
			if (col != null)
			{
				mSize = col.Count;
				mArray = new T[mSize];
				col.CopyTo(mArray, 0);
			}
			else foreach (T t in collection) Push(t);
		}

		public void Clear()
		{
			if (mArray != null) Array.Clear(mArray, 0, mArray.Length);
			mSize = 0;
			mVersion++;
		}

		public bool Contains(T t) { return mArray != null && Array.IndexOf(mArray, t, 0, mSize) != -1; }

		public void CopyTo(T[] dest, int idx)
		{
			if (mArray != null)
			{
				Array.Copy(mArray, 0, dest, idx, mSize);
				Array.Reverse(dest, idx, mSize);
			}
		}

		public T Peek()
		{
			if (mSize == 0) throw new InvalidOperationException();
			return mArray[mSize - 1];
		}

		public T Pop()
		{
			if (mSize == 0) throw new InvalidOperationException();
			mVersion++;
			T popped = mArray[--mSize];
			mArray[mSize] = default(T);
			return popped;
		}

		public void Push(T t)
		{
			if (mSize == 0 || mSize == mArray.Length) Array.Resize<T>(ref mArray, mSize == 0 ? INITIAL_SIZE : 2 * mSize);
			mVersion++;
			mArray[mSize++] = t;
		}

		public T[] ToArray()
		{
			T[] copy = new T[mSize];
			CopyTo(copy, 0);
			return copy;
		}

		public void TrimExcess()
		{
			if (mArray != null && (mSize < mArray.Length * 0.9)) Array.Resize<T>(ref mArray, mSize);
			mVersion++;
		}

		public int Count { get { return mSize; } }

		bool ICollection.IsSynchronized { get { return false; } }

		object ICollection.SyncRoot { get { return this; } }

		void ICollection.CopyTo(Array dest, int idx)
		{
			try
			{
				if (mArray != null)
				{
					mArray.CopyTo(dest, idx);
					Array.Reverse(dest, idx, mSize);
				}
			}
			catch (ArrayTypeMismatchException) { throw new ArgumentException(); }
		}

		public Enumerator GetEnumerator() { return new Enumerator(this); }

		IEnumerator<T> IEnumerable<T>.GetEnumerator() { return GetEnumerator(); }

		IEnumerator IEnumerable.GetEnumerator() { return GetEnumerator(); }

		public struct Enumerator : IEnumerator<T>, IEnumerator, IDisposable
		{
			const int NOT_STARTED = -2;
			const int FINISHED = -1;

			private Stack<T> mParent;
			private int mIndex;
			private int mVersion;

			internal Enumerator(Stack<T> t)
			{
				mParent = t;
				mIndex = NOT_STARTED;
				mVersion = t.mVersion;
			}

			public void Dispose() { mIndex = NOT_STARTED; }

			public bool MoveNext()
			{
				if (mVersion != mParent.mVersion) throw new InvalidOperationException();
				if (mIndex == NOT_STARTED) mIndex = mParent.mSize;
				return mIndex != FINISHED && --mIndex != FINISHED;
			}

			public T Current
			{
				get
				{
					if (mIndex < 0) throw new InvalidOperationException();
					return mParent.mArray[mIndex];
				}
			}

			void IEnumerator.Reset()
			{
				if (mVersion != mParent.mVersion) throw new InvalidOperationException();
				mIndex = NOT_STARTED;
			}

			object IEnumerator.Current { get { return Current; } }
		}
	}
}
