
/*
 * Copyright (c) 2000
 *      Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package kaffe.util;

import java.io.BufferedInputStream;
import java.io.DataInput;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.SimpleTimeZone;
import java.util.TimeZone;

/**
 * Represents a time zone as defined by a standard UNIX
 * <code>tzfile(5)</code> format time zone file.
 *
 * <p>
 * Example:
 *
 * <pre>
 *    UNIXTimeZone tz = new UNIXTimeZone(
 *      new File("/usr/share/zoneinfo/America/Los_Angeles"));
 * </pre>
 *
 * @see <a href="ftp://elsie.nci.nih.gov/pub/">
 *	<code>ftp://elsie.nci.nih.gov/pub/</code></a>
 */
public class UNIXTimeZone extends TimeZone {

	/**
	 * Timezone file magic number 'TZif'
	 */
	public static final int TZ_MAGIC =
	    ('T' << 24) | ('Z' << 16) | ('i' << 8) | 'f';

	private static final int MAX_TIMES = 1024;
	private static final int MAX_TYPES = 255;
	private static final int MAX_CHARS = 255;
	private static final int MAX_LEAPS = 255;

	private static final int SECSPERMIN = 60;
	private static final int MINSPERHOUR = 60;
	private static final int SECSPERHOUR = SECSPERMIN * MINSPERHOUR;
	private static final int SECSPERDAY = 24 * SECSPERHOUR;

	private Transition[] trans;	// transition times
	private TimeInfo defaultInfo;	// default TimeInfo
	private Leap[] leaps;		// leap second times
	private boolean[] standard;	// transition times standard, not wall
	private boolean[] gmt;		// transition times GMT, not local

	// Info about a transition time (i.e., change in the time offset)
	private static class Transition {
		long time;
		TimeInfo info;
	}

	// Info about a specific time offset
	private static class TimeInfo {
		int offset;
		boolean isDST;
		String abbrev;
	}

	// Info about a leap second adjustment
	private static class Leap {
		long time;
		int numSeconds;
	}

	/**
	 * Constructor.
	 *
	 * <p>
	 * @param zoneFile
	 *	The binary time zone data file in <code>tzfile(5)</code> format.
	 * @throws IOException
	 *	If there is an error processing the file
	 * @throws NullPointerException
	 *	If either argument is equal to <code>null</code>
	 */
	public UNIXTimeZone(String id, File zoneFile) throws IOException {
		FileInputStream f = new FileInputStream(zoneFile);
		try {
			if (!read(new DataInputStream(
				  new BufferedInputStream(f)))) {
				throw new IOException("invalid file contents");
			}
		} finally {
			f.close();
		}
		setID(id.toString());	// to generate NullPointerException
	}

	public int getOffset(int era, int year, int month,
			int day, int dayOfWeek, int millis) {
		return getTimeInfo(era, year,
		    month, day, dayOfWeek, millis).offset;
	}

	public int getOffset(Date date) {
		return getTimeInfo(date).offset;
	}

	public String getAbbreviation(Date date) {
		return getTimeInfo(date).abbrev;
	}

	public void setRawOffset(int offsetMillis) {
		// not
	}

	public int getRawOffset() {
		return defaultInfo.offset;
	}

	public boolean useDaylightTime() {
		for (int i = 0; i < trans.length; i++) {
			if (trans[i].info.isDST)
				return true;
		}
		return false;
	}

	public boolean inDaylightTime(Date date) {
		return getTimeInfo(date).isDST;
	}

