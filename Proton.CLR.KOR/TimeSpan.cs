using System.Text;

namespace System
{
    public struct TimeSpan : IComparable, IComparable<TimeSpan>, IEquatable<TimeSpan>
    {
        public static readonly TimeSpan MaxValue = new TimeSpan(long.MaxValue);
        public static readonly TimeSpan MinValue = new TimeSpan(long.MinValue);
        public static readonly TimeSpan Zero = new TimeSpan(0L);

        public const long TicksPerDay = 864000000000L;
        public const long TicksPerHour = 36000000000L;
        public const long TicksPerMillisecond = 10000L;
        public const long TicksPerMinute = 600000000L;
        public const long TicksPerSecond = 10000000L;

        private long mTicks;

        public TimeSpan(long ticks) { mTicks = ticks; }

        public TimeSpan(int hours, int minutes, int seconds) { mTicks = CalcTicks(0, hours, minutes, seconds, 0); }

        public TimeSpan(int days, int hours, int minutes, int seconds) { mTicks = CalcTicks(days, hours, minutes, seconds, 0); }

        public TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds) { mTicks = CalcTicks(days, hours, minutes, seconds, milliseconds); }

        private static long CalcTicks(int days, int hours, int minutes, int seconds, int millis)
        {
            return (long)days * TicksPerDay +
                   (long)hours * TicksPerHour + (long)minutes * TicksPerMinute +
                   (long)seconds * TicksPerSecond + (long)millis * TicksPerMillisecond;
        }

        public int Days { get { return (int)(mTicks / TicksPerDay); } }

        public int Hours { get { return (int)((mTicks % TicksPerDay) / TicksPerHour); } }

        public int Minutes { get { return (int)((mTicks % TicksPerHour) / TicksPerMinute); } }

        public int Seconds { get { return (int)((mTicks % TicksPerMinute) / TicksPerSecond); } }

        public int MilliSeconds { get { return (int)((mTicks % TicksPerSecond) / TicksPerMillisecond); } }

        public long Ticks { get { return mTicks; } }

        public double TotalDays { get { return ((double)mTicks) / ((double)TicksPerDay); } }

        public double TotalHours { get { return ((double)mTicks) / ((double)TicksPerHour); } }

        public double TotalMinutes { get { return ((double)mTicks) / ((double)TicksPerMinute); } }

        public double TotalSeconds { get { return ((double)mTicks) / ((double)TicksPerSecond); } }

        public double TotalMilliseconds { get { return ((double)mTicks) / ((double)TicksPerMillisecond); } }

        public TimeSpan Add(TimeSpan ts) { return new TimeSpan(mTicks + ts.mTicks); }

        public TimeSpan Subtract(TimeSpan ts) { return new TimeSpan(mTicks - ts.mTicks); }

        public TimeSpan Negate(TimeSpan ts) { return new TimeSpan(-mTicks); }

        public TimeSpan Duration() { return new TimeSpan(Math.Abs(mTicks)); }

        public static int Compare(TimeSpan t1, TimeSpan t2)
        {
            if (t1.mTicks < t2.mTicks) return -1;
            if (t1.mTicks > t2.mTicks) return 1;
            return 0;
        }

        #region IComparable Members

        public int CompareTo(object obj)
        {
            if (obj == null) return 1;
            if (!(obj is TimeSpan)) throw new ArgumentException("Object must be a TimeSpan");
            return Compare(this, (TimeSpan)obj);
        }

        #endregion

        #region IComparable<TimeSpan> Members

        public int CompareTo(TimeSpan x) { return Compare(this, x); }

        #endregion

        #region IEquatable<TimeSpan> Members

        public bool Equals(TimeSpan x) { return mTicks == x.mTicks; }

        #endregion

        public override int GetHashCode() { return mTicks.GetHashCode(); }

        public override bool Equals(object obj)
        {
            if (!(obj is TimeSpan)) return false;
            return mTicks == ((TimeSpan)obj).mTicks;
        }

        public static bool Equals(TimeSpan t1, TimeSpan t2) { return t1.mTicks == t2.mTicks; }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder(14);

            if (mTicks < 0) sb.Append('-');
            if (Days != 0)
            {
                sb.Append(Math.Abs(Days));
                sb.Append('.');
            }
            sb.AppendFormat("{0:D2}:{1:D2}:{2:D2}", Math.Abs(Hours), Math.Abs(Minutes), Math.Abs(Seconds));
            if (MilliSeconds != 0)
            {
                sb.Append('.');
                sb.AppendFormat("{0:D7}", Math.Abs(MilliSeconds) * (int)TicksPerMillisecond);
            }

            return sb.ToString();
        }

        public static TimeSpan operator +(TimeSpan t1, TimeSpan t2) { return new TimeSpan(t1.mTicks + t2.mTicks); }

        public static TimeSpan operator -(TimeSpan t1, TimeSpan t2) { return new TimeSpan(t1.mTicks - t2.mTicks); }

        public static bool operator ==(TimeSpan t1, TimeSpan t2) { return t1.mTicks == t2.mTicks; }

        public static bool operator !=(TimeSpan t1, TimeSpan t2) { return t1.mTicks != t2.mTicks; }

        public static bool operator >(TimeSpan t1, TimeSpan t2) { return t1.mTicks > t2.mTicks; }

        public static bool operator >=(TimeSpan t1, TimeSpan t2) { return t1.mTicks >= t2.mTicks; }

        public static bool operator <(TimeSpan t1, TimeSpan t2) { return t1.mTicks < t2.mTicks; }

        public static bool operator <=(TimeSpan t1, TimeSpan t2) { return t1.mTicks <= t2.mTicks; }

        public static TimeSpan operator +(TimeSpan ts) { return ts; }

        public static TimeSpan operator -(TimeSpan ts) { return new TimeSpan(-ts.mTicks); }
    }
}
