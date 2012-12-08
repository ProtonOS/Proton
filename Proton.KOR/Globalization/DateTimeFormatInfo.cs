namespace System.Globalization
{
    public sealed class DateTimeFormatInfo
    {
        public static DateTimeFormatInfo InvariantInfo
        {
            get
            {
                DateTimeFormatInfo dtfi = new DateTimeFormatInfo();
                dtfi.mIsReadOnly = true;
                return dtfi;
            }
        }

        public static DateTimeFormatInfo CurrentInfo
        {
            get
            {
                return CultureInfo.CurrentCulture.DateTimeFormat;
            }
        }

        public static DateTimeFormatInfo GetInstance(IFormatProvider fp)
        {
            return CurrentInfo;
        }

        private bool mIsReadOnly;
        private string[] mAbbreviatedDayNames;
        private string[] mAbbreviatedMonthGenitiveNames;
        private string[] mAbbreviatedMonthNames;
        private string mAMDesignator;
        private CalendarWeekRule mCalendarWeekRule;
        private string mDateSeparator;
        private string[] mDayNames;
        private DayOfWeek mFirstDayOfWeek;
        private string mFullDateTimePattern;
        private string mLongDatePattern;
        private string mLongTimePattern;
        private string mMonthDayPattern;
        private string[] mMonthGenitiveNames;
        private string[] mMonthNames;
        private string mNativeCalendarName; // This probably needs to change
        private string mPMDesignator;
        private string mRFC1123Pattern;
        private string mShortDatePattern;
        private string[] mShortestDayNames;
        private string mShortTimePattern;
        private string mSortableDateTimePattern;
        private string mTimeSeparator;
        private string mUniversalSortableDateTimePattern;
        private string mYearMonthPattern;
        private Calendar mCalendar;

        public DateTimeFormatInfo()
        {
            // Construct an invariant culture DateTimeFormatInfo
            char[] comma = new char[] { ',' };
            mIsReadOnly = true;
            mAbbreviatedDayNames = "Sun,Mon,Tue,Wed,Thu,Fri,Sat".Split(comma);
            mAbbreviatedMonthGenitiveNames = "Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec,".Split(comma);
            mAbbreviatedMonthNames = "Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec,".Split(comma);
            mAMDesignator = "AM";
            mCalendarWeekRule = CalendarWeekRule.FirstDay;
            mDateSeparator = "/";
            mDayNames = "Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday".Split(comma);
            mFirstDayOfWeek = DayOfWeek.Sunday;
            mFullDateTimePattern = "dddd, dd MMMM yyyy HH:mm:ss";
            mLongDatePattern = "dddd, dd MMMM yyyy";
            mLongTimePattern = "HH:mm:ss";
            mMonthDayPattern = "MMMM dd";
            mMonthGenitiveNames = "January,February,March,April,May,June,July,August,September,October,November,December,".Split(comma);
            mMonthNames = "January,February,March,April,May,June,July,August,September,October,November,December,".Split(comma);
            mNativeCalendarName = "Gregorian Calendar";
            mPMDesignator = "PM";
            mRFC1123Pattern = "ddd, dd MMM yyyy HH':'mm':'ss 'GMT'";
            mShortDatePattern = "MM/dd/yyyy";
            mShortestDayNames = "Su,Mo,Tu,We,Th,Fr,Sa".Split(comma);
            mShortTimePattern = "HH:mm";
            mSortableDateTimePattern = "yyyy'-'MM'-'dd'T'HH':'mm':'ss";
            mTimeSeparator = ":";
            mUniversalSortableDateTimePattern = "yyyy'-'MM'-'dd HH':'mm':'ss'Z'";
            mYearMonthPattern = "yyyy MMMM";
            mCalendar = new GregorianCalendar();
        }

        public bool IsReadOnly { get { return mIsReadOnly; } }

        public string[] AbbreviatedDayNames { get { return mAbbreviatedDayNames; } }

        public string[] AbbreviatedMonthGenitiveNames { get { return mAbbreviatedMonthGenitiveNames; } }

        public string[] AbbreviatedMonthNames { get { return mAbbreviatedMonthNames; } }

        public string AMDesignator { get { return mAMDesignator; } }

        public CalendarWeekRule CalendarWeekRule { get { return mCalendarWeekRule; } }

        public string DateSeparator { get { return mDateSeparator; } }

        public string[] DayNames { get { return mDayNames; } }

        public DayOfWeek FirstDayOfWeek { get { return mFirstDayOfWeek; } }

        public string FullDateTimePattern { get { return mFullDateTimePattern; } }

        public string LongDatePattern { get { return mLongDatePattern; } }

        public string LongTimePattern { get { return mLongTimePattern; } }

        public string MonthDayPattern { get { return mMonthDayPattern; } }

        public string[] MonthGenitiveNames { get { return mMonthGenitiveNames; } }

        public string[] MonthNames { get { return mMonthNames; } }

        public string NativeCalendarName { get { return mNativeCalendarName; } }

        public string PMDesignator { get { return mPMDesignator; } }

        public string RFC1123Pattern { get { return mRFC1123Pattern; } }

        public string ShortDatePattern { get { return mShortDatePattern; } }

        public string[] ShortestDayNames { get { return mShortestDayNames; } }

        public string ShortTimePattern { get { return mShortTimePattern; } }

        public string SortableDateTimePattern { get { return mSortableDateTimePattern; } }

        public string TimeSeparator { get { return mTimeSeparator; } }

        public string UniversalSortableDateTimePattern { get { return mUniversalSortableDateTimePattern; } }

        public string YearMonthPattern { get { return mYearMonthPattern; } }

        public Calendar Calendar { get { return mCalendar; } }

        public string GetAbbreviatedDayName(DayOfWeek dow) { return mAbbreviatedDayNames[(int)dow]; }

        public string GetAbbreviatedMonthName(int m) { return mAbbreviatedMonthNames[m]; }

        public string GetDayName(DayOfWeek dow) { return mDayNames[(int)dow]; }

        public string GetEraName(int era) { return mCalendar.mEraNames[era - 1]; }

        public string GetMonthName(int m) { return mMonthNames[m]; }
    }
}
