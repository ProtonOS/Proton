using System.Globalization;

namespace System
{
    public struct Int32 : IFormattable, IComparable, IComparable<int>, IEquatable<int>
    {
        public const int MaxValue = 0x7fffffff;
        public const int MinValue = -2147483648;

        private int mValue;

        public override bool Equals(object obj) { return (obj is int && ((int)obj).mValue == mValue); }

        public override int GetHashCode() { return mValue; }

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
            return CompareTo((int)obj);
        }

        public int CompareTo(int value) { return mValue > value ? 1 : (mValue < value ? -1 : 0); }

        public bool Equals(int obj) { return mValue == obj; }

        public static int Parse(string s) { return Parse(s, NumberStyles.Integer, null); }

        public static int Parse(string s, NumberStyles style) { return Parse(s, style, null); }

        public static int Parse(string s, IFormatProvider formatProvider) { return Parse(s, NumberStyles.Integer, formatProvider); }

        public static int Parse(string s, NumberStyles style, IFormatProvider formatProvider)
        {
            Exception e;
            int res;
            if (!ParseHelper.Parse(s, style, formatProvider, false, out res, out e))
            {
                throw e;
            }
            return res;
        }

        public static bool TryParse(string s, out int result) { return TryParse(s, NumberStyles.Integer, null, out result); }

        public static bool TryParse(string s, NumberStyles style, IFormatProvider format, out int result)
        {
            Exception e;
            return ParseHelper.Parse(s, style, format, true, out result, out e);
        }
    }
}
