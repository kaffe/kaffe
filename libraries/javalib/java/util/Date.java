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

import java.text.ParseException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.io.Serializable;

public class Date implements Serializable, Cloneable, Comparable {

	private static final long serialVersionUID = 7523967970034938905L;
	private long time;

public Date() {
	this(System.currentTimeMillis());
}

/**
 * @deprecated
 */
public Date(String s) {
	time = parse(s);
}

/**
 * @deprecated
 */
public Date(int year, int month, int date) {
	Calendar cal = Calendar.getInstance();
	cal.set(1900+year, month, date);
	time = cal.getTime().getTime();
}

/**
 * @deprecated
 */
public Date(int year, int month, int date, int hrs, int min) {
	Calendar cal = Calendar.getInstance();
	cal.set(1900+year, month, date, hrs, min);
	time = cal.getTime().getTime();
}

/**
 * @deprecated
 */
public Date(int year, int month, int date, int hrs, int min, int sec) {
	Calendar cal = Calendar.getInstance();
	cal.set(1900+year, month, date, hrs, min, sec);
	time = cal.getTime().getTime();
}

public Date(long date) {
	time = date;
}

/**
 * @deprecated
 */
public static long UTC(int year, int month, int date, int hrs, int min, int sec) {
	Calendar cal = Calendar.getInstance(TimeZone.getTimeZone("UTC"));
	cal.set(1900+year, month, date, hrs, min, sec);
	return (cal.getTime().getTime());
}

public boolean after(Date when) {
	return (getTime() > when.getTime());
}

public boolean before(Date when) {
	return (getTime() < when.getTime());
}

public int compareTo(Object o) {
	return compareTo((Date)o);
}

public int compareTo(Date that) {
	final long thisTime = this.getTime();
	final long thatTime = that.getTime();

	return (thisTime == thatTime) ? 0 : (thisTime < thatTime) ? -1 : 1;
}

public boolean equals(Object obj) {
	return (obj instanceof Date) && getTime() == ((Date)obj).getTime();
}

/**
 * @deprecated
 */
public int getDate() {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	return (cal.get(Calendar.DATE));
}

/**
 * @deprecated
 */
public int getDay() {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	return (cal.get(Calendar.DAY_OF_WEEK) - Calendar.SUNDAY);
}

/**
 * @deprecated
 */
public int getHours() {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	return (cal.get(Calendar.HOUR_OF_DAY));
}

/**
 * @deprecated
 */
public int getMinutes() {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	return (cal.get(Calendar.MINUTE));
}

/**
 * @deprecated
 */
public int getMonth() {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	return (cal.get(Calendar.MONTH));
}

/**
 * @deprecated
 */
public int getSeconds() {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	return (cal.get(Calendar.SECOND));
}

public long getTime() {
	return (time);
}

/**
 * @deprecated
 */
public int getTimezoneOffset() {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	return (cal.get(Calendar.ZONE_OFFSET));
}

/**
 * @deprecated
 */
public int getYear() {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	return (cal.get(Calendar.YEAR)-1900);
}

public int hashCode() {
	return (int)getTime();
}

/**
 * @deprecated
 */
public static long parse(String str) {
	DateFormat form;
	for (int ds = DateFormat.FULL; ds <= DateFormat.SHORT; ds++) {
		for (int ts = DateFormat.FULL; ts <= DateFormat.SHORT; ts++) {
			try {
				form = DateFormat.getDateTimeInstance(ds, ts);
				return (form.parse(str).time);
			}
			catch (ParseException _) {
			}
		}
	}
	for (int s = DateFormat.FULL; s <= DateFormat.SHORT; s++) {
		try {
			form = DateFormat.getDateInstance(s);
			return (form.parse(str).time);
		}
		catch (ParseException _) {
		}
	}
	for (int s = DateFormat.FULL; s <= DateFormat.SHORT; s++) {
		try {
			form = DateFormat.getTimeInstance(s);
			return (form.parse(str).time);
		}
		catch (ParseException _) {
		}
	}
	return (0);
}

/**
 * @deprecated
 */
public void setDate(int date) {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	cal.set(Calendar.DATE, date);
	time = cal.getTime().getTime();
}

/**
 * @deprecated
 */
public void setHours(int hours) {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	cal.set(Calendar.HOUR_OF_DAY, hours);
	time = cal.getTime().getTime();
}

/**
 * @deprecated
 */
public void setMinutes(int minutes) {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	cal.set(Calendar.MINUTE, minutes);
	time = cal.getTime().getTime();
}

/**
 * @deprecated
 */
public void setMonth(int month) {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	cal.set(Calendar.MONTH, month);
	time = cal.getTime().getTime();
}

/**
 * @deprecated
 */
public void setSeconds(int seconds) {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	cal.set(Calendar.SECOND, seconds);
	time = cal.getTime().getTime();
}

public void setTime(long time) {
	this.time = time;
}

/**
 * @deprecated
 */
public void setYear(int year) {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	cal.set(Calendar.YEAR, 1900+year);
	time = cal.getTime().getTime();
}

/**
 * @deprecated
 */
public String toGMTString() {
	DateFormat form = new SimpleDateFormat("d MMM yyyy HH:mm:ss GMT");
	form.setTimeZone(TimeZone.getTimeZone("GMT"));
	return (form.format(this));
}

/**
 * @deprecated
 */
public String toLocaleString() {
	DateFormat form = DateFormat.getDateTimeInstance();
	form.setTimeZone(TimeZone.getDefault());
	return (form.format(this));
}

public String toString() {
	SimpleDateFormat form =
	    new SimpleDateFormat("EEE MMM dd HH:mm:ss zzz yyyy");
	return (form.format(this));
}

public Object clone() {
	try {
		return super.clone();
	} catch (CloneNotSupportedException e) {
		return null;		// can't happen
	}
}
}
