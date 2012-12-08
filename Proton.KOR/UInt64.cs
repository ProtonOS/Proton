using System.Globalization;

namespace System
{
    public struct UInt64 : IFormattable, IComparable, IComparable<ulong>, IEquatable<ulong>
    {
        public const ulong MinValue = 0;
        public const ulong MaxValue = 0xffffffffffffffffL;

#pragma warning disable 0649
        private ulong mValue;
#pragma warning restore 0649

        public override bool Equals(object obj) { return (obj is ulong) && ((ulong)obj).mValue == mValue; }

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
            if (!(obj is ulong))
            {
                throw new ArgumentException();
            }
            return CompareTo((ulong)obj);
        }

        public int CompareTo(ulong value) { return mValue > value ? 1 : (mValue < value ? -1 : 0); }

        public bool Equals(ulong obj) { return mValue == obj; }
    }
}
