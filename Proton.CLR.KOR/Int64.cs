using System.Globalization;

namespace System
{
    public struct Int64 : IFormattable, IComparable, IComparable<long>, IEquatable<long>
    {
        public const long MaxValue = 0x7fffffffffffffff;
        public const long MinValue = unchecked((long)0x8000000000000000);

        private long mValue;

        public override bool Equals(object obj) { return (obj is long) && ((long)obj).mValue == mValue; }

        public override int GetHashCode() { return (int)(mValue & 0xffffffff) ^ (int)(mValue >> 32); }

        public override string ToString() { return NumberFormatter.FormatGeneral(new NumberFormatter.NumberStore(mValue)); }

        public string ToString(IFormatProvider formatProvider) { return NumberFormatter.FormatGeneral(new NumberFormatter.NumberStore(mValue), formatProvider); }

        public string ToString(string format) { return ToString(format, null); }

        public string ToString(string format, IFormatProvider formatProvider) { return NumberFormatter.NumberToString(format, mValue, NumberFormatInfo.GetInstance(formatProvider)); }

        public int CompareTo(object obj)
        {
            if (obj == null)
            {
                return 1;
            }
            if (!(obj is long))
            {
                throw new ArgumentException();
            }
            return CompareTo((long)obj);
        }

        public int CompareTo(long value) { return mValue > value ? 1 : (mValue < value ? -1 : 0); }

        public bool Equals(long obj) { return mValue == obj; }
    }
}
