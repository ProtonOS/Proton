using System.Globalization;

namespace System
{
    public struct UInt16 : IFormattable, IComparable, IComparable<ushort>, IEquatable<ushort>
    {
        public const ushort MaxValue = 0xffff;
        public const ushort MinValue = 0;

#pragma warning disable 0649
        private ushort mValue;
#pragma warning restore 0649

        public override bool Equals(object obj) { return (obj is ushort) && ((ushort)obj).mValue == mValue; }

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
            if (!(obj is ushort))
            {
                throw new ArgumentException();
            }
            return CompareTo((ushort)obj);
        }

        public int CompareTo(ushort value) { return mValue > value ? 1 : (mValue < value ? -1 : 0); }

        public bool Equals(ushort obj) { return mValue == obj; }
    }
}
