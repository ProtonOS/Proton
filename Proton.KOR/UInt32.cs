using System.Globalization;

namespace System
{
    public struct UInt32 : IFormattable, IComparable, IComparable<uint>, IEquatable<uint>
    {
        public const uint MaxValue = 0xffffffff;
        public const uint MinValue = 0;

#pragma warning disable 0649
        private uint mValue;
#pragma warning restore 0649

        public override bool Equals(object obj) { return (obj is uint) && ((uint)obj).mValue == mValue; }

        public override int GetHashCode() { return (int)mValue; }

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
            if (!(obj is uint))
            {
                throw new ArgumentException();
            }
            return CompareTo((uint)obj);
        }

        public int CompareTo(uint value) { return mValue > value ? 1 : (mValue < value ? -1 : 0); }

        public bool Equals(uint obj) { return mValue == obj; }
    }
}
