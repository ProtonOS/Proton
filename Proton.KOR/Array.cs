using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace System
{
    public abstract class Array : ICloneable, IList, ICollection, IEnumerable
    {
        private class NonGenericEnumerator : IEnumerator
        {
            private Array mArray;
            private int mIndex;
            private int mLength;

            public NonGenericEnumerator(Array array)
            {
                mArray = array;
                mIndex = -1;
                mLength = array.mLength;
            }

            public object Current
            {
                get
                {
                    if (mIndex < 0)
                    {
                        throw new InvalidOperationException("Enumeration has not started");
                    }
                    if (mIndex >= mLength)
                    {
                        throw new InvalidOperationException("Enumeration has finished");
                    }
                    return mArray.GetValue(mIndex);
                }
            }

            public bool MoveNext()
            {
                mIndex++;
                return (mIndex < mLength);
            }

            public void Reset() { mIndex = -1; }
        }

        private struct GenericEnumerator<T> : IEnumerator<T>
        {
            private Array mArray;
            private int mIndex;
            private int mLength;

            public GenericEnumerator(Array array)
            {
                mArray = array;
                mIndex = -1;
                mLength = array.mLength;
            }

            public T Current
            {
                get
                {
                    if (mIndex < 0)
                    {
                        throw new InvalidOperationException("Enumeration has not started");
                    }
                    if (mIndex >= mLength)
                    {
                        throw new InvalidOperationException("Enumeration has finished");
                    }
                    return (T)mArray.GetValue(mIndex);
                }
            }

            public void Dispose() { }

            object IEnumerator.Current { get { return Current; } }

            public bool MoveNext()
            {
                mIndex++;
                return (mIndex < mLength);
            }

            public void Reset() { mIndex = -1; }
        }

#pragma warning disable 0649
        private int mLength;
#pragma warning restore 0649

        private Array() { }

        private IEnumerator<T> Internal_GetGenericEnumerator<T>() { return new GenericEnumerator<T>(this); }

        private bool Internal_GenericIsReadOnly() { return true; }

        private void Internal_GenericAdd<T>(T item) { throw new NotSupportedException("Collection is read-only"); }

        private void Internal_GenericClear() { Array.Clear(this, 0, mLength); }

        private bool Internal_GenericContains<T>(T item) { return Array.IndexOf(this, (object)item) >= 0; }

        private void Internal_GenericCopyTo<T>(T[] array, int arrayIndex) { Array.Copy(this, 0, (Array)array, arrayIndex, mLength); }

        private bool Internal_GenericRemove<T>(T item) { throw new NotSupportedException("Collection is read-only"); }

        private int Internal_GenericIndexOf<T>(T item) { return IndexOf(this, (object)item); }

        private void Internal_GenericInsert<T>(int index, T item) { throw new NotSupportedException("List is read-only"); }

        private void Internal_GenericRemoveAt(int index) { throw new NotSupportedException("List is read-only"); }

        private T Internal_GenericGetItem<T>(int index) { return (T)GetValue(index); }

        private void Internal_GenericSetItem<T>(int index, T value) { SetValue((object)value, index); }

        public int Length { get { return mLength; } }

        internal static unsafe object GetValue(Array array, int index)
        {
            Type.TypeData* typeData = array.GetType().GetTypeDataPointer()->ArrayElementType;
            void* startOfArrayElement = (void*)((byte*)array.Internal_ReferenceToPointer() + sizeof(int) + (typeData->Size * index));
            Type type = Type.GetTypeFromHandle(new RuntimeTypeHandle(new IntPtr(typeData)));
            if (!type.IsValueType)
            {
                // TODO: GC support for getting an object, from an existing pointer to an object
                return null;
            }
            // TODO: GC support to Allocate boxed value type based on typeData
            return null;
        }

        internal static unsafe bool SetValue(Array array, object value, int index)
        {
            Type.TypeData* typeData = array.GetType().GetTypeDataPointer()->ArrayElementType;
            if (value.GetType().GetTypeDataPointer() != typeData) return false;
            void* startOfArrayElement = (void*)((byte*)array.Internal_ReferenceToPointer() + sizeof(int) + (typeData->Size * index));
            void* startOfValue = value.Internal_ReferenceToPointer();
            Type type = Type.GetTypeFromHandle(new RuntimeTypeHandle(new IntPtr(typeData)));
            if (!type.IsValueType)
            {
                *((void**)startOfArrayElement) = value.Internal_ReferenceToPointer();
                return true;
            }
            Internal_FastCopy(startOfValue, startOfArrayElement, (int)typeData->Size);
            return true;
        }

        public static unsafe void Clear(Array array, int index, int length)
        {
            if (array == null)
            {
                throw new ArgumentNullException();
            }
            if (index < 0 || length < 0 || (index + length) > array.Length)
            {
                throw new IndexOutOfRangeException();
            }
            void* startOfArrayData = (void*)((byte*)array.Internal_ReferenceToPointer() + sizeof(int));
            Type.TypeData* typeData = array.GetType().GetTypeDataPointer()->ArrayElementType;
            Internal_FastZero((void*)((byte*)startOfArrayData + (index * typeData->Size)), (int)(length * typeData->Size));
        }

        public static void Resize<T>(ref T[] array, int newSize)
        {
            if (newSize < 0)
            {
                throw new ArgumentOutOfRangeException();
            }
            if (array == null)
            {
                array = new T[newSize];
                return;
            }
            if (array.Length == newSize) return;

            T[] oldArray = array;
            array = new T[newSize];
            int copySize = newSize > oldArray.Length ? oldArray.Length : newSize;
            for (int i = 0; i < copySize; ++i) array[i] = oldArray[i];
        }

        public static void Reverse(Array array, int index, int length)
        {
            if (array == null)
            {
                throw new ArgumentNullException();
            }
            if (index < 0 || length < 0)
            {
                throw new ArgumentOutOfRangeException();
            }
            if (index + length > array.mLength)
            {
                throw new ArgumentException();
            }
            if (length > 1)
            {
                int swapped = length >> 1;
                for (int i = 0; i < swapped; ++i)
                {
                    object obj = array.GetValue(index + i);
                    array.SetValue(array.GetValue(index + (length - 1) - i), index + i);
                    array.SetValue(obj, index + (length - 1) - i);
                }
            }
        }

        public static void Reverse(Array array) { Reverse(array, 0, array.mLength); }

        public static int IndexOf(Array array, object value) { return IndexOf(array, value, 0, array.mLength); }

        public static int IndexOf(Array array, object value, int startIndex) { return IndexOf(array, value, startIndex, array.mLength - startIndex); }

        public static int IndexOf(Array array, object value, int startIndex, int count)
        {
            if (array == null)
            {
                throw new ArgumentNullException("array");
            }
            int max = startIndex + count;
            if (startIndex < 0 || max > array.mLength)
            {
                throw new ArgumentOutOfRangeException();
            }
            for (int i = startIndex; i < max; i++)
            {
                if (object.Equals(value, array.GetValue(i)))
                {
                    return i;
                }
            }
            return -1;
        }

        public static void Copy(Array srcArray, int srcIndex, Array dstArray, int dstIndex, int length)
        {
            if (srcArray == null || dstArray == null)
            {
                throw new ArgumentNullException((srcArray == null) ? "sourceArray" : "destinationArray");
            }
            if (srcIndex < 0 || dstIndex < 0 || length < 0)
            {
                throw new ArgumentOutOfRangeException();
            }
            if (srcIndex + length > srcArray.mLength || dstIndex + length > dstArray.mLength)
            {
                throw new ArgumentException();
            }

            int start, inc, end;
            // Need to make sure it works even if both arrays are the same
            if (dstIndex > srcIndex)
            {
                start = 0;
                inc = 1;
                end = length;
            }
            else
            {
                start = length - 1;
                inc = -1;
                end = -1;
            }
            for (int i = start; i != end; i += inc)
            {
                object item = srcArray.GetValue(srcIndex + i);
                dstArray.SetValue(item, dstIndex + i);
            }
        }

        public static void Copy(Array srcArray, Array dstArray, int length) { Copy(srcArray, 0, dstArray, 0, length); }

        public static int IndexOf<T>(T[] array, T value) { return IndexOf((Array)array, (object)value); }

        public static int IndexOf<T>(T[] array, T value, int startIndex) { return IndexOf((Array)array, (object)value, startIndex); }

        public static int IndexOf<T>(T[] array, T value, int startIndex, int count) { return IndexOf((Array)array, (object)value, startIndex, count); }

        public object GetValue(int index)
        {
            if (index < 0 || index >= mLength)
            {
                throw new IndexOutOfRangeException();
            }
            return GetValue(this, index);
        }

        public void SetValue(object value, int index)
        {
            if (index < 0 || index >= mLength)
            {
                throw new IndexOutOfRangeException();
            }
            if (!SetValue(this, value, index))
            {
                throw new InvalidCastException();
            }
        }

        public int Rank { get { return 1; } }

        public int GetLength(int dimension)
        {
            if (dimension != 0)
            {
                throw new IndexOutOfRangeException();
            }
            return mLength;
        }

        public int GetLowerBound(int dimension)
        {
            if (dimension != 0)
            {
                throw new IndexOutOfRangeException();
            }
            return 0;
        }

        public int GetUpperBound(int dimension)
        {
            if (dimension != 0)
            {
                throw new IndexOutOfRangeException();
            }
            return mLength - 1;
        }

        public static TOutput[] ConvertAll<TInput, TOutput>(TInput[] array, Converter<TInput, TOutput> converter)
        {
            if (array == null)
            {
                throw new ArgumentNullException("array");
            }
            if (converter == null)
            {
                throw new ArgumentNullException("converter");
            }

            TOutput[] output = new TOutput[array.Length];
            int arrayLen = array.Length;
            for (int i = 0; i < arrayLen; i++)
            {
                output[i] = converter(array[i]);
            }

            return output;
        }


        public object Clone() { return object.MemberwiseClone(this); }

        public bool IsFixedSize { get { return true; } }

        public bool IsReadOnly { get { return false; } }

        object IList.this[int index]
        {
            get
            {
                if (index < 0 || index >= mLength)
                {
                    throw new ArgumentOutOfRangeException("index");
                }
                return GetValue(index);
            }
            set
            {
                if (index < 0 || index >= mLength)
                {
                    throw new ArgumentOutOfRangeException("index");
                }
                SetValue(value, index);
            }
        }

        int IList.Add(object value) { throw new NotSupportedException("Collection is read-only"); }

        void IList.Clear() { Array.Clear(this, 0, mLength); }

        bool IList.Contains(object value) { return (IndexOf(this, value) >= 0); }

        int IList.IndexOf(object value) { return IndexOf(this, value); }

        void IList.Insert(int index, object value) { throw new NotSupportedException("Collection is read-only"); }

        void IList.Remove(object value) { throw new NotSupportedException("Collection is read-only"); }

        void IList.RemoveAt(int index) { throw new NotSupportedException("Collection is read-only"); }

        int ICollection.Count { get { return mLength; } }

        public bool IsSynchronized { get { return false; } }

        public object SyncRoot { get { return this; } }

        public void CopyTo(Array array, int index) { Copy(this, 0, array, index, this.mLength); }

        public IEnumerator GetEnumerator() { return new NonGenericEnumerator(this); }

        IEnumerator IEnumerable.GetEnumerator() { return GetEnumerator(); }
    }
}
