package java.util;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class GregorianCalendar
  extends Calendar
{
	final private static long serialVersionUID = -8125100834729963327L;
	final public static int BC = 0;
	final public static int AD = 1;
	final private static long MILLISECSPERSEC = 1000L;
        private static final long SECSPERMIN = 60L;
	final private static long MILLISECSPERMIN = MILLISECSPERSEC * SECSPERMIN;
        private static final long MINSPERHOUR = 60L;
	final private static long MILLISECSPERHOUR = MILLISECSPERMIN * MINSPERHOUR;
        private static final long HOURSPERDAY = 24L;
	final private static long MILLISECSPERDAY = MILLISECSPERHOUR * HOURSPERDAY;
	final private static int DAYSPERWEEK = 7;
        private static final int MONTHSPERYEAR = 12;
	final private static int EPOCH_WDAY = THURSDAY - SUNDAY;
	final private static int EPOCH_YEAR = 1970;
        /* Used to initialize Calendars used as counters. */
        private static final SimpleTimeZone simple_zone = new SimpleTimeZone(0, "_temp");
        /* Initialize the day that time switched from Julian
	 *  to Gregorian date keeping as 1582, October 15th.
	 */
	final private static Date stdDateChange = new Date(-12219292800000L);
	private Date dateChange = stdDateChange;
	private int dateChangeYear = 1582;

public GregorianCalendar() {
	this(TimeZone.getDefault(), Locale.getDefault());
}

public GregorianCalendar(Locale aLocale) {
	this(TimeZone.getDefault(), aLocale);
}

public GregorianCalendar(TimeZone zone) {
	this(zone, Locale.getDefault());
}

public GregorianCalendar(TimeZone zone, Locale aLocale) {
	super(zone, aLocale);
	setTime(new Date());
}

public GregorianCalendar(int year, int month, int date) {
	this(year, month, date, 0, 0);
}

public GregorianCalendar(int year, int month, int date, int hour, int minute) {
	this(year, month, date, hour, minute, 0);
}

public GregorianCalendar(int year, int month, int date, int hour, int minute, int second) {
	super();
	set(year, month, date, hour, minute, second);
}

public void add(int field, int amount) {
	if (field == ZONE_OFFSET || field == DST_OFFSET) {
		throw new IllegalArgumentException("bad field: " + field);
	}
	if (!isSet(field)) {
		computeFields();
	}
	fields[field] += amount;
	computeTime();
	computeFields();
}

/* check if all fields are within range if calendar is lenient */
private void checkFields() {
    if (!isLenient()) {
	for (int field = 0; field < FIELD_COUNT; ++ field ) {
	    int value = internalGet(field);

	    if (value < getMinimum(field) || value > getMaximum(field)) {
		throw new IllegalArgumentException("value " + value
						   + " out of range for field " + field
						   + " [ " + getMinimum(field) + ", "
						   + getMaximum(field) + " ]");
	    }
	}
    }
}

/* used by get*Counter methods to make sure the counter is always
 * set up in the same way
 */
private void completeCounter(GregorianCalendar counter) {
    counter.setTimeZone(simple_zone);
    counter.complete();
}

// Calculate the date & time
private long computeDateTime() {
	checkFields();

	long time = 0L;
	boolean up = ! (isSet(YEAR) && internalGet(YEAR) < EPOCH_YEAR);

	if (isSet(YEAR)) {
	    time += computeDateTimeForYear(up);
	}

	if (isSet(MONTH)) {
	    time +=computeDateTimeForMonth(up);
	    if (isSet(DAY_OF_MONTH)) {
		time += computeDateTimeForDayOfMonth(up);
	    }
	    else if (isSet(WEEK_OF_MONTH) && isSet(DAY_OF_WEEK)) {
		time += computeDateTimeForWeekOfMonthAndDayOfWeek(up);
	    }
	    else if (isSet(DAY_OF_WEEK_IN_MONTH) && isSet(DAY_OF_WEEK)) {
		time += computeDateTimeForDayOfWeekInMonthAndDayOfWeek(up);
	    }
	}
	else if (isSet(DAY_OF_YEAR)) {
	    time += computeDateTimeForDayOfYear(up);
	}
	else if (isSet(DAY_OF_WEEK) && isSet(WEEK_OF_YEAR)) {
	    time += computeDateTimeForWeekOfYearAndDayOfWeek(up);
	}

	time *= HOURSPERDAY;

	if (isSet(HOUR_OF_DAY)) {
	    time += computeDateTimeForHourOfDay(up);
	}
	else if (isSet(AM_PM) && isSet(HOUR)) {
	    time += computeDateTimeForAMPMAndHour(up);
	}
	time *= MINSPERHOUR;

	if (isSet(MINUTE)) {
	    time += computeDateTimeForMinute(up);
	}
	time *= SECSPERMIN;

	if (isSet(SECOND)) {
	    time += computeDateTimeForSecond(up);
	}
	time *= MILLISECSPERSEC;

	if (isSet(MILLISECOND)) {
	    time += computeDateTimeForMillisecond(up);
	}

	if (isSet(ZONE_OFFSET)) {
	    time += computeDateTimeForZoneOffset(up);
	}

	return (time);
}

/* returns the number of hours passed since/before day start.
 * Up == true means to count up since day start. If up is false,
 * this method counts down from day start, and the number of hours
 * passed is negative.
 */
private long computeDateTimeForAMPMAndHour(boolean up) {
    long time = 0L;

    if (up) {
	time += internalGet(HOUR);
	if (internalGet(AM_PM) == PM) {
	    time += 12;
	}
    }
    else {
	time -= getMaximum(HOUR) - internalGet(HOUR);
	if (internalGet(AM_PM) == AM) {
	    time -= 12;
	}
    }

    return time;
}
/* returns the number of days passed in the month sinc/before day of month.
 * Up == true means to count up from day of month. If up is false, the method
 * counts down from day of month, and the number of days passed is negative.
 */
private long computeDateTimeForDayOfMonth(boolean up) {
    if (up) {
	return internalGet(DAY_OF_MONTH) - 1;
    }
    else {
	return internalGet(DAY_OF_MONTH)
	    - getDaysInMonth(internalGet(MONTH), internalGet(YEAR));
    }
}

/* returns the number of days passed in the month sinc/before day of month.
 * Up == true means to count up from day of month. If up is false, the method
 * counts down from day of month, and the number of days passed is negative.
 * Day of month is calculated using day of week in month and day of week.
 */
private long computeDateTimeForDayOfWeekInMonthAndDayOfWeek(boolean up) {
    /* This is a very slow loop counting from first/last day of month
     * until the dates match.
     */
    GregorianCalendar month_counter = getMonthCounter(up);

    while (!(month_counter.get(DAY_OF_WEEK_IN_MONTH) == internalGet(DAY_OF_WEEK_IN_MONTH)
	     && month_counter.get(DAY_OF_WEEK) == internalGet(DAY_OF_WEEK))) {
	month_counter.roll(DAY_OF_YEAR, up);
    }

    return up ?
	month_counter.get(DAY_OF_MONTH) - 1 :
	month_counter.get(DAY_OF_MONTH) - getDaysInMonth(internalGet(MONTH), internalGet(YEAR));
}

/* returns the number of days passed since/before year start.
 * Up == true means to count up since year start. If up is false,
 * this method counts down from year start, and the number of days
 * passed is negative.
 */
private long computeDateTimeForDayOfYear(boolean up) {
    if (up) {
	return internalGet(DAY_OF_YEAR - 1);
    }
    else {
	return internalGet(DAY_OF_YEAR) - getDaysInYear(internalGet(YEAR));
    }
}

/* returns the number of hours passed since/before day start.
 * Up == true means to count up since day start. If up is false,
 * this method counts down from day start, and the number of hours
 * passed is negative.
 */
private long computeDateTimeForHourOfDay(boolean up) {
    if (up) {
	return internalGet(HOUR_OF_DAY);
    }
    else {
	return internalGet(HOUR_OF_DAY) - getMaximum(HOUR_OF_DAY);
    }
}

/* returns the number of milliseconds passed since/before second start.
 * Up == true means to count up since second start. If up is false,
 * this method counts down from second start, and the number of milliseconds
 * passed is negative.
 */
private long computeDateTimeForMillisecond(boolean up) {
    if (up) {
	return internalGet(MILLISECOND);
    }
    else {
	return internalGet(MILLISECOND) - getMaximum(MILLISECOND);
    }
}

/* returns the number of minutes passed since/before hour start.
 * Up == true means to count up since hour start. If up is false,
 * this method counts down from hour start, and the number of minutes
 * passed is negative.
 */
private long computeDateTimeForMinute(boolean up) {
    if (up) {
	return internalGet(MINUTE);
    }
    else {
	return internalGet(MINUTE) - getMaximum(MINUTE);
    }
}

/* returns the number of days passed in months since/before year start.
 * Up == true means to count up since year start. If up is false,
 * this method counts down from year start, and the number of days
 * passed is negative.
 */
private long computeDateTimeForMonth(boolean up) {
    long time = 0L;
    int end = internalGet(MONTH);

    /* handle lenient calendar's values of months,
     * for example -100 or 2999.
     */
    int current_year = internalGet(YEAR);
    int years = Math.abs(end) / MONTHSPERYEAR;
    end %= MONTHSPERYEAR;
    if (end < 0) {
	end += MONTHSPERYEAR;
    }

    if (up) {
	time += computeDaysBetweenYears(current_year, current_year + years);
	current_year += years;

	for (int month = JANUARY; month < end; ++month) {
	    time += getDaysInMonth(month, current_year);
	}
    }
    else {
	time -= computeDaysBetweenYears(current_year - years, current_year);
	current_year -= years;

	for (int month = DECEMBER; month > end; --month) {
	    time -= getDaysInMonth(month, current_year);
	}
    }

    return time;
}

/* returns the number of seconds passed since/before minute start.
 * Up == true means to count up since minute start. If up is false,
 * this method counts down from minute start, and the number of seconds
 * passed is negative.
 */
private long computeDateTimeForSecond(boolean up) {
    if (up) {
	return internalGet(SECOND);
    }
    else {
	return internalGet(SECOND) - getMaximum(SECOND);
    }
}

/* returns the number of days passed in the month since/before day of month.
 * Up == true means to count up from day of month. If up is false, the method
 * counts down from day of month, and the number of days passed is negative.
 * Day of month is calculated using week of month and day of week.
 */
private long computeDateTimeForWeekOfMonthAndDayOfWeek(boolean up) {
    /* This is a very slow loop counting from
     * the first/last day of month until the dates match.
     *
     * If you have a faster and *working* algorithm,
     * feel free to implement it. It has to take
     * care of all the corner cases introduced by
     * different first days in a week, and lenient
     * calendars.
     * 
     * Dalibor Topic <robilad@yahoo.com>
     */

    GregorianCalendar month_counter = getMonthCounter(up);
	
    while (!(month_counter.get(WEEK_OF_MONTH) == internalGet(WEEK_OF_MONTH)
	     && month_counter.get(DAY_OF_WEEK) == internalGet(DAY_OF_WEEK))) {
	month_counter.roll(DAY_OF_YEAR, up);
    }

    return up ?
	month_counter.get(DAY_OF_MONTH) - 1 :
	month_counter.get(DAY_OF_MONTH) - getDaysInMonth(internalGet(MONTH), internalGet(YEAR));
}

/* returns the number of days passed in the year since/before start of year.
 * Up == true means to count up from start of year. If up is false, the method
 * counts down from start of year, and the number of days passed is negative.
 * Day of year is calculated using week of year and day of week.
 */
private long computeDateTimeForWeekOfYearAndDayOfWeek(boolean up) {
    /* This is a very slow loop counting from
     * the first/last day of the year until the dates match.
     */

    GregorianCalendar year_counter = getYearCounter(up);

    while (!(year_counter.get(WEEK_OF_YEAR) == internalGet(WEEK_OF_YEAR)
	     && year_counter.get(DAY_OF_WEEK) == internalGet(DAY_OF_WEEK))) {
	year_counter.roll(DAY_OF_YEAR, up);
    }

    return up ?
	year_counter.get(DAY_OF_YEAR) :
	year_counter.get(DAY_OF_YEAR) - getDaysInYear(internalGet(YEAR));
}

/* returns the number of days passed in years since/before epoch start.
 * Up == true means to count up since epoch start. If up is false,
 * this method counts down from epoch start, and the number of days
 * passed is negative.
 */
private long computeDateTimeForYear(boolean up) {
    long time = 0L;
    int end = internalGet(YEAR);

    if (up) {
	time += computeDaysBetweenYears(EPOCH_YEAR, end);
    }
    else {
	time -= computeDaysBetweenYears(end + 1, EPOCH_YEAR);
    }

    return time;
}

/* returns the time adjustment due to zone offset in milliseconds. 
 * Up == true means that the offset is negated. If up is false,
 * this method returns the offset.
 */
private long computeDateTimeForZoneOffset(boolean up) {
    if (up) {
	return - internalGet(ZONE_OFFSET);
    }
    else {
	return internalGet(ZONE_OFFSET);
    }
}

/* returns the number of days between 1st January of start year
 * and 1st January of end year.
 */
private long computeDaysBetweenYears(int start, int end) {
    long time = 0L;

    for (int year = start; year < end; ++year) {
	time += getDaysInYear(year);
    }

    return time;
}

protected void computeFields() {
	if (areFieldsSet) {
		return;
	}
	else if (!isTimeSet) {
		throw new IllegalArgumentException();
	}

	// Calculate the calendar fields from the millisecond time.
	long rawoffset = getTimeZone().getRawOffset();
//	System.out.println("raw " + rawoffset + " tz " + getTimeZone().getID());
	computeFields(time + rawoffset);

	long offset = getTimeZone().getOffset(internalGet(ERA),
					      internalGet(YEAR),
					      internalGet(MONTH),
					      internalGet(DAY_OF_MONTH),
					      internalGet(DAY_OF_WEEK),
					      internalGet(MILLISECOND));
//	System.out.println("real " + offset + " tz " + getTimeZone().getID());
	// If we're in daylight saving, recompute based on this time
	if (offset != rawoffset) {
		computeFields(time + offset);
		internalSet(ZONE_OFFSET, (int)offset);
	} else {
		internalSet(ZONE_OFFSET, (int)rawoffset);
	}

	areFieldsSet = true;
}

// Calculate the calendar fields from the millisecond time.
private void computeFields(long time) {
	/* up is used to determine if we are counting up from the start of the epoch
	 * i.e. 1.1.1970, or down. If we are counting down, we invert time and work
	 * with the positive time value. Additionally we add special cases where 
	 * necessary to proceed further down to the right date.
	 *
	 * Example: 31.12.1969 , 23:59 h is one minute below epoch start. When we
	 * invert time, we are one minute above epoch start. In order to get the date
	 * right, we still need to decrease the day, month, etc., beside just adjusting
	 * the hours and minutes.
	 */
	boolean up = time >= 0;
	if (time < 0) {
		time = -time;
	}

	long rem = time;
	long days = rem / MILLISECSPERDAY;

	/* If we are counting down, and any second is set, then
	 * the proper day is one day further down.
	 */
	if (!up && ((rem % MILLISECSPERDAY) / MILLISECSPERSEC != 0)) {
		days++;
	}

	rem %= MILLISECSPERDAY;
	int hours = (int)(rem / MILLISECSPERHOUR);
	rem %= MILLISECSPERHOUR;
	int mins = (int)(rem / MILLISECSPERMIN);
	rem %= MILLISECSPERMIN;
	int secs = (int)(rem / MILLISECSPERSEC);
	int millis = (int)(rem % MILLISECSPERSEC);

	int wday = up ? (int)((EPOCH_WDAY + days + 1) % DAYSPERWEEK) : (int)(((EPOCH_WDAY - days + 1) % DAYSPERWEEK) + DAYSPERWEEK);
	/* correct weekday for days before gregorian change */
	if (!up &&  dateChangeYear < EPOCH_YEAR && isBeforeGregorianChange(-time)) {
		wday = (wday - 11) % DAYSPERWEEK + DAYSPERWEEK;
	}

	if (wday == 0) {
	    wday = SATURDAY;
	}

	int years = EPOCH_YEAR;
	long ydays = 0;
	for (;;) {
		long days_in_year = getDaysInYear(years);
		if (days < days_in_year) {
			if (!up && days != 0) {
				years--;
				ydays += getDaysInYear(years);
			}
			break;
		}

		days -= days_in_year;
		if (up) {
			ydays += days_in_year;
			years++;
		}
		else {
			ydays -= days_in_year;
			years--;
		}
	}
	long ymday = ydays;
	int months = up ? JANUARY : DECEMBER;
	int yday = (int)days;
	for (;;) {
		long mdays = getDaysInMonth(months, years);
		if (days < mdays) {
			break;
		}
		if (up) {
			ymday += mdays;
			months++;
		}
		else {
			ymday -= mdays;
			months--;
		}
		days -= mdays;
	}

	if (years < 1) {
		internalSet(ERA, 0);
	}
	else {
		internalSet(ERA, 1);
	}
	internalSet(YEAR, years);
	internalSet(MONTH, months);
	if (up) {
		internalSet(DAY_OF_MONTH, (int) (1 + days));
		internalSet(DAY_OF_YEAR, 1 + yday);
	}
	else {
		internalSet(DAY_OF_YEAR, getDaysInYear(years) - yday);
		internalSet(DAY_OF_MONTH, (int) (getDaysInMonth(months, years) - days /* + 1 */));
	}
	internalSet(DAY_OF_WEEK, wday);
	if (hours < 12) {
		if (up || hours == 0) {
			internalSet(AM_PM, AM);
			internalSet(HOUR, hours);
		}
		else {
			internalSet(AM_PM, PM);
			internalSet(HOUR, getMaximum(HOUR) - hours);
		}
	}
	else {
		if (up) {
			internalSet(AM_PM, PM);
			internalSet(HOUR, hours - 12);
		}
		else {
			internalSet(AM_PM, AM);
			internalSet(HOUR, getMaximum(HOUR_OF_DAY) - hours);
		}
	}

	if (up) {
		internalSet(HOUR_OF_DAY, hours);
		internalSet(MINUTE, mins);
		internalSet(SECOND, secs);
		internalSet(MILLISECOND, millis);
	}
	else {
		internalSet(HOUR_OF_DAY, hours == 0 ? 0 : getMaximum(HOUR_OF_DAY) - hours);
		internalSet(MINUTE, mins == 0 ? 0 : getMaximum(MINUTE) - mins);
		internalSet(SECOND, secs == 0 ? 0 : getMaximum(SECOND) - secs);
		internalSet(MILLISECOND, millis == 0 ? 0 : getMaximum(MILLISECOND) - millis);
	}

	/* FIXME: Different time zones/locales support different days/year schemes.
	   Current implementations is US-centric in that the first week in a year
	   is the week with at least one day, while ISO standard says the first week
	   in year is the first one with at least 4 days in the year.
	*/
	int yearstartday = (int)((EPOCH_WDAY + ydays) % DAYSPERWEEK);
	int weekofyear = (yearstartday + internalGet(DAY_OF_YEAR)) / DAYSPERWEEK + 1;

	internalSet(WEEK_OF_YEAR, weekofyear);
	internalSet(DAY_OF_WEEK_IN_MONTH, (1 + internalGet(DAY_OF_MONTH)) / DAYSPERWEEK);

	int monthstartday = (int)((EPOCH_WDAY + ymday) % DAYSPERWEEK);
	int weekofmonth = (monthstartday + internalGet(DAY_OF_MONTH)) / DAYSPERWEEK + 1;

	internalSet(WEEK_OF_MONTH, weekofmonth);
}

protected void computeTime() {
	if (isTimeSet) {
		return;
	}

	long offset = 0;

	if( !isSet(ERA) )
	{
		// Assume A.D.
		set(ERA, AD);
	}

	if( !isSet(ZONE_OFFSET) )
	{
		// Assume the time is given in current time zone.
		offset = getTimeZone().getOffset(internalGet(ERA),
						 internalGet(YEAR),
						 internalGet(MONTH),
						 internalGet(DAY_OF_MONTH),
						 internalGet(DAY_OF_WEEK),
						 internalGet(MILLISECOND));
	}
	time = computeDateTime() - offset;
	isTimeSet = true;
}

public boolean equals(Object obj) {
    return (obj instanceof GregorianCalendar) && super.equals(obj);
}

/* returns the number of days in a month in a given year */
private int getDaysInMonth(int month, int year) {
	switch (month) {
	    case FEBRUARY:
		    if (isLeapYear(year)) {
			    return 29;
		    }
		    else {
			    return 28;
		    }
	    case APRIL:
	    case SEPTEMBER:
	    case JUNE:
	    case NOVEMBER:
		    return 30;
	    case JANUARY:
	    case MARCH:
	    case MAY:
	    case JULY:
	    case AUGUST:
	    case OCTOBER:
	    case DECEMBER:
	    default:
		    return 31;
	}
}

/* returns the number of days in the given year */
private int getDaysInYear(int year) {
	return isLeapYear(year) ? 366 : 365;
}

public int getGreatestMinimum(int field) {
	return (getMinimum(field));
}

final public Date getGregorianChange() {
	if (dateChange == null) {
		return stdDateChange;
	}
	else {
		return dateChange;
	}
}

public int getLeastMaximum(int field) {
	switch (field) {
	    case DAY_OF_MONTH:
		    // february in a non-leap year
		    return (28);
	    case DAY_OF_WEEK_IN_MONTH:
		    // on a february in a non-leap year that fits into four weeks
	    case WEEK_OF_MONTH:
		    // february in a non-leap year can fit into four weeks
		    return 4;
	    case DAY_OF_YEAR:
		    // if it's not a leap year
		    return 365;
	    case WEEK_OF_YEAR:
		    // that's what jdk 1.3 says.
		    return 52;
	    case YEAR:
		    // that's what jdk 1.3 says.
		    return  292269054;
	default:
		return (getMaximum(field));
	}
}

public int getMaximum(int field) {
	switch (field) {
	case ERA:
	case AM_PM:
		return (1);
	case YEAR:
		return (292278994);
	case MONTH:
		return (DECEMBER);
	case WEEK_OF_YEAR:
		return (53);
	case DAY_OF_WEEK_IN_MONTH:
	case WEEK_OF_MONTH:
		return (6);
	case DAY_OF_MONTH: // DATE
		return (31);
	case DAY_OF_YEAR:
		return (366);
	case DAY_OF_WEEK:
		return (DAYSPERWEEK);
	case HOUR:
		return (11);
	case HOUR_OF_DAY:
		return (23);
	case MINUTE:
	case SECOND:
		return (59);
	case MILLISECOND:
		return (999);
	case ZONE_OFFSET:
		return (43200000);
	case DST_OFFSET:
	default:
		return (0);
	}
}

public int getMinimum(int field) {
	switch (field) {
	case YEAR:
	case WEEK_OF_YEAR:
	case DAY_OF_MONTH: // DATE
	case DAY_OF_YEAR:
	case DAY_OF_WEEK:
		return (1);
	case MONTH:
		return (JANUARY);
	case DAY_OF_WEEK_IN_MONTH:
		return (-1);
	case ZONE_OFFSET:
		return (-43200000);
	case ERA:
	case AM_PM:
	case MINUTE:
	case SECOND:
	case MILLISECOND:
	case HOUR:
	case HOUR_OF_DAY:
	case DST_OFFSET:
	case WEEK_OF_MONTH:
	default:
		return (0);
	}
}

/* returns a new calendar suitable for counting days
 * since/before start of the month. If up is true,
 * the calendar date is set to be the first of month.
 * If it is false, the claendar date is set to be the
 * last of month.
 *
 * Used in some of computeDate* methods.
 */
private GregorianCalendar getMonthCounter(boolean up) {
    GregorianCalendar counter =
	new GregorianCalendar(internalGet(YEAR),
			      internalGet(MONTH),
			      up ? 1 : getDaysInMonth(internalGet(MONTH),
						      internalGet(YEAR)));

    completeCounter(counter);
    return counter;
}

/* returns a new calendar suitable for counting days
 * since/before start of the year. If up is true,
 * the calendar date is set to be the first day of year.
 * If it is false, the calendar date is set to be the
 * last day of year.
 *
 * Used in some of computeDate* methods.
 */
private GregorianCalendar getYearCounter(boolean up) {
    GregorianCalendar counter =
	new GregorianCalendar(internalGet(YEAR),
			      up ? JANUARY : DECEMBER,
			      up ? 1 : 31);

    completeCounter(counter);
    return counter;
}

public synchronized int hashCode() {
	return (getFirstDayOfWeek() ^ getMinimalDaysInFirstWeek());
}

/* returns whether a given absolute time is before the gregorian change */
private boolean isBeforeGregorianChange(long time) {
	return time < dateChange.getTime();
}

public boolean isLeapYear(int year) {

	/* If the year is after the Gregorian Change,
	 * then leap years are determined by the
	 * Gregorian rule: a leap year is every fourth year,
	 * except century years ( year % 100 == 0) that not divisible by 400.
	 * 
	 * Otherwise, the Julian rule is used,
	 * where every fourth year is a leap year.
	 */
	if (year > dateChangeYear) {
		if (year % 400 == 0) {
			return (true);
		}
		if (year % 100 == 0) {
			return (false);
		}
		if (year % 4 == 0) {
			return (true);
		}
		return (false);
	}
	else {
		return (year % 4) == 0;
	}
}

public void roll(int field, boolean up) {
	if (field == ZONE_OFFSET || field == DST_OFFSET) {
		throw new IllegalArgumentException("bad field: " + field);
	}
	if (!isSet(field)) {
		computeFields();
	}
	if (up) {
		if (internalGet(field) == getMaximum(field)) {
			set(field, getMinimum(field));
		}
		else {
			set(field, internalGet(field) + 1);
		}
	}
	else {
		if (internalGet(field) == getMinimum(field)) {
			set(field, getMaximum(field));
		}
		else {
			set(field, internalGet(field) - 1);
		}
	}
	computeTime();
	computeFields();
}

public void setGregorianChange(Date date) {
	dateChange = date;
	dateChangeYear = date.getYear();
}
}
