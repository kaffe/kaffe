package java.util;

import java.io.Serializable;
import java.lang.String;
import java.lang.System;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
abstract public class TimeZone
  implements Serializable, Cloneable
{
	private static final long serialVersionUID = 3581463369166924961L;
	private static TimeZone defaultTimeZone;
	private static Hashtable zones = new Hashtable();
	private String timezoneID = null;

public TimeZone()
{
}

public Object clone()
{
	try {
		TimeZone tz = (TimeZone)getClass().newInstance();
		tz.setID(getID());
		return ((Object)tz);
	}
	catch (Exception _) {
		return (null);
	}
}

public static synchronized String[] getAvailableIDs()
{
	String[] str = new String[zones.size()];
	Enumeration e = zones.elements();

	for (int pos = 0; e.hasMoreElements(); pos++) {
		str[pos] = ((TimeZone)e.nextElement()).getID();
	}

	return (str);
}

public static synchronized String[] getAvailableIDs(int rawOffset)
{
	Enumeration e = zones.elements();

	int cnt = 0;
	while (e.hasMoreElements()) {
		TimeZone tz = (TimeZone)e.nextElement();
		if (tz.getRawOffset() == rawOffset) {
			cnt++;
		}
	}

	// No matches
	if (cnt == 0) {
		return (null);
	}

	String str[] = new String[cnt];
	e = zones.elements();
	cnt = 0;
	while (e.hasMoreElements()) {
		TimeZone tz = (TimeZone)e.nextElement();
		if (tz.getRawOffset() == rawOffset) {
			str[cnt++] = tz.getID();
		}
	}

	return (str);
}

public static synchronized TimeZone getDefault()
{
	if (defaultTimeZone == null) {
		String zne = System.getProperty("user.timezone", "GMT");
		defaultTimeZone = getTimeZone(zne);
		if (defaultTimeZone == null) {
			defaultTimeZone = getTimeZone("GMT");
		}
	}
	return (defaultTimeZone);
}

public String getID()
{
	return (timezoneID);
}

abstract public int getOffset(int era, int year, int month, int day, int dayOfWeek, int milliseconds);

abstract public int getRawOffset();

public static synchronized TimeZone getTimeZone(String ID)
{
	Enumeration e = zones.elements();

	while (e.hasMoreElements()) {
		TimeZone tz = (TimeZone)e.nextElement();
		if (ID.equals(tz.getID())) {
			return (tz);
		}
	}

	return (null);
}

abstract public boolean inDaylightTime(Date date);

public static synchronized void setDefault(TimeZone zone)
{
	defaultTimeZone = zone;
}

public void setID(String ID)
{
	if (timezoneID != null) {
		zones.remove(timezoneID);
	}
	timezoneID = ID;
	zones.put(timezoneID, this);
}

abstract public void setRawOffset(int offsetMillis);

public String toString()
{
	return (timezoneID);
}

abstract public boolean useDaylightTime();
}
