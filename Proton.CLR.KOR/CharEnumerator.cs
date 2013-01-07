using System.Collections;
using System.Collections.Generic;

namespace System
{
    public sealed class CharEnumerator : ICloneable, IEnumerator, IEnumerator<char>
    {
        private string mString;
        private int mIndex;
        private int mLength;

        internal CharEnumerator(string s)
        {
            mString = s;
            mIndex = -1;
            mLength = s.Length;
        }

        public object Clone() { return object.MemberwiseClone(this); }

        object IEnumerator.Current { get { return Current; } }

        public bool MoveNext()
        {
            mIndex++;
            return (mIndex < mLength);
        }

        public void Reset() { mIndex = -1; }

        public char Current
        {
            get
            {
                if (mIndex == -1 || mIndex >= mLength)
                {
                    throw new InvalidOperationException("The position is not valid.");
                }
                return mString[mIndex];
            }
        }

        public void Dispose() { }
    }
}
