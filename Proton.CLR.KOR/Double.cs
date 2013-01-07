using System.Globalization;

namespace System
{
    public struct Double : IFormattable, IComparable, IComparable<double>, IEquatable<double>
    {
        public const double Epsilon = 4.9406564584124650e-324;
        public const double MaxValue = 1.7976931348623157e308;
        public const double MinValue = -1.7976931348623157e308;
        public const double NaN = 0.0d / 0.0d;
        public const double NegativeInfinity = -1.0d / 0.0d;
        public const double PositiveInfinity = 1.0d / 0.0d;

        private double mValue;

#pragma warning disable 1718
        public static bool IsNaN(double d) { return d != d; }
#pragma warning restore 1718

        public static bool IsNegativeInfinity(double d) { return (d < 0.0d && (d == NegativeInfinity || d == PositiveInfinity)); }

        public static bool IsPositiveInfinity(double d) { return (d > 0.0d && (d == NegativeInfinity || d == PositiveInfinity)); }

        public static bool IsInfinity(double d) { return (d == PositiveInfinity || d == NegativeInfinity); }

        public override bool Equals(object obj)
        {
            if (!(obj is Double))
            {
                return false;
            }
            if (IsNaN((double)obj))
            {
                return IsNaN(mValue);
            }
            return ((double)obj).mValue == mValue;
        }

        public unsafe override int GetHashCode() { fixed (double* ptr = &mValue) return (*(long*)ptr).GetHashCode(); }

        public override string ToString() { return ToString(null, null); }

        public string ToString(IFormatProvider provider) { return ToString(null, provider); }

        public string ToString(string format) { return ToString(format, null); }

        public string ToString(string format, IFormatProvider provider) { return NumberFormatter.NumberToString(format, mValue, NumberFormatInfo.GetInstance(provider)); }

        public int CompareTo(object obj)
        {
            if (obj == null)
            {
                return 1;
            }
            if (!(obj is double))
            {
                throw new ArgumentException();
            }
            return CompareTo((double)obj);
        }

        public int CompareTo(double value)
        {
            if (IsNaN(mValue))
            {
                return IsNaN(value) ? 0 : -1;
            }
            if (IsNaN(value))
            {
                return 1;
            }
            return mValue > value ? 1 : (mValue < value ? -1 : 0);
        }

        public bool Equals(double obj)
        {
            if (IsNaN(mValue))
            {
                return IsNaN(obj);
            }
            return mValue == obj;
        }
    }
}
