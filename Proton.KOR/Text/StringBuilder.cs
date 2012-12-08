using System.Runtime.CompilerServices;

namespace System.Text
{
    public sealed class StringBuilder
    {

        private const int defaultMaxCapacity = int.MaxValue;
        private const int defaultInitialCapacity = 16;

        private int mLength;
        private int mCapacity;
        private char[] mData;

        #region Constructors

        public StringBuilder() : this(defaultInitialCapacity, defaultMaxCapacity) { }

        public StringBuilder(int initialCapacity) : this(initialCapacity, defaultMaxCapacity) { }

        public StringBuilder(int initialCapacity, int maxCapacity)
        {
            this.mCapacity = Math.Max(initialCapacity, 2);
            this.mLength = 0;
            this.mData = new char[this.mCapacity];
        }

        public StringBuilder(string value)
            : this((value != null) ? value.Length : defaultInitialCapacity, defaultMaxCapacity)
        {
            if (value != null)
            {
                this.Append(value);
            }
        }

        public StringBuilder(string value, int initialCapacity)
            : this(initialCapacity, defaultMaxCapacity)
        {
            if (value != null)
            {
                this.Append(value);
            }
        }

        public StringBuilder(string value, int startIndex, int length, int initialCapacity)
            : this(initialCapacity, defaultMaxCapacity)
        {
            if (value == null)
            {
                value = string.Empty;
            }
            if (startIndex < 0 || length < 0 || startIndex + length > value.Length)
            {
                throw new ArgumentOutOfRangeException();
            }
            this.Append(value, startIndex, length);
        }

        #endregion

        public override string ToString()
        {
            return new string(this.mData, 0, this.mLength);
        }

        public string ToString(int startIndex, int length)
        {
            if (startIndex < 0 || length < 0 || startIndex + length > this.mLength)
            {
                throw new ArgumentOutOfRangeException();
            }
            return new string(this.mData, startIndex, length);
        }

        private void EnsureSpace(int space)
        {
            if (this.mLength + space > this.mCapacity)
            {
                do
                {
                    this.mCapacity <<= 1;
                } while (this.mCapacity < this.mLength + space);
                char[] newData = new char[mCapacity];
                Array.Copy(this.mData, 0, newData, 0, this.mLength);
                this.mData = newData;
            }
        }

        public int Length
        {
            get
            {
                return this.mLength;
            }
            set
            {
                if (value < 0)
                {
                    throw new ArgumentOutOfRangeException();
                }
                if (value > this.mLength)
                {
                    this.EnsureSpace(this.mLength - value);
                    for (int i = this.mLength; i < value; i++)
                    {
                        this.mData[i] = '\x0000';
                    }
                }
                this.mLength = value;
            }
        }

        public int Capacity
        {
            get
            {
                return this.mCapacity;
            }
        }

        [IndexerName("Chars")]
        public char this[int index]
        {
            get
            {
                if (index < 0 || index >= this.mLength)
                {
                    throw new IndexOutOfRangeException();
                }
                return this.mData[index];
            }
            set
            {
                if (index < 0 || index >= this.mLength)
                {
                    throw new IndexOutOfRangeException();
                }
                this.mData[index] = value;
            }
        }

        public void CopyTo(int srcIndex, char[] dst, int dstIndex, int count)
        {
            if (dst == null)
            {
                throw new ArgumentNullException("destination");
            }
            if (srcIndex < 0 || count < 0 || dstIndex < 0 ||
                srcIndex + count > this.mLength || dstIndex + count > dst.Length)
            {
                throw new ArgumentOutOfRangeException();
            }
            Array.Copy(this.mData, srcIndex, dst, dstIndex, count);
        }

        public void EnsureCapacity(int capacity)
        {
            if (this.mCapacity < capacity)
            {
                // This is not quite right, as it will often over-allocate memory
                this.EnsureSpace(capacity - this.mCapacity);
            }
        }

        public StringBuilder Remove(int startIndex, int length)
        {
            if (startIndex < 0 || length < 0 || startIndex + length > this.mLength)
            {
                throw new ArgumentOutOfRangeException();
            }
            Array.Copy(this.mData, startIndex + length, this.mData, startIndex, this.mLength - length - startIndex);
            this.mLength -= length;
            return this;
        }

        #region Append Methods

        public StringBuilder Append(string value)
        {
            int len = value.Length;
            this.EnsureSpace(len);
            for (int i = 0; i < len; i++)
            {
                this.mData[this.mLength++] = value[i];
            }
            return this;
        }

        public StringBuilder Append(string value, int startIndex, int count)
        {
            if (value == null)
            {
                return this;
            }
            if (startIndex < 0 || count < 0 || value.Length < startIndex + count)
            {
                throw new ArgumentOutOfRangeException();
            }
            this.EnsureSpace(count);
            for (int i = 0; i < count; i++)
            {
                this.mData[this.mLength++] = value[startIndex + i];
            }
            return this;
        }

        public StringBuilder Append(char value)
        {
            EnsureSpace(1);
            mData[mLength++] = value;
            return this;
        }

        public StringBuilder Append(char value, int repeatCount)
        {
            if (repeatCount < 0)
            {
                throw new ArgumentOutOfRangeException();
            }
            EnsureSpace(repeatCount);
            for (int i = 0; i < repeatCount; i++)
            {
                this.mData[this.mLength++] = value;
            }
            return this;
        }

        public StringBuilder Append(char[] value)
        {
            if (value == null)
            {
                return this;
            }
            int addLen = value.Length;
            this.EnsureSpace(addLen);
            Array.Copy(value, 0, this.mData, this.mLength, addLen);
            this.mLength += addLen;
            return this;
        }

