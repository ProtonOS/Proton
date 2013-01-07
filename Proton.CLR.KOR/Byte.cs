using System.Globalization;

namespace System
{
    public struct Byte : IFormattable, IComparable, IComparable<byte>, IEquatable<byte>
    {
        public const byte MinValue = 0;
        public const byte MaxValue = 255;

#pragma warning disable 0649
        private byte mValue;
#pragma warning restore 0649

        public override bool Equals(object obj) { return (obj is byte) && ((byte)obj).mValue == mValue; }

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
            if (!(obj is byte))
            {
                throw new ArgumentException();
            }
            return CompareTo((byte)obj);
        }

        public int CompareTo(byte value) { return mValue > value ? 1 : (mValue < value ? -1 : 0); }

        public bool Equals(byte obj) { return mValue == obj; }
    }
}
