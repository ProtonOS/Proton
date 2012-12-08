namespace System.Globalization
{
    public class NumberFormatInfo : IFormatProvider
    {
        private static string[] defaultNativeDigits = new string[] { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

        public static NumberFormatInfo CurrentInfo { get { return CultureInfo.CurrentCulture.NumberFormat; } } 

        public static NumberFormatInfo InvariantInfo { get { return new NumberFormatInfo(); } }

        public static NumberFormatInfo GetInstance(IFormatProvider provider) { return CurrentInfo; }

        private bool mIsReadOnly;
        private int mCurrencyDecimalDigits;
        private string mCurrencyDecimalSeparator;
        private string mCurrencyGroupSeparator;
        private int[] mCurrencyGroupSizes;
        private int mCurrencyNegativePattern;
        private int mCurrencyPositivePattern;
        private string mCurrencySymbol;
        private DigitShapes mDigitSubstitution;
        private string mNaNSymbol;
        private string[] mNativeDigits;
        private string mNegativeInfinitySymbol;
        private string mNegativeSign;
        private int mNumberDecimalDigits;
        private string mNumberDecimalSeparator;
        private string mNumberGroupSeparator;
        private int[] mNumberGroupSizes;
        private int mNumberNegativePattern;
        private int mPercentDecimalDigits;
        private string mPercentDecimalSeparator;
        private string mPercentGroupSeparator;
        private int[] mPercentGroupSizes;
        private int mPercentNegativePattern;
        private int mPercentPositivePattern;
        private string mPercentSymbol;
        private string mPerMilleSymbol;
        private string mPositiveInfinitySymbol;
        private string mPositiveSign;

        public NumberFormatInfo()
        {
            // Set up defaults for invariant culture
            mIsReadOnly = true;
            mCurrencyDecimalDigits = 2;
            mCurrencyDecimalSeparator = ".";
            mCurrencyGroupSeparator = ",";
            mCurrencyGroupSizes = new int[] { 3 };
            mCurrencyNegativePattern = 0;
            mCurrencyPositivePattern = 0;
            mCurrencySymbol = "$";
            mDigitSubstitution = DigitShapes.None;
            mNaNSymbol = "NaN";
            mNativeDigits = defaultNativeDigits;
            mNegativeInfinitySymbol = "-Infinity";
            mNegativeSign = "-";
            mNumberDecimalDigits = 2;
            mNumberDecimalSeparator = ".";
            mNumberGroupSeparator = ",";
            mNumberGroupSizes = new int[] { 3 };
            mNumberNegativePattern = 1;
            mPercentDecimalDigits = 2;
            mPercentDecimalSeparator = ".";
            mPercentGroupSeparator = ",";
            mPercentGroupSizes = new int[] { 3 };
            mPercentNegativePattern = 0;
            mPercentPositivePattern = 0;
            mPercentSymbol = "%";
            mPerMilleSymbol = "\x2030";
            mPositiveInfinitySymbol = "Infinity";
            mPositiveSign = "+";
        }

        public bool IsReadOnly { get { return mIsReadOnly; } }
        public int CurrencyDecimalDigits { get { return mCurrencyDecimalDigits; } }
        public string CurrencyDecimalSeparator { get { return mCurrencyDecimalSeparator; } }
        public string CurrencyGroupSeparator { get { return mCurrencyGroupSeparator; } }
        public int[] CurrencyGroupSizes { get { return mCurrencyGroupSizes; } }
        public int CurrencyNegativePattern { get { return mCurrencyNegativePattern; } }
        public int CurrencyPositivePattern { get { return mCurrencyPositivePattern; } }
        public string CurrencySymbol { get { return mCurrencySymbol; } }
        public DigitShapes DigitSubstitution { get { return mDigitSubstitution; } }
        public string NaNSymbol { get { return mNaNSymbol; } }
        public string[] NativeDigits { get { return mNativeDigits; } }
        public string NegativeInfinitySymbol { get { return mNegativeInfinitySymbol; } }
        public string NegativeSign { get { return mNegativeSign; } }
        public int NumberDecimalDigits { get { return mNumberDecimalDigits; } }
        public string NumberDecimalSeparator { get { return mNumberDecimalSeparator; } }
        public string NumberGroupSeparator { get { return mNumberGroupSeparator; } }
        public int[] NumberGroupSizes { get { return mNumberGroupSizes; } }
        public int NumberNegativePattern { get { return mNumberNegativePattern; } }
        public int PercentDecimalDigits { get { return mPercentDecimalDigits; } }
        public string PercentDecimalSeparator { get { return mPercentDecimalSeparator; } }
        public string PercentGroupSeparator { get { return mPercentGroupSeparator; } }
        public int[] PercentGroupSizes { get { return mPercentGroupSizes; } }
        public int PercentNegativePattern { get { return mPercentNegativePattern; } }
        public int PercentPositivePattern { get { return mPercentPositivePattern; } }
        public string PercentSymbol { get { return mPercentSymbol; } }
        public string PerMilleSymbol { get { return mPerMilleSymbol; } }
        public string PositiveInfinitySymbol { get { return mPositiveInfinitySymbol; } }
        public string PositiveSign { get { return mPositiveSign; } }

        public object GetFormat(Type formatType) { throw new Exception("The method or operation is not implemented."); }
    }
}
