namespace System.Collections.Generic
{
    public class List<T> : IList<T>, ICollection<T>, IEnumerable<T>, IList, ICollection, IEnumerable
    {
        public struct Enumerator : IEnumerator<T>, IDisposable
        {
            private List<T> mList;
            private int mIndex;

            internal Enumerator(List<T> list)
            {
                mList = list;
                mIndex = -1;
            }

            public T Current { get { return mList[mIndex]; } }

            public void Dispose() { }

            object IEnumerator.Current { get { return mList[mIndex]; } }

            public bool MoveNext()
            {
                mIndex++;
                return (mIndex < mList.Count);
            }

            public void Reset() { mIndex = -1; }
        }

        private const int DefaultCapacity = 4;

        private T[] mItems;
        private int mCount;

        public List() : this(DefaultCapacity) { }

        public List(int capacity)
        {
            if (capacity < 0) throw new ArgumentOutOfRangeException("capacity");
            mItems = new T[capacity];
            mCount = 0;
        }

        public List(IEnumerable<T> collection)
        {
            ICollection<T> iCol = collection as ICollection<T>;
            if (iCol != null)
            {
                mCount = iCol.Count;
                mItems = new T[mCount];
                iCol.CopyTo(mItems, 0);
            }
            else
            {
                mItems = new T[DefaultCapacity];
                mCount = 0;
                foreach (T item in collection) Add(item);
            }
        }

        private void EnsureSpace(int space)
        {
            if (mCount + space > mItems.Length) Array.Resize<T>(ref mItems, Math.Max(mItems.Length << 1, 4));
        }

        private void Shift(int index, int count)
        {
            if (count > 0)
            {
                EnsureSpace(count);
                for (int i = mCount - 1; i >= index; i--) mItems[i + count] = mItems[i];
            }
            else
            {
                for (int i = index; i < mCount + count; i++) mItems[i] = mItems[i - count];
            }
            mCount += count;
        }

        public void Add(T item)
        {
            EnsureSpace(1);
            mItems[mCount++] = item;
        }

        public int Count { get { return mCount; } }

        public int Capacity { get { return mItems.Length; } set { throw new NotImplementedException(); } }

        public T this[int index]
        {
            get
            {
                if (index >= mCount || index < 0) throw new ArgumentOutOfRangeException("index");
                return mItems[index];
            }
            set
            {
                if (index >= mCount || index < 0) throw new ArgumentOutOfRangeException("index");
                mItems[index] = value;
            }
        }

        public Enumerator GetEnumerator() { return new Enumerator(this); }

        public int IndexOf(T item, int start, int count) { return Array.IndexOf<T>(mItems, item, start, count); }

        public int IndexOf(T item, int start) { return IndexOf(item, start, mCount - start); }

        public void InsertRange(int index, IEnumerable<T> collection)
        {
            if (collection == null) throw new ArgumentNullException("collection");
            if (index < 0 || index > mCount) throw new ArgumentOutOfRangeException("index");
            List<T> toInsert = new List<T>(collection);
            Shift(index, toInsert.Count);
            for (int i = 0; i < toInsert.Count; i++) mItems[index + i] = toInsert[i];
        }

        public T[] ToArray()
        {
            T[] array = new T[mCount];
            Array.Copy(mItems, array, mCount);
            return array;
        }

        public int IndexOf(T item) { return IndexOf(item, 0, mCount); }

        public void Insert(int index, T item)
        {
            if (index < 0 || index > mCount) throw new ArgumentOutOfRangeException("index");
            Shift(index, 1);
            mItems[index] = item;
        }

        public void RemoveAt(int index) { Shift(index, -1); }

        public bool IsReadOnly { get { return false; } }

        public void Clear()
        {
            Array.Clear(mItems, 0, mItems.Length);
            mCount = 0;
        }

        public bool Contains(T item) { return Array.IndexOf(mItems, item) >= 0; }

        public void CopyTo(T[] array, int arrayIndex) { Array.Copy(mItems, 0, (Array)array, arrayIndex, mCount); }

        public bool Remove(T item)
        {
            int idx = Array.IndexOf(mItems, item);
            if (idx >= 0)
            {
                RemoveAt(idx);
                return true;
            }
            return false;
        }

        IEnumerator<T> IEnumerable<T>.GetEnumerator() { return new Enumerator(this); }

        IEnumerator IEnumerable.GetEnumerator() { return new Enumerator(this); }

        public bool IsFixedSize { get { return false; } }

        object IList.this[int index] { get { return this[index]; } set { this[index] = (T)value; } }

        public int Add(object value)
        {
            Add((T)value);
            return mItems.Length - 1;
        }

        public bool Contains(object value) { return Contains((T)value); }

        public int IndexOf(object value) { return IndexOf((T)value); }

        public void Insert(int index, object value) { Insert(index, (T)value); }

        public void Remove(object value) { Remove((T)value); }

        public bool IsSynchronized { get { return false; } }

        public object SyncRoot { get { return this; } }

        public void CopyTo(Array array, int index) { Array.Copy(mItems, 0, array, index, mCount); }
    }
}
