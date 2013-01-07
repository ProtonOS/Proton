using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Text;

namespace System
{
    public class String : ICloneable, IEnumerable, IEnumerable<char>,
        IComparable, IComparable<string>, IEquatable<string>
    {
        public static readonly string Empty = "";

        public static bool IsNullOrEmpty(string value) { return (value == null) || (value.mLength == 0); }

        // This field must be the only field, to tie up with GC code, and must be 32bits
        private int mLength;

		internal unsafe char* InternalCharDataPointer { get { return (char*)((ulong)Internal_ReferenceToPointer() + sizeof(int)); } }

        #region Private Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)]
		public extern String(char c, int count);

		private static unsafe string InternalConcat(string str0, string str1)
		{
			int str0Length = str0.mLength;
			int str1Length = str1.mLength;
			int strLength = str0Length + str1Length;
			string str = GC.AllocateEmptyStringOfLength((uint)strLength);
			char* strData = str.InternalCharDataPointer;
			char* str0Data = str0.InternalCharDataPointer;
			char* str1Data = str1.InternalCharDataPointer;
			for (int index = 0; index < str0Length; ++index) strData[index] = str0Data[index];
			for (int index = 0; index < str1Length; ++index) strData[str0Length + index] = str1Data[index];
			return str;
		}

		private unsafe string InternalReplace(string oldValue, string newValue)
		{
			int oldLength = oldValue.Length;
			if (oldLength > mLength) return this;
			int newLength = newValue.Length;
			int thisLength = mLength;
			char* oldData = oldValue.InternalCharDataPointer;
			char* newData = newValue.InternalCharDataPointer;
			char* thisData = InternalCharDataPointer;
			List<int> foundIndexes = new List<int>();
			for (int index = 0; index < thisLength; ++index)
			{
				bool found = false;
				for (int oldIndex = 0; oldIndex < oldLength && (found = (thisData[index + oldIndex] == oldValue[oldIndex])); ++index) ;
				if (found)
				{
					foundIndexes.Add(index);
					index += (oldLength - 1);
				}
			}
			if (foundIndexes.Count == 0) return this;
			int strLength = thisLength - (oldLength * foundIndexes.Count) + (newLength * foundIndexes.Count);
			string str = GC.AllocateEmptyStringOfLength((uint)strLength);
			char* strData = str.InternalCharDataPointer;
			int strIndex = 0;
			for (int index = 0; index < thisLength; ++index)
			{
				if (foundIndexes.Count > 0 && foundIndexes[0] == index)
				{
					for (int newIndex = 0; newIndex < newLength; ++newIndex, ++strIndex) strData[strIndex] = newData[newIndex];
					index += (oldLength - 1);
					foundIndexes.RemoveAt(0);
				}
				else
				{
					strData[strIndex] = thisData[index];
					++strIndex;
				}
			}
			return str;
		}

		// trimType: 1 = start; 2 = end; 3 = both
		private unsafe string InternalTrim(char[] trimChars, int trimType)
		{
			int removedFromStart = 0;
			int removedFromEnd = 0;
			char* thisData = InternalCharDataPointer;
			int thisLength = mLength;
			int trimLength = trimChars.Length;
			if (trimType == 1 || trimType == 3)
			{
				for (int thisIndex = 0; thisIndex < thisLength; ++thisIndex)
				{
					bool found = false;
					for (int trimIndex = 0; trimIndex < trimLength; ++trimIndex)
					{
						if (trimChars[trimIndex] == thisData[thisIndex])
						{
							++removedFromStart;
							found = true;
							break;
						}
					}
					if (!found) break;
				}
			}
			if (trimType == 2 || trimType == 3)
			{
				for (int thisIndex = thisLength - 1; thisIndex >= 0; --thisIndex)
				{
					bool found = false;
					for (int trimIndex = 0; trimIndex < trimLength; ++trimIndex)
					{
						if (trimChars[trimIndex] == thisData[thisIndex])
						{
							++removedFromEnd;
							found = true;
							break;
						}
					}
					if (!found) break;
				}
			}
			if (removedFromStart == 0 && removedFromEnd == 0) return this;
			int strLength = thisLength - (removedFromStart + removedFromEnd);
			return GC.AllocateStringFromString(this, (uint)removedFromStart, (uint)strLength);
		}

		private unsafe int InternalIndexOf(char value, int startIndex, int count, bool forward)
		{
			if (count == 0) return -1;
			char* thisData = InternalCharDataPointer;
			int thisLength = mLength;
			int index = startIndex;
			while (count > 0)
			{
				if (index < 0 || index >= thisLength) break;
				if (thisData[index] == value) return index;
				index += forward ? 1 : -1;
				--count;
			}
			return -1;
		}

		private unsafe int InternalIndexOfAny(char[] anyOf, int startIndex, int count, bool forward)
		{
			if (count == 0) return -1;
			char* thisData = InternalCharDataPointer;
			int thisLength = mLength;
			int index = startIndex;
			int anyOfLength = anyOf.Length;
			while (count > 0)
			{
				if (index < 0 || index >= thisLength) break;
				for (int anyIndex = 0; anyIndex < anyOfLength; ++anyIndex)
				{
					if (thisData[index] == anyOf[anyIndex]) return index;
				}
				index += forward ? 1 : -1;
				--count;
			}
			return -1;
		}

        #endregion

        public virtual int Length { get { return mLength; } }

        [IndexerName("Chars")]
		public unsafe virtual char this[int index]
        {
			get
			{
				if (index < 0 || index >= mLength) throw new ArgumentOutOfRangeException();
				char* thisData = InternalCharDataPointer;
				return thisData[index];
			}
        }

        #region Misc Methods

        public static string Join(string separator, string[] value)
        {
            return Join(separator, value, 0, value.Length);
        }

        public static string Join(string separator, string[] value, int startIndex, int count)
        {
            StringBuilder sb = new StringBuilder();
            for (int i = startIndex; i < count; i++)
            {
                sb.Append(value[i]);
                sb.Append(separator);
            }
            return sb.ToString(0, sb.Length - separator.Length);
        }

		public static string Join<T>(string separator, IEnumerable<T> values)
		{
			List<T> lst = new List<T>(values);
			string[] strvalues = new string[lst.Count];
			for (int index = 0; index < lst.Count; ++index) strvalues[index] = lst[index].ToString();
			return Join(separator, strvalues);
		}

		public string[] Split(params char[] separator)
        {
            return this.Split(separator, int.MaxValue);
        }

        public string[] Split(char[] separator, int count)
        {
            if (count < 0)
            {
                throw new ArgumentException("count");
            }
            if (count == 0)
            {
                return new string[0];
            }
            if (separator == null || separator.Length == 0)
            {
                separator = char.WhiteChars;
            }

            List<string> ret = new List<string>();

            int pos = 0;
            for (; count > 0; count--)
            {
                int sepPos = this.IndexOfAny(separator, pos);
                if (sepPos < 0)
                {
                    ret.Add(GC.AllocateStringFromString(this, (uint)pos, (uint)(mLength - pos)));
                    break;
                }
                ret.Add(GC.AllocateStringFromString(this, (uint)pos, (uint)(sepPos - pos)));
                pos = sepPos + 1;
            }

            return ret.ToArray();
        }

        public bool StartsWith(string str)
        {
            return this.Substring(0, str.mLength) == str;
        }

        public bool EndsWith(string str)
        {
            return this.Substring(this.mLength - str.mLength, str.mLength) == str;
        }

        #endregion

        #region Concat Methods

        public static string Concat(string str0, string str1)
        {
            if (str0 == null)
            {
                return str1 ?? string.Empty;
            }
            if (str1 == null)
            {
                return str0;
            }
            return InternalConcat(str0, str1);
        }

        public static string Concat(string str0, string str1, string str2)
        {
            return Concat(Concat(str0, str1), str2);
        }

        public static string Concat(string str0, string str1, string str2, string str3)
        {
            return Concat(Concat(str0, str1), Concat(str2, str3));
        }

        public static string Concat(params string[] values)
        {
            if (values == null)
            {
                throw new ArgumentNullException("args");
            }
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < values.Length; i++)
            {
                sb.Append(values[i]);
            }
            return sb.ToString();
        }

