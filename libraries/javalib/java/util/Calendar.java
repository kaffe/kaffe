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
	private static final long serialVersionUID = -1807547505821590642L;
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
}

private boolean areAllFieldsSet() {
	for (int i = 0; i < FIELD_COUNT; ++i) {
		if (!isSet(i)) {
			return false;
		}
	}

	return true;
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

	return (cal);
}

protected void complete()
{
	if (!isTimeSet) {
		computeTime();
	}
	if (!areFieldsSet) {
		computeFields();
	}
}

abstract protected void computeFields();

abstract protected void computeTime();

abstract public boolean equals(Object when);

final public int get(int field)
{
	if (!isSet[field])
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
	if (!isTimeSet) {
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

public void roll(int field, int amount)
{
	boolean direction = true;
	int lpc;
	
	if( amount < 0 )
	{
		amount = -amount;
		direction = false;
	}
	for( lpc = 0; lpc < amount; lpc++ )
	{
		this.roll(field, direction);
	}
}

final public void set(int field, int value)
{
	isSet[field] = true;
	fields[field] = value;
	isTimeSet = false;
	areFieldsSet = false;	// Force recalculation of other fields.
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

public String toString() {
	return getClass().getName()
		+ "[time=" + time
		+ ",areFieldsSet=" + areFieldsSet
		+ ",areAllFieldsSet=" + areAllFieldsSet()
		+ ",lenient=" + isLenient()
		+ ",zone=" + getTimeZone()
		+ ",firstDayOfWeek=" + getFirstDayOfWeek()
		+ ",minimalDaysInFirstWeek=" + getMinimalDaysInFirstWeek()
		+ ",ERA=" + get(ERA)
		+ ",YEAR=" + get(YEAR)
		+ ",MONTH=" + get(MONTH)
		+ ",WEEK_OF_YEAR=" + get(WEEK_OF_YEAR)
		+ ",WEEK_OF_MONTH=" + get(WEEK_OF_MONTH)
		+ ",DAY_OF_MONTH=" + get(DAY_OF_MONTH)
		+ ",DAY_OF_YEAR=" + get(DAY_OF_YEAR)
		+ ",DAY_OF_WEEK=" + get(DAY_OF_WEEK)
		+ ",DAY_OF_WEEK_IN_MONTH=" + get(DAY_OF_WEEK_IN_MONTH)
		+ ",AM_PM=" + get(AM_PM)
		+ ",HOUR=" + get(HOUR)
		+ ",HOUR_OF_DAY=" + get(HOUR_OF_DAY)
		+ ",MINUTE=" + get(MINUTE)
		+ ",SECOND=" + get(SECOND)
		+ ",MILLISECOND=" + get(MILLISECOND)
		+ ",ZONE_OFFSET=" + get(ZONE_OFFSET)
		+ ",DST_OFFSET=" + get(DST_OFFSET)
		+ ']';
}
}
