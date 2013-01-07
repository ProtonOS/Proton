using System;

namespace System.Collections.Generic
{
	public class Queue<T> : IEnumerable<T>, ICollection, IEnumerable
	{
		private T[] mArray;
		private int mHead;
		private int mTail;
		private int mSize;
		private int mVersion;

		private const int INITIAL_SIZE = 16;

		public Queue() { }

		public Queue(int count)
		{
			if (count < 0) throw new ArgumentOutOfRangeException("count");
			mArray = new T[count];
		}

		public Queue(IEnumerable<T> collection)
		{
			if (collection == null) throw new ArgumentNullException("collection");
			foreach (T t in collection) Enqueue(t);
		}

		public void Clear()
		{
			if (mArray != null) Array.Clear(mArray, 0, mArray.Length);
			mHead = mTail = mSize = 0;
			mVersion++;
		}

		public bool Contains(T item)
		{
			if (item == null)
			{
				foreach (T t in this) if (t == null) return true;
			}
			else
			{
				foreach (T t in this) if (item.Equals(t)) return true;
			}
			return false;
		}

		public void CopyTo(T[] array, int idx)
		{
			if (array == null) throw new ArgumentNullException();
			if ((uint)idx > (uint)array.Length) throw new ArgumentOutOfRangeException();
			if (array.Length - idx < mSize) throw new ArgumentOutOfRangeException();
			if (mSize == 0) return;
			int contents_length = mArray.Length;
			int length_from_head = contents_length - mHead;
			Array.Copy(mArray, mHead, array, idx, Math.Min(mSize, length_from_head));
			if (mSize > length_from_head) Array.Copy(mArray, 0, array, idx + length_from_head, mSize - length_from_head);
		}

		void ICollection.CopyTo(Array array, int idx)
		{
			if (array == null) throw new ArgumentNullException();
			if ((uint)idx < (uint)array.Length) throw new ArgumentOutOfRangeException();
			if (array.Length - idx < mSize) throw new ArgumentOutOfRangeException();
			if (mSize == 0) return;
			try
			{
				int contents_length = mArray.Length;
				int length_from_head = contents_length - mHead;
				Array.Copy(mArray, mHead, array, idx, Math.Min(mSize, length_from_head));
				if (mSize > length_from_head) Array.Copy(mArray, 0, array, idx + length_from_head, mSize - length_from_head);
			}
			catch (ArrayTypeMismatchException) { throw new ArgumentException(); }
		}

		public T Dequeue()
		{
			T ret = Peek();
			mArray[mHead] = default(T);
			if (++mHead == mArray.Length) mHead = 0;
			mSize--;
			mVersion++;
			return ret;
		}

		public T Peek()
		{
			if (mSize == 0) throw new InvalidOperationException();
			return mArray[mHead];
		}

		public void Enqueue(T item)
		{
			if (mArray == null || mSize == mArray.Length) SetCapacity(Math.Max(mSize * 2, 4));
			mArray[mTail] = item;
			if (++mTail == mArray.Length) mTail = 0;
			mSize++;
			mVersion++;
		}

		public T[] ToArray()
		{
			T[] t = new T[mSize];
			CopyTo(t, 0);
			return t;
		}

		public void TrimExcess() { if (mArray != null && (mSize < mArray.Length * 0.9)) SetCapacity(mSize); }

		void SetCapacity(int new_size)
		{
			if (mArray != null && new_size == mArray.Length) return;
			if (new_size < mSize) throw new InvalidOperationException("shouldnt happen");
			T[] new_data = new T[new_size];
			if (mSize > 0) CopyTo(new_data, 0);
			mArray = new_data;
			mTail = mSize;
			mHead = 0;
			mVersion++;
		}

		public int Count { get { return mSize; } }

		bool ICollection.IsSynchronized { get { return false; } }

		object ICollection.SyncRoot { get { return this; } }

		public Enumerator GetEnumerator() { return new Enumerator(this); }

		IEnumerator<T> IEnumerable<T>.GetEnumerator() { return GetEnumerator(); }

		IEnumerator IEnumerable.GetEnumerator() { return GetEnumerator(); }

		public struct Enumerator : IEnumerator<T>, IEnumerator, IDisposable
		{
			const int NOT_STARTED = -2;
			const int FINISHED = -1;

			private Queue<T> mParent;
			private int mIndex;
			private int mVersion;

			internal Enumerator(Queue<T> q)
			{
				mParent = q;
				mIndex = NOT_STARTED;
				mVersion = q.mVersion;
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
					return mParent.mArray[(mParent.mSize - 1 - mIndex + mParent.mHead) % mParent.mArray.Length];
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
