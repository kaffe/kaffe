package java.util;

import java.io.Serializable;
import java.text.DateFormat;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
abstract public class Calendar
  extends Object
  implements Serializable, Cloneable
{
	final public static int ERA = 0;
	final public static int YEAR = 1;
	final public static int MONTH = 2;
	final public static int WEEK_OF_YEAR = 3;
	final public static int WEEK_OF_MONTH = 4;
	final public static int DATE = 5;
	final public static int DAY_OF_MONTH = 5;
	final public static int DAY_OF_YEAR = 6;
	final public static int DAY_OF_WEEK = 7;
	final public static int DAY_OF_WEEK_IN_MONTH = 8;
	final public static int AM_PM = 9;
	final public static int HOUR = 10;
	final public static int HOUR_OF_DAY = 11;
	final public static int MINUTE = 12;
	final public static int SECOND = 13;
	final public static int MILLISECOND = 14;
	final public static int ZONE_OFFSET = 15;
	final public static int DST_OFFSET = 16;
	final public static int FIELD_COUNT = 17;
	final public static int SUNDAY = 1;
	final public static int MONDAY = 2;
	final public static int TUESDAY = 3;
	final public static int WEDNESDAY = 4;
	final public static int THURSDAY = 5;
	final public static int FRIDAY = 6;
	final public static int SATURDAY = 7;
	final public static int JANUARY = 0;
	final public static int FEBRUARY = 1;
	final public static int MARCH = 2;
	final public static int APRIL = 3;
	final public static int MAY = 4;
	final public static int JUNE = 5;
	final public static int JULY = 6;
	final public static int AUGUST = 7;
	final public static int SEPTEMBER = 8;
	final public static int OCTOBER = 9;
	final public static int NOVEMBER = 10;
	final public static int DECEMBER = 11;
	final public static int UNDECIMBER = 12;
	final public static int AM = 0;
	final public static int PM = 1;
	private static Vector calendars = new Vector();
	private TimeZone zone;
	private Locale locale;
	private boolean lenient;
	private int firstdayofweek;
	private int mindaysinfirstweek;
	protected int[] fields;
	protected boolean[] isSet;
	protected long time;
	protected boolean isTimeSet;
	protected boolean areFieldsSet;

protected Calendar()
{
	this(TimeZone.getDefault(), Locale.getDefault());
}

protected Calendar(TimeZone zne, Locale aLocale)
{
	fields = new int[FIELD_COUNT];
	isSet = new boolean[FIELD_COUNT];
	time = 0;
	isTimeSet = false;
	areFieldsSet = false;
	lenient = true;
	firstdayofweek = SUNDAY;
	mindaysinfirstweek = 1;
	zone = zne;
	locale = aLocale;

	synchronized(calendars) {
		calendars.addElement(this);
	}
}

abstract public void add(int field, int amount);

abstract public boolean after(Object when);

abstract public boolean before(Object when);

final public void clear()
{
	for (int i = 0; i < FIELD_COUNT; i++) {
		fields[i] = 0;
		isSet[i] = false;
	}
	areFieldsSet = false;
}

final public void clear(int field)
{
	isSet[field] = false;
	fields[field] = 0;
	areFieldsSet = false;
}

public Object clone()
{
	Calendar cal = getInstance(zone, locale);

	cal.time = time;
	for (int i = 0; i < FIELD_COUNT; i++) {
		cal.fields[i] = fields[i];
		cal.isSet[i] = isSet[i];
	}
	cal.isTimeSet = isTimeSet;
	cal.areFieldsSet = areFieldsSet;

	return ((Object)cal);
}

protected void complete()
{
	if (isTimeSet == false) {
		computeTime();
	}
	if (areFieldsSet == false) {
		computeFields();
	}
}

abstract protected void computeFields();

abstract protected void computeTime();

abstract public boolean equals(Object when);

final public int get(int field)
{
	complete();
	return (fields[field]);
}

public static synchronized Locale[] getAvailableLocales()
{
	return (DateFormat.getAvailableLocales());
}

public int getFirstDayOfWeek()
{
	return (firstdayofweek);
}

abstract public int getGreatestMinimum(int field);

public static synchronized Calendar getInstance()
{
	return (getInstance(TimeZone.getDefault(), Locale.getDefault()));
}

public static synchronized Calendar getInstance(Locale aLocale)
{
	return (getInstance(TimeZone.getDefault(), aLocale));
}

public static synchronized Calendar getInstance(TimeZone zone)
{
	return (getInstance(zone, Locale.getDefault()));
}

public static synchronized Calendar getInstance(TimeZone zone, Locale aLocale)
{
	synchronized(calendars) {
		Enumeration e = calendars.elements();
		while (e.hasMoreElements()) {
			Calendar c = (Calendar)e.nextElement();
			if (c.zone == zone && c.locale == aLocale) {
//				c = new GregorianCalendar(c.zone, c.locale);
				return (c);
			}
		}
	}
	return (new GregorianCalendar( zone, aLocale));
}

abstract public int getLeastMaximum(int field);

abstract public int getMaximum(int field);

public int getMinimalDaysInFirstWeek()
{
	return (mindaysinfirstweek);
}

abstract public int getMinimum(int field);

final public Date getTime()
{
	return (new Date(getTimeInMillis()));
}

protected long getTimeInMillis()
{
	if (isTimeSet == false) {
		computeTime();
	}
	return (time);
}

public TimeZone getTimeZone()
{
	return (zone);
}

final protected int internalGet(int field)
{
	return (fields[field]);
}

public boolean isLenient()
{
	return (lenient);
}

final public boolean isSet(int field)
{
	return (isSet[field]);
}

abstract public void roll(int field, boolean up);

final public void set(int field, int value)
{
	isSet[field] = true;
	fields[field] = value;
	isTimeSet = false;
}

final public void set(int year, int month, int date)
{
	fields[YEAR] = year;
	fields[MONTH] = month;
	fields[DATE] = date;
	isSet[YEAR] = true;
	isSet[MONTH] = true;
	isSet[DATE] = true;
	isTimeSet = false;
}

final public void set(int year, int month, int date, int hour, int minute)
{
	fields[YEAR] = year;
	fields[MONTH] = month;
	fields[DATE] = date;
	fields[HOUR_OF_DAY] = hour;
	fields[MINUTE] = minute;
	isSet[YEAR] = true;
	isSet[MONTH] = true;
	isSet[DATE] = true;
	isSet[HOUR_OF_DAY] = true;
	isSet[MINUTE] = true;
	isTimeSet = false;
}

final public void set(int year, int month, int date, int hour, int minute, int second)
{
	fields[YEAR] = year;
	fields[MONTH] = month;
	fields[DATE] = date;
	fields[HOUR_OF_DAY] = hour;
	fields[MINUTE] = minute;
	fields[SECOND] = second;
	isSet[YEAR] = true;
	isSet[MONTH] = true;
	isSet[DATE] = true;
	isSet[HOUR_OF_DAY] = true;
	isSet[MINUTE] = true;
	isSet[SECOND] = true;
	isTimeSet = false;
}

public void setFirstDayOfWeek(int value)
{
	firstdayofweek = value;
}

public void setLenient(boolean l)
{
	lenient = l;
}

public void setMinimalDaysInFirstWeek(int value)
{
	mindaysinfirstweek = value;
}

final public void setTime(Date date)
{
	setTimeInMillis(date.getTime());
}

protected void setTimeInMillis(long millis)
{
	time = millis;
	isTimeSet = true;
	areFieldsSet = false;
	computeFields();
}

public void setTimeZone(TimeZone value)
{
	zone = value;
}
}
