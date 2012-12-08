using System.Globalization;

namespace System
{
    public struct Single : IFormattable, IComparable, IComparable<float>, IEquatable<float>
    {
        public const float Epsilon = 1.4e-45f;
        public const float MaxValue = 3.40282346638528859e38f;
        public const float MinValue = -3.40282346638528859e38f;
        public const float NaN = 0.0f / 0.0f;
        public const float PositiveInfinity = 1.0f / 0.0f;
        public const float NegativeInfinity = -1.0f / 0.0f;

        private float mValue;

#pragma warning disable 1718
        public static bool IsNaN(float f) { return f != f; }
#pragma warning restore 1718

        public static bool IsNegativeInfinity(float f) { return (f < 0.0f && (f == NegativeInfinity || f == PositiveInfinity)); }

        public static bool IsPositiveInfinity(float f) { return (f > 0.0f && (f == NegativeInfinity || f == PositiveInfinity)); }

        public static bool IsInfinity(float f) { return (f == PositiveInfinity || f == NegativeInfinity); }

        public override bool Equals(object obj)
        {
            if (!(obj is Single))
            {
                return false;
            }
            if (IsNaN((float)obj))
            {
                return IsNaN(this);
            }
            return ((float)obj).mValue == mValue;
        }

        public unsafe override int GetHashCode()
        {
            fixed (float* ptr = &mValue) return (*(int*)ptr).GetHashCode();
        }

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
            if (!(obj is float))
            {
                throw new ArgumentException();
            }
            return CompareTo((float)obj);
        }

        public int CompareTo(float value)
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

        public bool Equals(float obj)
        {
            if (IsNaN(mValue))
            {
                return IsNaN(obj);
            }
            return mValue == obj;
        }
    }
}