	private TimeInfo getTimeInfo(int era, int year, int month,
			int day, int dayOfWeek, int millis) {

		// Convert to GMT time
		GregorianCalendar cal = new GregorianCalendar(
		    new SimpleTimeZone(0, "_temp"));
		cal.set(Calendar.ERA, era);
		cal.set(Calendar.YEAR, year);
		cal.set(Calendar.MONTH, month);
		cal.set(Calendar.DAY_OF_MONTH, day);
		int secs = millis / 1000;
		int hours = secs / SECSPERHOUR;
		secs -= (hours * SECSPERHOUR);
		cal.set(Calendar.HOUR_OF_DAY, hours);
		int mins = secs / SECSPERMIN;
		secs -= (mins * SECSPERMIN);
		cal.set(Calendar.MINUTE, mins);
		cal.set(Calendar.SECOND, secs);
		cal.set(Calendar.MILLISECOND, millis % 1000);
		Date gmtDate = cal.getTime();
		long gmtTime = gmtDate.getTime();

		// Find if there's transition within 24 hours of GMT time
		int tindex;
		for (tindex = 0; tindex < trans.length; tindex++) {
			if (Math.abs(gmtTime - trans[tindex].time)
			    < SECSPERDAY * 1000)
				break;
		}

		// If not, offset is going to be the same as with gmtDate
		if (tindex == trans.length)
			return getTimeInfo(gmtDate);

		// Figure out if the transition happened yet
		if (gmtTime - trans[tindex].info.offset >= trans[tindex].time
		    || tindex == 0)
			return trans[tindex].info;
		return trans[tindex - 1].info;
	}

	private TimeInfo getTimeInfo(Date date) {
		if (trans.length == 0)
			return defaultInfo;
		long when = date.getTime();
		int i;
		for (i = 0; i < trans.length - 1
		    && when >= trans[i + 1].time; i++);
		return trans[i].info;
	}

	// Read in a zone file in 'TZif' format
	private boolean read(DataInput data) throws IOException {

		// Check magic number
		switch (data.readInt()) {
		case 0:			// allow for backwards compatibility
		case TZ_MAGIC:
			break;
		default:
			return false;
		}

		// Skip header/reserved words
		data.readFully(new byte[16]);

		// Get initial counts
		int ttisgmtcnt = data.readInt();
		int ttisstdcnt = data.readInt();
		int leapcnt = data.readInt();
		int timecnt = data.readInt();
		int typecnt = data.readInt();
		int charcnt = data.readInt();

		// Sanity
		if (leapcnt < 0 || leapcnt > MAX_LEAPS
		    || typecnt <= 0 || typecnt > MAX_TYPES
		    || timecnt < 0 || timecnt > MAX_TIMES
		    || charcnt < 0 || charcnt > MAX_CHARS
		    || (ttisstdcnt != 0 && ttisstdcnt != typecnt)
		    || (ttisgmtcnt != 0 && ttisgmtcnt != typecnt))
			return false;

		// Read transition times
		trans = new Transition[timecnt];
		for (int i = 0; i < timecnt; i++) {
			trans[i] = new Transition();
			trans[i].time = data.readInt() * 1000L;
		}

		// Read time info indicies for each transition time
		int infoIndex[] = new int[timecnt];
		for (int i = 0; i < timecnt; i++) {
			infoIndex[i] = data.readUnsignedByte();
			if (infoIndex[i] >= typecnt)
				return false;
		}

		// Read time info's
		TimeInfo[] infos = new TimeInfo[typecnt];
		int[] abidx = new int[typecnt];
		for (int i = 0; i < typecnt; i++) {
			infos[i] = new TimeInfo();
			infos[i].offset = data.readInt() * 1000;
			infos[i].isDST = readBooleanStrict(data);
			abidx[i] = data.readUnsignedByte();
			if (abidx[i] < 0 || abidx[i] > charcnt)
				return false;
		}
		defaultInfo = infos[0];

		// Resolve time info indicies
		for (int i = 0; i < timecnt; i++)
			trans[i].info = infos[infoIndex[i]];

		// Read abbrieviation characters
		char[] abuf = new char[charcnt];
		for (int i = 0; i < charcnt; i++)
			abuf[i] = (char)data.readUnsignedByte();
		String astr = new String(abuf);

		// Resolve abbrieviation indicies
		for (int i = 0; i < typecnt; i++) {
			int end;
			for (end = abidx[i];
			    end < charcnt && abuf[end] != 0;
			    end++);
			infos[i].abbrev = astr.substring(abidx[i], end);
		}

		// Read leap second times
		leaps = new Leap[leapcnt];
		for (int i = 0; i < leapcnt; i++) {
			leaps[i] = new Leap();
			leaps[i].time = data.readInt() * 1000L;
			leaps[i].numSeconds = data.readInt();
		}

		// Read standard/wall indicators
		standard = new boolean[typecnt];
		if (ttisstdcnt != 0) {
			for (int i = 0; i < typecnt; i++)
				standard[i] = readBooleanStrict(data);
		}

		// Read UTC/local time indicators
		gmt = new boolean[typecnt];
		if (ttisgmtcnt != 0) {
			for (int i = 0; i < typecnt; i++)
				gmt[i] = readBooleanStrict(data);
		}

		// Done
		return true;
	}