        public StringBuilder Append(char[] value, int startIndex, int charCount)
        {
            if (value == null)
            {
                return this;
            }
            if (charCount < 0 || startIndex < 0 || value.Length < (startIndex + charCount))
            {
                throw new ArgumentOutOfRangeException();
            }
            this.EnsureSpace(charCount);
            Array.Copy(value, startIndex, this.mData, this.mLength, charCount);
            this.mLength += charCount;
            return this;
        }

        public StringBuilder Append(object value)
        {
            if (value == null)
            {
                return this;
            }
            return Append(value.ToString());
        }

        public StringBuilder Append(bool value)
        {
            return Append(value.ToString());
        }

        public StringBuilder Append(byte value)
        {
            return Append(value.ToString());
        }

        public StringBuilder Append(decimal value)
        {
            return Append(value.ToString());
        }

        public StringBuilder Append(double value)
        {
            return Append(value.ToString());
        }

        public StringBuilder Append(short value)
        {
            return Append(value.ToString());
        }

        public StringBuilder Append(int value)
        {
            return Append(value.ToString());
        }

        public StringBuilder Append(long value)
        {
            return Append(value.ToString());
        }

        public StringBuilder Append(sbyte value)
        {
            return Append(value.ToString());
        }

        public StringBuilder Append(float value)
        {
            return Append(value.ToString());
        }

        public StringBuilder Append(ushort value)
        {
            return Append(value.ToString());
        }

        public StringBuilder Append(uint value)
        {
            return Append(value.ToString());
        }

        public StringBuilder Append(ulong value)
        {
            return Append(value.ToString());
        }

        #endregion

        #region AppendFormat Methods

        public StringBuilder AppendFormat(string format, object obj0)
        {
            StringHelper.FormatHelper(this, null, format, obj0);
            return this;
        }

        public StringBuilder AppendFormat(string format, object obj0, object obj1)
        {
            StringHelper.FormatHelper(this, null, format, obj0, obj1);
            return this;
        }

        public StringBuilder AppendFormat(string format, object obj0, object obj1, object obj2)
        {
            StringHelper.FormatHelper(this, null, format, obj0, obj1, obj2);
            return this;
        }

        public StringBuilder AppendFormat(string format, params object[] objs)
        {
            StringHelper.FormatHelper(this, null, format, objs);
            return this;
        }

        public StringBuilder AppendFormat(IFormatProvider provider, string format, params object[] objs)
        {
            StringHelper.FormatHelper(this, provider, format, objs);
            return this;
        }

        #endregion

        #region AppendLine Methods

        public StringBuilder AppendLine()
        {
            return this.Append(Environment.NewLine);
        }

        public StringBuilder AppendLine(string value)
        {
            return this.Append(value).Append(Environment.NewLine);
        }

        #endregion

        #region Insert Methods

        public StringBuilder Insert(int index, string value)
        {
            if (index < 0 || index > this.mLength)
            {
                throw new ArgumentOutOfRangeException();
            }
            if (string.IsNullOrEmpty(value))
            {
                return this;
            }
            int len = value.Length;
            EnsureSpace(len);
            Array.Copy(this.mData, index, this.mData, index + len, this.mLength - index);
            for (int i = 0; i < len; i++)
            {
                this.mData[index + i] = value[i];
            }
            this.mLength += len;
            return this;
        }

        public StringBuilder Insert(int index, bool value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, byte value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, char value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, char[] value)
        {
            return this.Insert(index, new string(value));
        }

        public StringBuilder Insert(int index, decimal value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, double value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, short value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, int value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, long value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, object value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, sbyte value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, float value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, ushort value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, uint value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, ulong value)
        {
            return this.Insert(index, value.ToString());
        }

        public StringBuilder Insert(int index, string value, int count)
        {
            if (count < 0)
            {
                throw new ArgumentOutOfRangeException();
            }
            if (count == 0 || string.IsNullOrEmpty(value))
            {
                return this;
            }
            StringBuilder toInsert = new StringBuilder(value.Length * count);
            for (; count > 0; count--)
            {
                toInsert.Append(value);
            }
            return this.Insert(index, toInsert.ToString());
        }

        public StringBuilder Insert(int index, char[] value, int startIndex, int charCount)
        {
            if (value == null && (startIndex != 0 || charCount != 0))
            {
                throw new ArgumentNullException("value");
            }
            if (startIndex < 0 || charCount < 0 || startIndex + charCount > value.Length)
            {
                throw new ArgumentOutOfRangeException();
            }
            return this.Insert(index, new string(value, startIndex, charCount));
        }

        #endregion

        #region Replace Methods

        public StringBuilder Replace(char oldChar, char newChar)
        {
            return this.Replace(oldChar, newChar, 0, this.mLength);
        }

        public StringBuilder Replace(string oldValue, string newValue)
        {
            return this.Replace(oldValue, newValue, 0, this.mLength);
        }

        public StringBuilder Replace(char oldChar, char newChar, int startIndex, int count)
        {
            if (startIndex < 0 || count < 0 || startIndex + count > this.mLength)
            {
                throw new ArgumentOutOfRangeException();
            }
            for (int i = 0; i < count; i++)
            {
                if (this.mData[startIndex + i] == oldChar)
                {
                    this.mData[startIndex + i] = newChar;
                }
            }
            return this;
        }

        public StringBuilder Replace(string oldValue, string newValue, int startIndex, int count)
        {
            string subStr = this.ToString(startIndex, count);
            subStr = subStr.Replace(oldValue, newValue);
            this.Remove(startIndex, count);
            this.Insert(startIndex, subStr);
            return this;
        }

        #endregion
    }
}
