package java.util;

import java.lang.String;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class SimpleTimeZone
  extends TimeZone
{
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

// Install the standard SimpleTimeZones
static {
	new SimpleTimeZone(-11*60*60*1000, "MIT");
	new SimpleTimeZone(-10*60*60*1000, "HST");
	new SimpleTimeZone(-9*60*60*1000, "AST");
	new SimpleTimeZone(-8*60*60*1000, "PST", Calendar.APRIL, 1, Calendar.SUNDAY, 2*60*60*1000, Calendar.OCTOBER, -1, Calendar.SUNDAY, 2*60*60*1000);
	new SimpleTimeZone(-7*60*60*1000, "PNT");
	new SimpleTimeZone(-7*60*60*1000, "MST", Calendar.APRIL, 1, Calendar.SUNDAY, 2*60*60*1000, Calendar.OCTOBER, -1, Calendar.SUNDAY, 2*60*60*1000);
	new SimpleTimeZone(-6*60*60*1000, "CST");
	new SimpleTimeZone(-5*60*60*1000, "EST");
	new SimpleTimeZone(-5*60*60*1000, "IET");
	new SimpleTimeZone(-4*60*60*1000, "PRT");
	new SimpleTimeZone(-3500*60*60, "CNT");
	new SimpleTimeZone(-3*60*60*1000, "AGT");
	new SimpleTimeZone(-1*60*60*1000, "CAT");
	new SimpleTimeZone(0, "GMT");
	new SimpleTimeZone(1*60*60*1000, "ECT");
	new SimpleTimeZone(1*60*60*1000, "EET");
	new SimpleTimeZone(2*60*60*1000, "ART");
	new SimpleTimeZone(3*60*60*1000, "EAT");
	new SimpleTimeZone(3500*60*60, "MET");
	new SimpleTimeZone(4*60*60*1000, "NET");
	new SimpleTimeZone(5*60*60*1000, "PLT");
	new SimpleTimeZone(5500*60*60, "IST");
	new SimpleTimeZone(6*60*60*1000, "BST");
	new SimpleTimeZone(7*60*60*1000, "VST");
	new SimpleTimeZone(8*60*60*1000, "CTT");
	new SimpleTimeZone(9*60*60*1000, "JST");
	new SimpleTimeZone(9500*60*60, "ACT");
	new SimpleTimeZone(10*60*60*1000, "AET");
	new SimpleTimeZone(11*60*60*1000, "SST");
	new SimpleTimeZone(12*60*60*1000, "NST");
}

public SimpleTimeZone(int rawOffset, String ID)
	{
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

public Object clone()
	{
	SimpleTimeZone tz = new SimpleTimeZone(getRawOffset(), getID(), startMonth, startDayOfWeekInMonth, startDayOfWeek, startTime, endMonth, endDayOfWeekInMonth, endDayOfWeek, endTime);
	return ((Object)tz);
}

public boolean equals(Object o)
	{
	try {
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
	catch (ClassCastException _) {
	}
	catch (NullPointerException _) {
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