	private boolean readBooleanStrict(DataInput data) throws IOException {
		switch (data.readByte()) {
		case 0:
			return false;
		case 1:
			return true;
		default:
			throw new IOException("invalid file contents");
		}
	}

	/**
	 * Test routine
	 */
	public static void main(String[] args) throws Exception {

		// Test cases
		int[][] tests = new int[100][];
		int num = 0;

		tests[num++] = new int[] { 2000, Calendar.APRIL, 2, 1, 30 };
		tests[num++] = new int[] { 2000, Calendar.APRIL, 2, 2, 30 };
		tests[num++] = new int[] { 2000, Calendar.APRIL, 2, 3, 30 };

		tests[num++] = new int[] { 2000, Calendar.OCTOBER, 29, 0, 30 };
		tests[num++] = new int[] { 2000, Calendar.OCTOBER, 29, 1, 30 };
		tests[num++] = new int[] { 2000, Calendar.OCTOBER, 29, 2, 30 };

		tests[num++] = new int[] { 0001, Calendar.JANUARY, 1, 0, 0 };
		tests[num++] = new int[] { 9999, Calendar.DECEMBER, 31, 0, 0 };

		// Get UNIXTimeZone by reading the specified file
		File file = null;
		switch (args.length) {
		case 1:
			file = new File(args[0]);
			break;
		default:
			System.err.println("Usage: UNIXTimeZone filename");
			System.exit(1);
		}
		UNIXTimeZone tz = new UNIXTimeZone("foobar", file);

		// Do tests
		System.out.println("Time zone uses daylight savings = "
		    + tz.useDaylightTime());
		for (int i = 0; i < tests.length && tests[i] != null; i++) {

			System.out.println("Date: "
			    +tests[i][0]+"/"+(tests[i][1]+1)+"/"+tests[i][2]
			    +" "+tests[i][3]+":"+tests[i][4]);

			// Determine offset at this time
			int offset = tz.getOffset(GregorianCalendar.AD,
			    tests[i][0],
			    tests[i][1],
			    tests[i][2],
			    0,
			    (tests[i][3] * SECSPERHOUR
				+ tests[i][4] * SECSPERMIN) * 1000);

			// Determine whether DST in effect at this time
			GregorianCalendar cal = new GregorianCalendar(tz);
			cal.set(Calendar.ERA, GregorianCalendar.AD);
			cal.set(Calendar.YEAR, tests[i][0]);
			cal.set(Calendar.MONTH, tests[i][1]);
			cal.set(Calendar.DAY_OF_MONTH, tests[i][2]);
			cal.set(Calendar.HOUR_OF_DAY, tests[i][3]);
			cal.set(Calendar.MINUTE, tests[i][4]);

			boolean dst = tz.inDaylightTime(cal.getTime());
			String ab = tz.getAbbreviation(cal.getTime());

			int secs = offset / 1000;
			boolean neg = secs < 0;
			if (neg)
				secs = -secs;
			int hours = secs / SECSPERHOUR;
			secs -= hours * SECSPERHOUR;
			int mins = secs / SECSPERMIN;
			secs -= mins * SECSPERMIN;
			System.out.print("  offset=" + (neg ? '-' : '+'));
			System.out.println(hours
			    + " hrs " + mins + " mins " + secs + " secs");
			System.out.println("  DST=" + dst);
			System.out.println("  Abbreviation=" + ab);
		}
	}
}

