namespace System
{
    public struct Boolean : IComparable, IComparable<bool>, IEquatable<bool>
    {
        public static readonly string TrueString = "True";
        public static readonly string FalseString = "False";

        private bool mValue;

        public override string ToString() { return mValue ? TrueString : FalseString; }

        public override bool Equals(object obj) { return (obj is bool) && ((bool)obj).mValue == mValue; }

        public override int GetHashCode() { return mValue ? 1 : 0; }

        public static bool Parse(string value)
        {
            if (value == null)
            {
                throw new ArgumentNullException("value");
            }
            value = value.Trim();
            if (value == TrueString)
            {
                return true;
            }
            if (value == FalseString)
            {
                return false;
            }
            throw new FormatException("Value is not a valid boolean");
        }

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
            return CompareTo((bool)obj);
        }

        public int CompareTo(bool value) { return mValue == value ? 0 : (mValue ? 1 : -1); }

        public bool Equals(bool obj) { return mValue == obj; }
    }
}
