
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

import java.io.File;
import java.io.IOException;
import java.io.Serializable;
import kaffe.util.UNIXTimeZone;

abstract public class TimeZone implements Serializable, Cloneable {
	private static final long serialVersionUID = 3581463369166924961L;
	private static TimeZone defaultTimeZone = null;

	/**
	 * zones maps timezone IDs (of type String) to either
	 * TimeZone objects or to File objects.
	 * File objects are converted to TimeZone objects
	 * on the first access.  This is done transparently in 
	 * the loadTimeZone(String ID) method.
	 * @see loadTimeZone
	 */
	private static HashMap zones = new HashMap();

	private static boolean zonesLoaded = false;

	private static String[] zoneDirs = {
		"/usr/share/zoneinfo",
		"/usr/share/lib/zoneinfo"
		// XXX others to try??
	};

	private String timezoneID = null;

// Load the system timezones
private static void loadTimeZones() {

	// Install some standard SimpleTimeZones
	addSimple(-11*60*60*1000, "MIT");
	addSimple(-10*60*60*1000, "HST");
	addSimple(-9*60*60*1000, "AST");
	addSimple(-8*60*60*1000, "PST", Calendar.APRIL, 1, Calendar.SUNDAY, 2*60*60*1000, Calendar.OCTOBER, -1, Calendar.SUNDAY, 2*60*60*1000);
	addSimple(-8*60*60*1000, "PDT", Calendar.APRIL, 1, Calendar.SUNDAY, 2*60*60*1000, Calendar.OCTOBER, -1, Calendar.SUNDAY, 2*60*60*1000);
	addSimple(-7*60*60*1000, "PNT");
	addSimple(-7*60*60*1000, "MST", Calendar.APRIL, 1, Calendar.SUNDAY, 2*60*60*1000, Calendar.OCTOBER, -1, Calendar.SUNDAY, 2*60*60*1000);
	addSimple(-7*60*60*1000, "MDT", Calendar.APRIL, 1, Calendar.SUNDAY, 2*60*60*1000, Calendar.OCTOBER, -1, Calendar.SUNDAY, 2*60*60*1000);
	addSimple(-6*60*60*1000, "CST", Calendar.APRIL, 1, Calendar.SUNDAY, 2*60*60*1000, Calendar.OCTOBER, -1, Calendar.SUNDAY, 2*60*60*1000);
	addSimple(-6*60*60*1000, "CDT", Calendar.APRIL, 1, Calendar.SUNDAY, 2*60*60*1000, Calendar.OCTOBER, -1, Calendar.SUNDAY, 2*60*60*1000);
	addSimple(-5*60*60*1000, "EST", Calendar.APRIL, 1, Calendar.SUNDAY, 2*60*60*1000, Calendar.OCTOBER, -1, Calendar.SUNDAY, 2*60*60*1000);
	addSimple(-5*60*60*1000, "EDT", Calendar.APRIL, 1, Calendar.SUNDAY, 2*60*60*1000, Calendar.OCTOBER, -1, Calendar.SUNDAY, 2*60*60*1000);
	addSimple(-5*60*60*1000, "IET");
	addSimple(-4*60*60*1000, "PRT");
	addSimple(-3500*60*60, "CNT");
	addSimple(-3*60*60*1000, "AGT");
	addSimple(-1*60*60*1000, "CAT");
	addSimple(0, "GMT");
	addSimple(1*60*60*1000, "ECT");
	addSimple(1*60*60*1000, "EET");
	addSimple(2*60*60*1000, "ART");
	addSimple(3*60*60*1000, "EAT");
	addSimple(3500*60*60, "MET");
	addSimple(4*60*60*1000, "NET");
	addSimple(5*60*60*1000, "PLT");
	addSimple(5500*60*60, "IST");
	addSimple(6*60*60*1000, "BST");
	addSimple(7*60*60*1000, "VST");
	addSimple(8*60*60*1000, "CTT");
	addSimple(9*60*60*1000, "JST");
	addSimple(9500*60*60, "ACT");
	addSimple(10*60*60*1000, "AET");
	addSimple(11*60*60*1000, "SST");
	addSimple(12*60*60*1000, "NST");

	// Now read in any 'TZif' timezone files we find
	for (int i = 0; i < zoneDirs.length; i++) {
		File dir = new File(zoneDirs[i]);
		if (dir.isDirectory())
			addZoneFiles(null, dir);
	}

	// Done
	zonesLoaded = true;
}

private static void addSimple(int rawOffset, String id) {
	zones.put(id, new SimpleTimeZone(rawOffset, id));
}

private static void addSimple(int off, String id, int i1,
		int i2, int i3, int i4, int i5, int i6, int i7, int i8) {
	zones.put(id, new SimpleTimeZone(off, id, i1, i2, i3, i4,
		i5, i6, i7, i8));
}

// Recurse through a directory tree adding any UNIX zone files found
private static void addZoneFiles(String prefix, File dir) {
	String[] files = dir.list();
	for (int i = 0; i < files.length; i++) {
		File file = new File(dir, files[i]);
		String filePrefix = (prefix == null) ?
		    files[i] : prefix + "/" + files[i];
		if (file.isFile()) {
			zones.put(filePrefix, file);
		} else if (file.isDirectory()) {
			addZoneFiles(filePrefix, file);
		}
	}
}

public TimeZone() {
}

public Object clone() {
	try {
		return (TimeZone)super.clone();
	} catch (Exception e) {
		return null;
	}
}

public static synchronized String[] getAvailableIDs() {
	if (!zonesLoaded)
		loadTimeZones();
	return (String[])zones.keySet().toArray(new String[zones.size()]);
}

private static TimeZone loadTimeZone(String ID) {
	Object tzo = zones.get(ID);
	if (tzo == null) {
		return getCustomTimeZone(ID);
        }
	if (tzo instanceof TimeZone) {
		return ((TimeZone)tzo);
	}
	if (!(tzo instanceof File)) {
		throw new InternalError("found " + tzo + " in zones map");
	}
	File file = (File)tzo;
	TimeZone tz = null;

	try {
		tz = new UNIXTimeZone(ID, file);
		zones.put(ID, tz);
	} catch (IOException e) {
		/* If file cannot parsed properly, simply fall back to GMT
		 * timezone.  This is what the getTimeZone() API doc demands.
		 */
		tz = getTimeZone("GMT");
		zones.put(ID, tz);
	}
	return (tz);
}

private static TimeZone getCustomTimeZone(String ID) {
	TimeZone tz = getTimeZone("GMT");
	if (ID.startsWith("GMT")) {
		try {
			String sign = ID.substring(3,4);
			String hh;
			String mm;
			int i = ID.indexOf(":");
			if (i >= 0) {
				hh = ID.substring(4,i);
				mm = ID.substring(i+1);
			}
			else {
				hh = ID.substring(4,6);
				mm = ID.substring(6,8);
			}
			int hhi = Integer.parseInt(hh);
			int mmi = Integer.parseInt(mm);
			int rawOffset = (hhi * 60 + mmi) * 60 * 1000;
			if (sign.equals("+")) {
				tz = new SimpleTimeZone(rawOffset, ID);
			}
			else if (sign.equals("-")) {
				tz = new SimpleTimeZone(- rawOffset, ID);
			}
		} catch (Exception e) {}
	}
	zones.put(ID, tz);
	return (tz);
}

public static synchronized String[] getAvailableIDs(int rawOffset) {
	if (!zonesLoaded)
		loadTimeZones();
	HashSet ids = new HashSet();
	for (Iterator i = zones.entrySet().iterator(); i.hasNext(); ) {
		Map.Entry ent = (Map.Entry)i.next();
		TimeZone tz = loadTimeZone((String)ent.getKey());
		if (tz.getRawOffset() == rawOffset)
			ids.add(ent.getKey());
	}
	return (String[])ids.toArray(new String[ids.size()]);
}

public static synchronized TimeZone getDefault()
{
	if (defaultTimeZone == null) {
		String zne = System.getProperty("user.timezone", "GMT");
		defaultTimeZone = getTimeZone(zne);
		if (defaultTimeZone == null) {
			defaultTimeZone = getTimeZone("GMT");
		}
		if (defaultTimeZone == null) {
			throw new InternalError("Cannot intialize timezone."
			    + " GMT & " +zne+ " zones are undefined.");
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

public static synchronized TimeZone getTimeZone(String ID) {
	if (!zonesLoaded)
		loadTimeZones();
	return (loadTimeZone(ID));
}

abstract public boolean inDaylightTime(Date date);

public static synchronized void setDefault(TimeZone zone)
{
	defaultTimeZone = zone;
}

public void setID(String ID)
{
	timezoneID = ID;
}

abstract public void setRawOffset(int offsetMillis);

public String toString()
{
	return (timezoneID);
}

abstract public boolean useDaylightTime();
}
