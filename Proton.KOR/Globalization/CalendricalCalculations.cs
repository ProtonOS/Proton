namespace System.Globalization
{
    /// <summary>The class implements methods to handle the fixed date value from
    /// the "Calendrical Calculations" books.
    /// </summary>
    /// <remarks>
    /// <para>
    /// For implementing the Calendar classes I used the algorithms from the
    /// book "Calendrical Calculations" by Nachum Dershowitz and Edward M.
    /// Rheingold, second reprint 1998. Trying to prevent the introduction of new
    /// bugs, I implemented their algorithms in the
    /// <see cref="N:CalendricalCalculations"/>
    /// namespace and wrapped it in the calendar classes.
    /// </para>
    /// <para>
    /// The fixed day number is also known as R.D. - rata die.
    /// Midnight at the onset of Monday,
    /// January 1, year 1 (Gregorian) is R.D. 1.
    /// </para>
    /// <para>Here are all my references:</para>
    /// <list type="table">
    /// <item><description>
    /// [1] Nachum Dershowitz and Edward M. Rheingold: "Calendrical Calculations";
    /// Cambridge University Press; second reprint 1998.
    /// </description></item>
    /// <item><description>
    /// [2] P. Kenneth Seidelmann (ed.): "Explanatory Supplement to the Astronomical
    /// Almanac"; University Science Books, Sausalito; 1992 
    /// </description></item>
    /// <item><description>
    /// [3] F. Richard Stephenson: "Historical Eclipses and Earth Rotation";
    /// Cambridge University Press; 1997
    /// </description></item>
    /// </list>
    /// </remarks>
    internal class CCFixed
    {
        /// <summary>The method computes the
        /// <see cref="T:DateTime"/>
        /// from a fixed day number.
        /// </summary>
        /// <param name="date">A integer representing the fixed day number.
        /// </param>
        /// <returns>The <see cref="T:DateTime"/> representing
        /// the date.
        /// </returns>
        public static DateTime ToDateTime(int date)
        {
            long ticks = (date - 1) * TimeSpan.TicksPerDay;
            return new DateTime(ticks);
        }

        /// <summary>The method computes the
        /// <see cref="T:DateTime"/>
        /// from a fixed day number and time arguments.
        /// </summary>
        /// <param name="date">An integer representing the fixed day number.
        /// </param>
        /// <param name="hour">An integer argument specifying the hour.
        /// </param>
        /// <param name="minute">An integer argument specifying the minute.
        /// </param>
        /// <param name="second">An integer argument giving the second.
        /// </param>
        /// <param name="milliseconds">An double argument specifying
        /// the milliseconds. Notice that
        /// <see cref="T:DateTime"/> has 100 nanosecond resolution.
        /// </param>
        /// <returns>The <see cref="T:DateTime"/> representing
        /// the date.
        /// </returns>
        public static DateTime ToDateTime(int date,
            int hour, int minute, int second, double milliseconds)
        {
            DateTime time = ToDateTime(date);
            time = time.AddHours(hour);
            time = time.AddMinutes(minute);
            time = time.AddSeconds(second);
            return time.AddMilliseconds(milliseconds);
        }

        /// <summary>
        /// A static method computing the fixed day number from a 
        /// <see cref="T:DateTime"/> value.
        /// </summary>
        /// <param name="time">A
        /// <see cref="T:DateTime"/> value representing the date.
        /// </param>
        /// <returns>The fixed day number as integer representing the date.
        /// </returns>
        public static int FromDateTime(DateTime time)
        {
            return 1 + (int)(time.Ticks / TimeSpan.TicksPerDay);
        }

        /// <summary>
        /// The static method computes the <see cref="T:DayOfWeek"/>.
        /// </summary>
        /// <param name="date">An integer representing the fixed day number.
        /// </param>
        /// <returns>The day of week.</returns>
        public static DayOfWeek day_of_week(int date)
        {
            return (DayOfWeek)(date % 7);//CCMath.mod(date, 7);
        }

        /// <summary>
        /// The static method computes the date of a day of week on or before
        /// a particular date.
        /// </summary>
        /// <param name="date">An integer representing the date as
        /// fixed day number.
        /// </param>
        /// <param name="k">An integer representing the day of the week,
        /// starting with 0 for sunday.
        /// </param>
        /// <returns>The fixed day number of the day of week specified by k
        /// on or before the given date.
        /// </returns>
        public static int kday_on_or_before(int date, int k)
        {
            return date - (int)day_of_week(date - k);
        }

        /// <summary>
        /// The static method computes the date of a day of week on or after
        /// a particular date.
        /// </summary>
        /// <param name="date">An integer representing the date as
        /// fixed day number.
        /// </param>
        /// <param name="k">An integer representing the day of the week,
        /// starting with 0 for sunday.
        /// </param>
        /// <returns>The fixed day number of the day of week specified by k
        /// on or after the given date.
        /// </returns>
        public static int kday_on_or_after(int date, int k)
        {
            return kday_on_or_before(date + 6, k);
        }

        /// <summary>
        /// The static method computes the date of a day of week that is
        /// nearest to a particular date.
        /// </summary>
        /// <param name="date">An integer representing the date as
        /// fixed day number.
        /// </param>
        /// <param name="k">An integer representing the day of the week,
        /// starting with 0 for sunday.
        /// </param>
        /// <returns>The fixed day number of the day of week neares to the
        /// given date.
        /// </returns>
        public static int kd_nearest(int date, int k)
        {
            return kday_on_or_before(date + 3, k);
        }

        /// <summary>
        /// The static method computes the date of a day of week after
        /// a particular date.
        /// </summary>
        /// <param name="date">An integer representing the date as
        /// fixed day number.
        /// </param>
        /// <param name="k">An integer representing the day of the week,
        /// starting with 0 for sunday.
        /// </param>
        /// <returns>The fixed day number of the day of week specified by k
        /// after the given date.
        /// </returns>
        public static int kday_after(int date, int k)
        {
            return kday_on_or_before(date + 7, k);
        }

        /// <summary>
        /// The static method computes the date of a day of week before
        /// a particular date.
        /// </summary>
        /// <param name="date">An integer representing the date as
        /// fixed day number.
        /// </param>
        /// <param name="k">An integer representing the day of the week,
        /// starting with 0 for sunday.
        /// </param>
        /// <returns>The fixed day number of the day of week specified by k
        /// before the given date.
        /// </returns>
        public static int kday_before(int date, int k)
        {
            return kday_on_or_before(date - 1, k);
        }
    } // class CCFixed


    /// <summary>
    /// A class encapsulating the functions of the Gregorian calendar as static
    /// methods.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This class is not compatible to
    /// <see cref="T:Globalization.GregorianCalendar"/>.
    /// </para>
    /// <para>
    /// The fixed day number is also known as R.D. - rata die.
    /// Midnight at the onset of Monday,
    /// January 1, year 1 (Gregorian) is R.D. 1.
    /// </para>
    /// <seealso cref="T:CCFixed"/>
    /// </remarks>
    internal class CCGregorianCalendar
    {
        /// <summary>An integer defining the epoch of the Gregorian calendar
        /// as fixed day number.</summary>
        /// <remarks>The epoch is January 3, 1 C.E. (Julian).</remarks>
        const int epoch = 1;

        /// <summary>The enumeration defines the months of the Gregorian
        /// calendar.
        /// </summary>
        public enum Month
        {
            /// <summary>
            /// January.
            /// </summary>
            january = 1,
            /// <summary>
            /// February.
            /// </summary>
            february,
            /// <summary>
            /// March.
            /// </summary>
            march,
            /// <summary>
            /// April.
            /// </summary>
            april,
            /// <summary>
            /// May.
            /// </summary>
            may,
            /// <summary>
            /// June.
            /// </summary>
            june,
            /// <summary>
            /// July.
            /// </summary>
            july,
            /// <summary>
            /// August.
            /// </summary>
            august,
            /// <summary>
            /// September.
            /// </summary>
            september,
            /// <summary>
            /// October.
            /// </summary>
            october,
            /// <summary>
            /// November.
            /// </summary>
            november,
            /// <summary>
            /// December.
            /// </summary>
            december
        };


        /// <summary>
        /// The method tells whether the year is a leap year.
        /// </summary>
        /// <param name="year">An integer representing the Gregorian year.
        /// </param>
        /// <returns>A boolean which is true if <paramref name="year"/> is
        /// a leap year.
        /// </returns>
        public static bool is_leap_year(int year)
        {
            if (year % 4 != 0)
                return false;
            switch (year % 400)
            {
                case 100:
                    return false;
                case 200:
                    return false;
                case 300:
                    return false;
            }
            return true;
        }

        /// <summary>
        /// The method returns the fixed day number of the given Gregorian
        /// date.
        /// </summary>
        /// <param name="day">An integer representing the day of the month,
        /// counting from 1.
        /// </param>
        /// <param name="month">An integer representing the month in the
        /// Gregorian year.
        /// </param>
        /// <param name="year">An integer representing the Gregorian year.
        /// Non-positive values are allowed also.
        /// </param>
        /// <returns>An integer value representing the fixed day number.
        /// </returns>
        public static int fixed_from_dmy(int day, int month, int year)
        {
            int k = epoch - 1;
            k += 365 * (year - 1);
            //k += CCMath.div(year-1, 4);
            //k -= CCMath.div(year-1, 100);
            //k += CCMath.div(year-1, 400);
            //k += CCMath.div(367*month-362, 12);
            k += (year - 1) / 4;
            k -= (year - 1) / 100;
            k += (year - 1) / 400;
            k += (367 * month - 362) / 12;
            if (month > 2)
            {
                k -= is_leap_year(year) ? 1 : 2;
            }

            k += day;

            return k;
        }

        /// <summary>
        /// The method computes the Gregorian year from a fixed day number.
        /// </summary>
        /// <param name="date">The fixed day number.
        /// </param>
        /// <returns>An integer value giving the Gregorian year of the date.
        /// </returns>
        public static int year_from_fixed(int date)
        {
            int d = date - epoch;
            int n_400 = d / 146097;
            d %= 146097;
            int n_100 = d / 36524;
            d %= 36524;
            int n_4 = d / 1461;
            d %= 1461;
            int n_1 = d / 365;
            //int n_400	= CCMath.div_mod(out d, d, 146097);
            //int n_100	= CCMath.div_mod(out d, d, 36524);
            //int n_4		= CCMath.div_mod(out d, d, 1461);
            //int n_1		= CCMath.div(d, 365);

            int year = 400 * n_400 + 100 * n_100 + 4 * n_4 + n_1;
            return (n_100 == 4 || n_1 == 4) ? year : year + 1;
        }

        /// <summary>
        /// The method computes the Gregorian year and month from a fixed day
        /// number.
        /// </summary>
        /// <param name="month">The output value giving the Gregorian month.
        /// </param>
        /// <param name="year">The output value giving the Gregorian year.
        /// </param>
        /// <param name="date">An integer value specifying the fixed day
        /// number.</param>
        public static void my_from_fixed(out int month, out int year,
            int date)
        {
            year = year_from_fixed(date);

            int prior_days = date - fixed_from_dmy(1, (int)Month.january,
                year);

            int correction;
            if (date < fixed_from_dmy(1, (int)Month.march, year))
            {
                correction = 0;
            }
            else if (is_leap_year(year))
            {
                correction = 1;
            }
            else
            {
                correction = 2;
            }

            //month = CCMath.div(12 * (prior_days + correction) + 373, 367);
            month = (12 * (prior_days + correction) + 373) / 367;

        }

        /// <summary>
        /// The method computes the Gregorian year, month, and day from a
        /// fixed day number.
        /// </summary>
        /// <param name="day">The output value returning the day of the
        /// month.
        /// </param>
        /// <param name="month">The output value giving the Gregorian month.
        /// </param>
        /// <param name="year">The output value giving the Gregorian year.
        /// </param>
        /// <param name="date">An integer value specifying the fixed day
        /// number.</param>
        public static void dmy_from_fixed(out int day, out int month,
            out int year,
            int date)
        {
            my_from_fixed(out month, out year, date);
            day = date - fixed_from_dmy(1, month, year) + 1;
        }

        /// <summary>A method computing the Gregorian month from a fixed
        /// day number.
        /// </summary>
        /// <param name="date">An integer specifying the fixed day number.
        /// </param>
        /// <returns>An integer value representing the Gregorian month.
        /// </returns>
        public static int month_from_fixed(int date)
        {
            int month, year;

            my_from_fixed(out month, out year, date);
            return month;
        }

        /// <summary>
        /// A method computing the day of the month from a fixed day number.
        /// </summary>
        /// <param name="date">An integer specifying the fixed day number.
        /// </param>
        /// <returns>An integer value representing the day of the month.
        /// </returns>
        public static int day_from_fixed(int date)
        {
            int day, month, year;

            dmy_from_fixed(out day, out month, out year, date);
            return day;
        }

        /// <summary>
        /// The method computes the difference between two Gregorian dates.
        /// </summary>
        /// <param name="dayA">The integer parameter gives the day of month
        /// of the first date.
        /// </param>
        /// <param name="monthA">The integer parameter gives the Gregorian
        /// month of the first date.
        /// </param>
        /// <param name="yearA">The integer parameter gives the Gregorian
        /// year of the first date.
        /// </param>
        /// <param name="dayB">The integer parameter gives the day of month
        /// of the second date.
        /// </param>
        /// <param name="monthB">The integer parameter gives the Gregorian
        /// month of the second date.
        /// </param>
        /// <param name="yearB">The integer parameter gives the Gregorian
        /// year of the second date.
        /// </param>
        /// <returns>An integer giving the difference of days from the first
        /// the second date.
        /// </returns>
        public static int date_difference(int dayA, int monthA, int yearA,
            int dayB, int monthB, int yearB)
        {
            return fixed_from_dmy(dayB, monthB, yearB) -
                fixed_from_dmy(dayA, monthA, yearA);
        }

        /// <summary>
        /// The method computes the number of the day in the year from
        /// a Gregorian date.
        /// </summary>
        /// <param name="day">An integer representing the day of the month,
        /// counting from 1.
        /// </param>
        /// <param name="month">An integer representing the month in the
        /// Gregorian year.
        /// </param>
        /// <param name="year">An integer representing the Gregorian year.
        /// Non-positive values are allowed also.
        /// </param>
        /// <returns>An integer value giving the number of the day in the
        /// Gregorian year, counting from 1.
        /// </returns>
        public static int day_number(int day, int month, int year)
        {
            return date_difference(31, (int)Month.december, year - 1,
                day, month, year);
        }

        /// <summary>
        /// The method computes the days remaining in the given Gregorian
        /// year from a Gregorian date.
        /// </summary>
        /// <param name="day">An integer representing the day of the month,
        /// counting from 1.
        /// </param>
        /// <param name="month">An integer representing the month in the
        /// Gregorian year.
        /// </param>
        /// <param name="year">An integer representing the Gregorian year.
        /// Non-positive values are allowed also.
        /// </param>
        /// <returns>An integer value giving the number of days remaining in
        /// the Gregorian year.
        /// </returns>
        public static int days_remaining(int day, int month, int year)
        {
            return date_difference(day, month, year,
                31, (int)Month.december, year);
        }

        // Helper functions for the Gregorian calendars.

        /// <summary>
        /// Adds months to the given date.
        /// </summary>
        /// <param name="time">The
        /// <see cref="T:DateTime"/> to which to add
        /// months.
        /// </param>
        /// <param name="months">The number of months to add.</param>
        /// <returns>A new <see cref="T:DateTime"/> value, that
        /// results from adding <paramref name="months"/> to the specified
        /// DateTime.</returns>
        public static DateTime AddMonths(DateTime time,
            int months)
        {
            int rd = CCFixed.FromDateTime(time);
            int day, month, year;
            dmy_from_fixed(out day, out month, out year, rd);
            month += months;
            int maxday = GetDaysInMonth(year, month);
            if (day > maxday)
                day = maxday;
            rd = fixed_from_dmy(day, month, year);
            DateTime t = CCFixed.ToDateTime(rd);
            return t.Add(time.TimeOfDay);
        }

        /// <summary>
        /// Adds years to the given date.
        /// </summary>
        /// <param name="time">The
        /// <see cref="T:DateTime"/> to which to add
        /// months.
        /// </param>
        /// <param name="years">The number of years to add.</param>
        /// <returns>A new <see cref="T:DateTime"/> value, that
        /// results from adding <paramref name="years"/> to the specified
        /// DateTime.</returns>
        public static DateTime AddYears(DateTime time,
            int years)
        {
            int rd = CCFixed.FromDateTime(time);
            int day, month, year;
            dmy_from_fixed(out day, out month, out year, rd);
            year += years;
            int maxday = GetDaysInMonth(year, month);
            if (day > maxday)
                day = maxday;
            rd = fixed_from_dmy(day, month, year);
            DateTime t = CCFixed.ToDateTime(rd);
            return t.Add(time.TimeOfDay);
        }

        /// <summary>
        /// Gets the of the month from <paramref name="time"/>.
        /// </summary>
        /// <param name="time">The
        /// <see cref="T:DateTime"/> that specifies a
        /// date.
        /// </param>
        /// <returns>An integer giving the day of months, starting with 1.
        /// </returns>
        public static int GetDayOfMonth(DateTime time)
        {
            return day_from_fixed(CCFixed.FromDateTime(time));
        }

        /// <summary>
        /// The method gives the number of the day in the year.
        /// </summary>
        /// <param name="time">The
        /// <see cref="T:DateTime"/> that specifies a
        /// date.
        /// </param>
        /// <returns>An integer representing the day of the year,
        /// starting with 1.</returns>
        public static int GetDayOfYear(DateTime time)
        {
            int rd = CCFixed.FromDateTime(time);
            int year = year_from_fixed(rd);
            int rd1_1 = fixed_from_dmy(1, 1, year);
            return rd - rd1_1 + 1;
        }

        /// <summary>
        /// A method that gives the number of days of the specified
        /// month of the <paramref name="year"/>.
        /// </summary>
        /// <param name="year">An integer that gives the year in the current
        /// era.</param>
        /// <param name="month">An integer that gives the month, starting
        /// with 1.</param>
        /// <returns>An integer that gives the number of days of the
        /// specified month.</returns>
        public static int GetDaysInMonth(int year, int month)
        {
            int rd1 = fixed_from_dmy(1, month, year);
            int rd2 = fixed_from_dmy(1, month + 1, year);
            return rd2 - rd1;
        }

        /// <summary>
        /// The method gives the number of days in the specified year.
        /// </summary>
        /// <param name="year">An integer that gives the year.
        /// </param>
        /// <returns>An integer that gives the number of days of the
        /// specified year.</returns>
        public static int GetDaysInYear(int year)
        {
            int rd1 = fixed_from_dmy(1, 1, year);
            int rd2 = fixed_from_dmy(1, 1, year + 1);
            return rd2 - rd1;
        }

        /// <summary>
        /// The method gives the number of the month of the specified
        /// date.
        /// </summary>
        /// <param name="time">The
        /// <see cref="T:DateTime"/> that specifies a
        /// date.
        /// </param>
        /// <returns>An integer representing the month, 
        /// starting with 1.</returns>
        public static int GetMonth(DateTime time)
        {
            return month_from_fixed(CCFixed.FromDateTime(time));
        }

        /// <summary>
        /// The method gives the number of the year of the specified
        /// date.
        /// </summary>
        /// <param name="time">The
        /// <see cref="T:DateTime"/> that specifies a
        /// date.
        /// </param>
        /// <returns>An integer representing the year. 
        /// </returns>
        public static int GetYear(DateTime time)
        {
            return year_from_fixed(CCFixed.FromDateTime(time));
        }

        /// <summary>
        /// A virtual method that tells whether the given day
        /// is a leap day.
        /// </summary>
        /// <param name="year">An integer that specifies the year.
        /// </param>
        /// <param name="month">An integer that specifies the month.
        /// </param>
        /// <param name="day">An integer that specifies the day.
        /// </param>
        /// <returns>A boolean that tells whether the given day is a leap
        /// day.
        /// </returns>
        public static bool IsLeapDay(int year, int month, int day)
        {
            return is_leap_year(year) && month == 2 && day == 29;
        }

        /// <summary>
        /// A method that creates the
        /// <see cref="T:DateTime"/> from the parameters.
        /// </summary>
        /// <param name="year">An integer that gives the year
        /// </param>
        /// <param name="month">An integer that specifies the month.
        /// </param>
        /// <param name="day">An integer that specifies the day.
        /// </param>
        /// <param name="hour">An integer that specifies the hour.
        /// </param>
        /// <param name="minute">An integer that specifies the minute.
        /// </param>
        /// <param name="second">An integer that gives the second.
        /// </param>
        /// <param name="milliseconds">An integer that gives the
        /// milliseconds.
        /// </param>
        /// <returns>A
        /// <see cref="T:system.DateTime"/> representig the date and time.
        /// </returns>
        public static DateTime ToDateTime(int year, int month, int day,
            int hour, int minute, int second, int milliseconds)
        {
            return CCFixed.ToDateTime(fixed_from_dmy(day, month, year),
                hour, minute, second, milliseconds);
        }
    }
}
