using System.Globalization;

namespace System
{
    public struct SByte : IFormattable, IComparable, IComparable<sbyte>, IEquatable<sbyte>
    {
        public const sbyte MinValue = -128;
        public const sbyte MaxValue = 127;

#pragma warning disable 0649
        private sbyte mValue;
#pragma warning restore 0649

        public override bool Equals(object obj) { return (obj is sbyte) && ((sbyte)obj).mValue == mValue; }

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
            if (!(obj is int))
            {
                throw new ArgumentException();
            }
            return CompareTo((sbyte)obj);
        }

        public int CompareTo(sbyte value) { return mValue > value ? 1 : (mValue < value ? -1 : 0); }

        public bool Equals(sbyte obj) { return mValue == obj; }
    }
}
