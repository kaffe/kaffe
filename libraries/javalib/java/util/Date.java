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

import java.text.DateFormat;
import java.text.ParseException;
import java.lang.String;
import java.lang.System;

public class Date {

private long time = 0;

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

public boolean equals(Object obj) {
	try {
		return (getTime() == ((Date)obj).getTime());
	}
	catch (ClassCastException _) {
	}
	catch (NullPointerException _) {
	}
	return false;
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
	return (cal.get(Calendar.HOUR));
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
	return (cal.get(Calendar.YEAR)-1990);
}

public int hashCode() {
	return (int)getTime();
}

/**
 * @deprecated
 */
public static long parse(String s) {
	DateFormat form = DateFormat.getDateTimeInstance();
	try {
		return (form.parse(s).time);
	}
	catch (ParseException _) {
		return (0);
	}
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
private void setDay(int day) {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	cal.set(Calendar.DAY_OF_WEEK, day);
	time = cal.getTime().getTime();
}

/**
 * @deprecated
 */
public void setHours(int hours) {
	Calendar cal = Calendar.getInstance();
	cal.setTime(this);
	cal.set(Calendar.HOUR, hours);
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
	DateFormat form = DateFormat.getDateTimeInstance();
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
	DateFormat form = DateFormat.getDateTimeInstance();
	return (form.format(this));
}
}
