using System.Globalization;

namespace System
{
    public struct Int16 : IFormattable, IComparable, IComparable<short>, IEquatable<short>
    {
        public const short MaxValue = 0x7fff;
        public const short MinValue = -32768;

#pragma warning disable 0649
        private short mValue;
#pragma warning restore 0649

        public override bool Equals(object obj) { return (obj is short) && ((short)obj).mValue == mValue; }

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
            if (!(obj is short))
            {
                throw new ArgumentException();
            }
            return CompareTo((short)obj);
        }

        public int CompareTo(short value) { return mValue > value ? 1 : (mValue < value ? -1 : 0); }

        public bool Equals(short obj) { return mValue == obj; }
    }
}
