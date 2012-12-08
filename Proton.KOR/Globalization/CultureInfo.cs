namespace System.Globalization
{
    public class CultureInfo
    {

        private static CultureInfo sInvariantCulture = null;

        public static CultureInfo GetCultureInfo(string name) { return sInvariantCulture; }

        public static CultureInfo CurrentCulture { get { return InvariantCulture; } }

        public static CultureInfo InvariantCulture
        {
            get
            {
                if (sInvariantCulture == null) sInvariantCulture = new CultureInfo();
                return sInvariantCulture;
            }
        }

        public static CultureInfo[] GetCultures(CultureTypes types) { return new CultureInfo[] { sInvariantCulture }; }

        private string mName;
        private int mLCID;
        private string mParentName;
        private string mDisplayName;
        private string mEnglishName;
        private string mNativeName;
        private string mTwoLetterISOLanguageName;
        private string mThreeLetterISOLanguageName;
        private string mThreeLetterWindowsLanguageName;
        private CultureTypes mCultureTypes;
        private string mIETFLanguageTag;
        private bool mIsNeutralCulture;
        private NumberFormatInfo mNumberFormatInfo;
        private TextInfo mTextInfo;
        private DateTimeFormatInfo mDateTimeFormat;

        internal CultureInfo()
        {
            mName = "en-US";
            mLCID = 0x7f;
            mParentName = mDisplayName = mEnglishName = mNativeName = "English";
            mTwoLetterISOLanguageName = "en";
            mThreeLetterISOLanguageName = "eng";
            mThreeLetterWindowsLanguageName = "ENU";
            mCultureTypes = Globalization.CultureTypes.AllCultures;
            mIETFLanguageTag = "en";
            mIsNeutralCulture = true;
            mNumberFormatInfo = NumberFormatInfo.InvariantInfo;
            mTextInfo = TextInfo.InvariantInfo;
            mDateTimeFormat = DateTimeFormatInfo.InvariantInfo;
        }

        public bool IsReadOnly { get { return true; } }

        public virtual NumberFormatInfo NumberFormat { get { return mNumberFormatInfo; } }

        public virtual DateTimeFormatInfo DateTimeFormat { get { return mDateTimeFormat; } }

        public virtual int LCID { get { return mLCID; } }

        public virtual string Name { get { return mName; } }

        public virtual CultureInfo Parent { get { return GetCultureInfo(mParentName); } }

        public virtual string DisplayName { get { return mDisplayName; } }

        public virtual string EnglishName { get { return mEnglishName; } }

        public virtual string NativeName { get { return mNativeName; } }

        public virtual string TwoLetterISOLanguageName { get { return mTwoLetterISOLanguageName; } }

        public virtual string ThreeLetterISOLanguageName { get { return mThreeLetterISOLanguageName; } }

        public virtual string ThreeLetterWindowsLanguageName { get { return mThreeLetterWindowsLanguageName; } }

        public virtual CultureTypes CultureTypes { get { return mCultureTypes; } }

        public virtual string IetfLanguageTag { get { return mIETFLanguageTag; } }

        public virtual bool IsNeutralCulture { get { return mIsNeutralCulture; } }

        public virtual TextInfo TextInfo { get { return mTextInfo; } }

        public override string ToString() { return mName; }

    }
}
