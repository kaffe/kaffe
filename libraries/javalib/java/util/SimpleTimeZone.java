
/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util;

public class SimpleTimeZone extends TimeZone {
	private static final long serialVersionUID = -403250971215465050L;
	private int startMonth;
	private int startDayOfWeek;
	private int startDayOfWeekInMonth;
	private int startTime;
	private int endMonth;
	private int endDayOfWeek;
	private int endDayOfWeekInMonth;
	private int endTime;
	private int startYear;
	private int rawOffset;
	private int daylightOffset;
	private boolean useDaylight;

public SimpleTimeZone(int rawOffset, String ID) {
	this.rawOffset = rawOffset;
	setID(ID);
	useDaylight = false;
}

public SimpleTimeZone(int rawOffset, String ID,
  int startMonth, int startDayOfWeekInMonth, int startDayOfWeek, int startTime,
  int endMonth, int endDayOfWeekInMonth, int endDayOfWeek, int endTime)
	{
	this.startYear = 0;
	this.rawOffset = rawOffset;
	this.daylightOffset = rawOffset + 60*60*1000;
	setID(ID);

	setStartRule(startMonth, startDayOfWeekInMonth, startDayOfWeek, startTime);
	setEndRule(endMonth, endDayOfWeekInMonth, endDayOfWeek, endTime);

	useDaylight = true;
}

public boolean equals(Object o)
	{
	if (o instanceof SimpleTimeZone) {
		SimpleTimeZone obj = (SimpleTimeZone)o;
		if (this.getID() == obj.getID() &&
		    this.rawOffset == obj.rawOffset &&
		    this.startMonth == obj.startMonth &&
		    this.startDayOfWeekInMonth == obj.startDayOfWeekInMonth &&
		    this.startDayOfWeek == obj.startDayOfWeek &&
		    this.startTime == obj.startTime &&
		    this.endMonth == obj.endMonth &&
		    this.endDayOfWeekInMonth == obj.endDayOfWeekInMonth &&
		    this.endDayOfWeek == obj.endDayOfWeek &&
		    this.endTime == obj.endTime &&
		    this.startYear == obj.startYear) {
			return (true);
		}
	}
	return (false);
}

public int getOffset(int era, int year, int month, int day, int dayOfWeek, int millis)
	{
	int offset = rawOffset;
	int doffset = daylightOffset;

	if (year < startYear || era == GregorianCalendar.BC) {
		return (offset);
	}
	if (useDaylight == false) {
		return (offset);
	}

	// Time is outside DST
	if (month < startMonth || month > endMonth) {
		return (offset);
	}
	// Time is inside DST
	if (month > startMonth && month < endMonth) {
		return (doffset);
	}

	// Okay we did the easy stuff, now we have to do the hard stuff.

	int dayofweekinmonth = (6 + dayOfWeek - Calendar.SUNDAY + day - 1) / 7;

	int mdays;
	switch (month) {
	case Calendar.JANUARY:
	case Calendar.MARCH:
	case Calendar.MAY:
	case Calendar.JULY:
	case Calendar.AUGUST:
	case Calendar.OCTOBER:
	case Calendar.DECEMBER:
	default:
		mdays = 31;
		break;
	case Calendar.FEBRUARY:
		mdays = 29;	// Worst case - probably go wrong is we have
		// a DST on a leap day (which'd be stupid anyhow)
		break;
	case Calendar.APRIL:
	case Calendar.SEPTEMBER:
	case Calendar.JUNE:
	case Calendar.NOVEMBER:
		mdays = 30;
		break;
	}

	// Work out which week of the month the changes happen.
	int weeksinmonth = (6 + dayOfWeek - Calendar.SUNDAY + mdays) / 7;
	if (month == startMonth) {

		int sweekinmonth = startDayOfWeekInMonth;
		if (sweekinmonth < 0) {
			sweekinmonth += weeksinmonth;
		}

		if (dayofweekinmonth < sweekinmonth) {
			return (offset);
		}
		if (dayofweekinmonth > sweekinmonth) {
			return (doffset);
		}
		if (dayOfWeek < startDayOfWeek) {
			return (offset);
		}
		if (dayOfWeek > startDayOfWeek) {
			return (doffset);
		}
		if (millis < startTime) {
			return (offset);
		}
		return (doffset);

	}
	else {

		int eweekinmonth = endDayOfWeekInMonth;
		if (eweekinmonth < 0) {
			eweekinmonth += weeksinmonth;
		}

		if (dayofweekinmonth < eweekinmonth) {
			return (doffset);
		}
		if (dayofweekinmonth > eweekinmonth) {
			return (offset);
		}
		if (dayOfWeek < endDayOfWeek) {
			return (doffset);
		}
		if (dayOfWeek > endDayOfWeek) {
			return (offset);
		}
		if (millis < endTime) {
			return (doffset);
		}
		return (offset);
	}
}

public int getRawOffset()
	{
	return (rawOffset);
}

public synchronized int hashCode()
	{
	return ((int)rawOffset);
}

public boolean inDaylightTime(Date date)
	{
	if (useDaylight == false) {
		return (false);
	}

	// Create a calendar and set the time.
	GregorianCalendar cal = new GregorianCalendar();
	cal.setTime(date);

	// Extra various fields and use them to compute the timezone offset for
	// this date.
	int offset = getOffset(cal.get(Calendar.ERA), cal.get(Calendar.YEAR), cal.get(Calendar.MONTH), cal.get(Calendar.DAY_OF_MONTH), cal.get(Calendar.DAY_OF_WEEK), (((cal.get(Calendar.HOUR) * 60) + cal.get(Calendar.MINUTE)) * 60 + cal.get(Calendar.SECOND)) * 1000 + cal.get(Calendar.MILLISECOND));

	// If offset is still raw then we're not in daylight savings time.
	if (offset == rawOffset) {
		return (false);
	}

	// Otherwise we must be.
	return (true);
}

public void setEndRule(int month, int dayOfWeekInMonth, int dayOfWeek, int time)
	{
	endMonth = month;
	endDayOfWeekInMonth = dayOfWeekInMonth;
	endDayOfWeek = dayOfWeek;
	endTime =  time;
}

public void setRawOffset(int offsetMillis)
	{
	rawOffset = offsetMillis;
}

public void setStartRule(int month, int dayOfWeekInMonth, int dayOfWeek, int time)
	{
	startMonth = month;
	startDayOfWeekInMonth = dayOfWeekInMonth;
	startDayOfWeek = dayOfWeek;
	startTime = time;
}

public void setStartYear(int year)
	{
	startYear = year;
}

public boolean useDaylightTime()
	{
	return (useDaylight);
}
}