        public static string Concat(object obj0)
        {
            return obj0.ToString();
        }

        public static string Concat(object obj0, object obj1)
        {
            string str0 = (obj0 == null) ? null : obj0.ToString();
            string str1 = (obj1 == null) ? null : obj1.ToString();
            if (str0 == null)
            {
                return str1 ?? string.Empty;
            }
            if (str1 == null)
            {
                return str0;
            }
            return InternalConcat(str0, str1);
        }

        public static string Concat(object obj0, object obj1, object obj2)
        {
            return Concat(new object[] { obj0, obj1, obj2 });
        }

        public static string Concat(object obj0, object obj1, object obj2, object obj3)
        {
            return Concat(new object[] { obj0, obj1, obj2, obj3 });
        }

        public static string Concat(params object[] objs)
        {
            if (objs == null)
            {
                throw new ArgumentNullException("args");
            }
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < objs.Length; i++)
            {
                sb.Append(objs[i]);
            }
            return sb.ToString();
        }

        #endregion

        #region Trim Methods

        public string Trim(params char[] trimChars)
        {
            if (trimChars == null || trimChars.Length == 0)
            {
                trimChars = char.WhiteChars;
            }
            return InternalTrim(trimChars, 3);
        }

        public string Trim()
        {
            return InternalTrim(char.WhiteChars, 3);
        }

        public string TrimStart(params char[] trimChars)
        {
            if (trimChars == null || trimChars.Length == 0)
            {
                trimChars = char.WhiteChars;
            }
            return InternalTrim(trimChars, 1);
        }

        public string TrimEnd(params char[] trimChars)
        {
            if (trimChars == null || trimChars.Length == 0)
            {
                trimChars = char.WhiteChars;
            }
            return InternalTrim(trimChars, 2);
        }

        #endregion

        #region Substring Methods

        public string Substring(int startIndex)
        {
            if (startIndex < 0 || startIndex > this.mLength)
            {
                throw new ArgumentOutOfRangeException();
            }

            return GC.AllocateStringFromString(this, (uint)startIndex, (uint)(mLength - startIndex));
        }

        public string Substring(int startIndex, int length)
        {
            if (startIndex < 0 || length < 0 || startIndex + length > this.mLength)
            {
                throw new ArgumentOutOfRangeException();
            }

            return GC.AllocateStringFromString(this, (uint)startIndex, (uint)length);
        }

        #endregion

        #region Format Methods

        public static string Format(string format, object obj0)
        {
            return Format(null, format, new object[] { obj0 });
        }

        public static string Format(string format, object obj0, object obj1)
        {
            return Format(null, format, new object[] { obj0, obj1 });
        }

        public static string Format(string format, object obj0, object obj1, object obj2)
        {
            return Format(null, format, new object[] { obj0, obj1, obj2 });
        }

        public static string Format(string format, params object[] args)
        {
            return Format(null, format, args);
        }

        public static string Format(IFormatProvider provider, string format, params object[] args)
        {
            StringBuilder sb = new StringBuilder();
            StringHelper.FormatHelper(sb, provider, format, args);
            return sb.ToString();
        }

        #endregion

        #region Replace & Remove Methods

        public string Replace(char oldChar, char newChar)
        {
            StringBuilder sb = new StringBuilder(this);
            return sb.Replace(oldChar, newChar).ToString();
        }

        public string Replace(string oldValue, string newValue)
        {
            if (oldValue == null)
            {
                throw new ArgumentNullException("oldValue");
            }
            if (oldValue.Length == 0)
            {
                throw new ArgumentException("oldValue is an empty string.");
            }
            if (this.mLength == 0)
            {
                return this;
            }
            if (newValue == null)
            {
                newValue = string.Empty;
            }
            return InternalReplace(oldValue, newValue);
        }

        public string Remove(int startIndex)
        {
            if (startIndex < 0 || startIndex >= this.mLength)
            {
                throw new ArgumentOutOfRangeException("startIndex");
            }
            return GC.AllocateStringFromString(this, 0, (uint)startIndex);
        }

        public string Remove(int startIndex, int count)
        {
            if (startIndex < 0 || count < 0 || startIndex + count >= this.mLength)
            {
                throw new ArgumentOutOfRangeException();
            }
            int pos2 = startIndex + count;
            return (GC.AllocateStringFromString(this, 0, (uint)startIndex)) + (GC.AllocateStringFromString(this, (uint)pos2, (uint)(mLength - pos2)));
        }

        #endregion

        #region Compare and CompareOrdinal Methods

        public static int Compare(string strA, string strB)
        {
            return CompareOrdinal(strA, strB);
        }

        public static int Compare(string strA, int indexA, string strB, int indexB, int length)
        {
            return CompareOrdinal(strA.Substring(indexA, length), strB.Substring(indexB, length));
        }

        public static int CompareOrdinal(string strA, string strB)
        {
            if (strA == null)
            {
                if (strB == null)
                {
                    return 0;
                }
                return -1;
            }
            if (strB == null)
            {
                return 1;
            }
            int top = Math.Min(strA.Length, strB.Length);
            for (int i = 0; i < top; i++)
            {
                if (strA[i] != strB[i])
                {
                    return (strA[i] - strB[i]);
                }
            }
            return strA.Length - strB.Length;
        }

        #endregion

        #region IndexOf... Methods

        public int IndexOf(string value)
        {
            return IndexOf(value, 0, this.mLength);
        }

        public int IndexOf(string value, int startIndex)
        {
            return IndexOf(value, startIndex, this.mLength - startIndex);
        }

        public int IndexOf(string value, int startIndex, int count)
        {
            if (value == null)
            {
                throw new ArgumentNullException("value");
            }
            if (startIndex < 0 || count < 0 || startIndex + count > this.mLength)
            {
                throw new ArgumentOutOfRangeException();
            }
            if (value.mLength == 0)
            {
                return startIndex;
            }
            int valueLen = value.mLength;
            int finalIndex = startIndex + count - valueLen + 1;
            char char0 = value[0];
            for (int i = startIndex; i < finalIndex; i++)
            {
                if (this[i] == char0)
                {
                    bool ok = true;
                    for (int j = 1; j < valueLen; j++)
                    {
                        if (this[i + j] != value[j])
                        {
                            ok = false;
                            break;
                        }
                    }
                    if (ok)
                    {
                        return i;
                    }
                }
            }
            return -1;
        }

        public int IndexOf(char value)
        {
            return this.IndexOf(value, 0, this.mLength, true);
        }

        public int IndexOf(char value, int startIndex)
        {
            return this.IndexOf(value, startIndex, this.mLength - startIndex, true);
        }

        public int IndexOf(char value, int startIndex, int count)
        {
            return this.IndexOf(value, startIndex, count, true);
        }

        public int LastIndexOf(char value)
        {
            return this.IndexOf(value, 0, this.mLength, false);
        }

        public int LastIndexOf(char value, int startIndex)
        {
            return this.IndexOf(value, startIndex, this.mLength - startIndex, false);
        }

        public int LastIndexOf(char value, int startIndex, int count)
        {
            return this.IndexOf(value, startIndex, count, false);
        }

        private int IndexOf(char value, int startIndex, int count, bool forwards)
        {
            if (startIndex < 0 || count < 0 || startIndex + count > this.mLength)
            {
                throw new ArgumentOutOfRangeException();
            }
            return this.InternalIndexOf(value, startIndex, count, forwards);
        }

        public int IndexOfAny(char[] anyOf)
        {
            return this.IndexOfAny(anyOf, 0, this.mLength, true);
        }

        public int IndexOfAny(char[] anyOf, int startIndex)
        {
            return this.IndexOfAny(anyOf, startIndex, this.mLength - startIndex, true);
        }

        public int IndexOfAny(char[] anyOf, int startIndex, int count)
        {
            return this.IndexOfAny(anyOf, startIndex, count, true);
        }

        public int LastIndexOfAny(char[] anyOf)
        {
            return this.IndexOfAny(anyOf, 0, this.mLength, false);
        }

        public int LastIndexOfAny(char[] anyOf, int startIndex)
        {
            return this.IndexOfAny(anyOf, startIndex, this.mLength - startIndex, false);
        }

        public int LastIndexOfAny(char[] anyOf, int startIndex, int count)
        {
            return this.IndexOfAny(anyOf, startIndex, count, false);
        }

        private int IndexOfAny(char[] anyOf, int startIndex, int count, bool forward)
        {
            if (startIndex < 0 || count < 0 || startIndex + count > this.mLength)
            {
                throw new ArgumentOutOfRangeException();
            }
            /*int anyOfLen = anyOf.Length;
            int finIndex = (forward) ? (startIndex + count) : (startIndex - 1);
            int inc = (forward) ? 1 : -1;
            for (int i = (forward) ? startIndex : (startIndex + count - 1); i != finIndex; i += inc) {
                char c = this[i];
                for (int j = 0; j < anyOfLen; j++) {
                    if (c == anyOf[j]) {
                        return i;
                    }
                }
            }
            return -1;*/
            return this.InternalIndexOfAny(anyOf, startIndex, count, forward);
        }

        #endregion

        #region Case methods

        public string ToLower()
        {
            return ToLower(CultureInfo.CurrentCulture);
        }

        public string ToLower(CultureInfo culture)
        {
            if (culture == null)
            {
                throw new ArgumentNullException("culture");
            }
            if (culture.LCID == 0x007f)
            {
                return ToLowerInvariant();
            }
            return culture.TextInfo.ToLower(this);
        }

        public string ToLowerInvariant()
        {
            int len = this.mLength;
            StringBuilder sb = new StringBuilder(len);
            for (int i = 0; i < len; i++)
            {
                sb.Append(char.ToLowerInvariant(this[i]));
            }
            return sb.ToString();
        }

        public string ToUpper()
        {
            return ToLower(CultureInfo.CurrentCulture);
        }

        public string ToUpper(CultureInfo culture)
        {
            if (culture == null)
            {
                throw new ArgumentNullException("culture");
            }
            if (culture.LCID == 0x007f)
            {
                return ToUpperInvariant();
            }
            return culture.TextInfo.ToUpper(this);
        }

        public string ToUpperInvariant()
        {
            int len = this.mLength;
            StringBuilder sb = new StringBuilder(len);
            for (int i = 0; i < len; i++)
            {
                sb.Append(char.ToUpperInvariant(this[i]));
            }
            return sb.ToString();
        }

        #endregion

        #region Overrides and Operators

        public override string ToString()
        {
            return this;
        }

        public override bool Equals(object obj)
        {
            return Equals(this, obj as string);
        }

        public static bool operator ==(string a, string b)
        {
            return Equals(a, b);
        }

        public static bool operator !=(string a, string b)
        {
            return !Equals(a, b);
        }

		public unsafe static bool Equals(string a, string b)
		{
			if (object.ReferenceEquals(a, b)) return true;
			if (a == null || b == null) return false;
			if (a.mLength != b.mLength) return false;
			int length = a.mLength;
			char* aData = a.InternalCharDataPointer;
			char* bData = b.InternalCharDataPointer;
			for (int index = 0; index < length; ++index) if (aData[index] != bData[index]) return false;
			return true;
		}

		public unsafe override int GetHashCode()
		{
			char* thisData = InternalCharDataPointer;
			int thisLength = mLength;
			int hash = 0;
			for (int index = 0; index < thisLength; ++index) hash = (hash << 5) - hash + thisData[index];
			return hash;
		}

		#endregion

		#region IClonable Members

		public object Clone()
        {
            return this;
        }

        #endregion

        #region IComparable Members

        public int CompareTo(object value)
        {
            if (value == null)
            {
                return 1;
            }
            if (!(value is string))
            {
                throw new ArgumentException();
            }
            return string.Compare(this, (string)value);
        }

        public int CompareTo(string value)
        {
            if (value == null)
                return 1;

            return string.Compare(this, value);
        }

        #endregion

        #region IEquatable<string> Members

        public bool Equals(string other)
        {
            return Equals(this, other);
        }

        #endregion

        #region IEnumerable Members

        public IEnumerator GetEnumerator()
        {
            return new CharEnumerator(this);
        }

        IEnumerator<char> IEnumerable<char>.GetEnumerator()
        {
            return new CharEnumerator(this);
        }

        #endregion
    }
}
