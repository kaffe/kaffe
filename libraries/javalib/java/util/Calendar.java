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
	lenient = true;
	firstdayofweek = SUNDAY;
	mindaysinfirstweek = 1;
	zone = zne;
	locale = aLocale;
	set(ZONE_OFFSET, zne.getRawOffset());
}

abstract public void add(int field, int amount);

public boolean after(Object when) {
	try {
		Calendar cal = (Calendar)when;
		if (getTimeInMillis() > cal.getTimeInMillis()) {
			return (true);
		}
	}
	catch (ClassCastException _) {
	}
	catch (NullPointerException __) {
	}
	return (false);
}

private boolean areAllFieldsSet() {
	for (int i = 0; i < FIELD_COUNT; ++i) {
		if (!isSet(i)) {
			return false;
		}
	}

	return true;
}

public boolean before(Object when) {
	try {
		Calendar cal = (Calendar)when;
		if (getTimeInMillis() < cal.getTimeInMillis()) {
			return (true);
		}
	}
	catch (ClassCastException _) {
	}
	catch (NullPointerException __) {
	}
	return (false);
}

final public void clear()
{
	Arrays.fill(fields, 0);
	Arrays.fill(isSet, false);
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

	System.arraycopy(fields, 0, cal.fields, 0, fields.length);
	System.arraycopy(isSet, 0, cal.isSet, 0, isSet.length);
	cal.time = time;
	cal.isTimeSet = isTimeSet;
	cal.areFieldsSet = areFieldsSet;
	cal.lenient = lenient;
	cal.firstdayofweek = firstdayofweek;
	cal.mindaysinfirstweek = mindaysinfirstweek;

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

public boolean equals(Object obj) {
	if (obj instanceof Calendar) {
		Calendar cal = (Calendar)obj;
		return (isLenient() == cal.isLenient()
			&& getFirstDayOfWeek() == cal.getFirstDayOfWeek()
			&& getMinimalDaysInFirstWeek() == cal.getMinimalDaysInFirstWeek()
			&& getTimeZone() == cal.getTimeZone()
			&& getTime().equals(cal.getTime()));
	}
	return (false);
}

final public int get(int field)
{
	if (!isSet[field]) {
		complete();
	}
	return (internalGet(field));
}

private int getActualLimit(int field, boolean up) {
	int old_value = get(field);
	int limit = 0;

	do {
		limit = get(field);
		roll (field, up);
	}
	while (limit != get(field));
	set(field, old_value);
	return limit;
}

public int getActualMaximum(int field) {
	return getActualLimit(field, true);
}

public int getActualMinimum(int field) {
	return getActualLimit(field, false);
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

public int hashCode() {
	return (getFirstDayOfWeek()
		^ getMinimalDaysInFirstWeek()
		^ getTimeZone().getRawOffset()
		^ (int) getTimeInMillis()
		^ (isLenient() ? 0xFFFFFFFF : 0));
}

final protected int internalGet(int field)
{
	return (fields[field]);
}

/* used to just set the given field to the given value */
void internalSet(int field, int value) {
    isSet[field] = true;
    fields[field] = value;
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
	/* the range of value is restricted for lenient calendars */
	if (isLenient()) {
		if (value > internalGet(field)) {
			int limit = getMaximum(field);
			if (value > limit) {
				value = limit;
			}
		}
		else if (value < internalGet(field)) {
			int limit = getMinimum(field);
			if (value < limit) {
				value = limit;
			}
		}
	}

	internalSet(field, value);

	/* Perform eventual clean up, clearing other fields,
	 * so that the recomputation will consider the new
	 * values. See Chen et al. Java Class Libraries
	 * Second Edition Volume 1, p.270.
	 *
	 * We deferr the setting of fields until the recomputation.
	 */
	switch (field) {
	    case AM_PM:
		    clear(HOUR_OF_DAY);
		    break;
	    case DAY_OF_WEEK:
		    if (isSet(DAY_OF_MONTH)) {
			    clear(DAY_OF_MONTH);
		    }
		    else {
			    clear(MONTH);
			    clear(DAY_OF_YEAR);
		    }
		    break;
	    case DAY_OF_WEEK_IN_MONTH:
		    clear(DAY_OF_MONTH);
		    clear(WEEK_OF_MONTH);
		    break;
	    case DAY_OF_YEAR:
		    clear(MONTH);
		    break;
	    case HOUR:
		    clear(HOUR_OF_DAY);
		    break;
	    case WEEK_OF_MONTH:
		    clear(DAY_OF_MONTH);
		    break;
	    case WEEK_OF_YEAR:
		    clear(MONTH);
		    clear(DAY_OF_YEAR);
		    break;
	}

	isTimeSet = false;
	areFieldsSet = false;	// Force recalculation of other fields.
}

final public void set(int year, int month, int date)
{
	set(YEAR, year);
	set(MONTH, month);
	set(DATE, date);
}

final public void set(int year, int month, int date, int hour, int minute)
{
	set(year, month, date);
	set(HOUR_OF_DAY, hour);
	set(MINUTE, minute);
}

final public void set(int year, int month, int date, int hour, int minute, int second)
{
	set(year, month, date, hour, minute);
	set(SECOND, second);
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
	complete();
	return getClass().getName()
		+ "[time=" + getTimeInMillis()
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
