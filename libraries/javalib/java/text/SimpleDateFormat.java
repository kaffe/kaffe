package java.text;

import java.lang.String;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.Locale;
import java.util.TimeZone;
import kaffe.util.DateParser;
import kaffe.util.NotImplemented;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class SimpleDateFormat
  extends DateFormat
{
	final private static String DEFAULTPATTERNCHARS = "GyMdkHmsSEDFwWahKz";
	private DateFormatSymbols syms;
	private String pattern;

public SimpleDateFormat() {
	this("", Locale.getDefault());
}

public SimpleDateFormat(String pattern) {
	this(pattern, Locale.getDefault());
}

public SimpleDateFormat(String pattern, DateFormatSymbols syms) {
	this.syms = syms;
	this.pattern = pattern;
	this.calendar = new GregorianCalendar();
	this.format = new DecimalFormat("0");
}

public SimpleDateFormat(String pattern, java.util.Locale loc) {
	this.syms = new DateFormatSymbols(loc);
	this.pattern = pattern;
	this.calendar = new GregorianCalendar(loc);
	this.format = new DecimalFormat("0", loc);
}

public void applyLocalizedPattern(String pattern) {
	StringBuffer buf = new StringBuffer();
	String locals = syms.getLocalPatternChars();

	for (int i = 0; i < pattern.length(); i++) {
		char letter = pattern.charAt(i);
		int idx = locals.indexOf(letter);
		if (idx >= 0) {
			buf.append(DEFAULTPATTERNCHARS.charAt(idx));
		}
		else {
			buf.append(letter);
			if (letter == '\'') {
				do {
					i++;
					letter = pattern.charAt(i);
					buf.append(letter);
				} while (letter != '\'');
			}
		}
	}

	this.pattern = buf.toString();
}

public void applyPattern(String pattern) {
	this.pattern = pattern;
}

public Object clone() {
	return (super.clone());
}

public boolean equals(Object obj) {
	return (super.equals(obj));
}

public StringBuffer format(Date date, StringBuffer buf, FieldPosition pos) {
	calendar.setTime(date);

	char[] patt = pattern.toCharArray();
	for (int i = 0; i < patt.length; ) {
		int plen = 0;
		char letter = patt[i];
		i++;
		if (letter != '\'') {
			for (plen++; i < patt.length && patt[i] == letter; plen++, i++);
		}
		int cpos = buf.length();
		int val;
		switch (letter) {
		case 'G':
			val = calendar.get(Calendar.ERA);
			buf.append(syms.eras[val]);
			if (pos.field == ERA_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'y':
			val = calendar.get(Calendar.YEAR);
			if (plen < 4) {
				val = val % 100;
				if (val < 10) {
					buf.append('0');
				}
			}
			buf.append(val);
			if (pos.field == YEAR_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'M':
			val = calendar.get(Calendar.MONTH);
			if (plen < 3) {
				if (val < 10 && plen == 2) {
					buf.append('0');
				}
				buf.append(val);
			}
			else if (plen == 3) {
				buf.append(syms.shortMonths[val]);
			}
			else {
				buf.append(syms.months[val]);
			}
			if (pos.field == MONTH_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'd':
			val = calendar.get(Calendar.DAY_OF_MONTH);
			if (plen > 1 && val < 10) {
				buf.append('0');
			}
			buf.append(val);
			if (pos.field == DATE_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'k':
			val = calendar.get(Calendar.HOUR_OF_DAY);
			if (val == 0) {
				val = 24;
			}
			buf.append(val);
			if (pos.field == HOUR_OF_DAY1_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'H':
			val = calendar.get(Calendar.HOUR_OF_DAY);
			buf.append(val);
			if (pos.field == HOUR_OF_DAY0_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'm':
			val = calendar.get(Calendar.MINUTE);
			if (val < 10) {
				buf.append('0');
			}
			buf.append(val);
			if (pos.field == MINUTE_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 's':
			val = calendar.get(Calendar.SECOND);
			if (val < 10) {
				buf.append('0');
			}
			buf.append(val);
			if (pos.field == SECOND_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'S':
			val = calendar.get(Calendar.MILLISECOND);
			buf.append(val);
			if (pos.field == MILLISECOND_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'E':
			val = calendar.get(Calendar.DAY_OF_WEEK);
			if (plen < 4) {
				buf.append(syms.shortWeekdays[val]);
			}
			else {
				buf.append(syms.weekdays[val]);
			}
			if (pos.field == DAY_OF_WEEK_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'D':
			val = calendar.get(Calendar.DAY_OF_YEAR);
			buf.append(val);
			if (pos.field == DAY_OF_YEAR_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'F':
			val = calendar.get(Calendar.DAY_OF_WEEK_IN_MONTH);
			buf.append(val);
			break;
		case 'w':
			val = calendar.get(Calendar.WEEK_OF_YEAR);
			buf.append(val);
			if (pos.field == WEEK_OF_YEAR_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'W':
			val = calendar.get(Calendar.WEEK_OF_MONTH);
			buf.append(val);
			if (pos.field == WEEK_OF_MONTH_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'a':
			val = calendar.get(Calendar.AM_PM);
			buf.append(syms.amPmStrings[val]);
			if (pos.field == AM_PM_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'h':
			val = calendar.get(Calendar.HOUR);
			if (val == 0) {
				val = 12;
			}
			if ( (plen > 1) && (val < 10) )
				buf.append('0');
			buf.append(val);
			if (pos.field == HOUR1_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'K':
			val = calendar.get(Calendar.HOUR);
			buf.append(val);
			if (pos.field == HOUR0_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case 'z':
			TimeZone zone = calendar.getTimeZone();
			String szone = zone.getID();
			boolean daylight = zone.inDaylightTime(date);
			int j;
			for (j = 0; j < syms.zoneStrings.length; j++) {
				String[] sel = syms.zoneStrings[j];
				if (!szone.equals(sel[0])) {
					continue;
				}
				if (plen < 4) {
					if (daylight) {
						buf.append(sel[4]);
					}
					else {
						buf.append(sel[2]);
					}
				}
				else {
					if (daylight) {
						buf.append(sel[3]);
					}
					else {
						buf.append(sel[1]);
					}
				}
				break;
			}
			// If no matching timezone, put in GMT info.
			if (j == syms.zoneStrings.length) {
				buf.append("GMT");
				int ro = zone.getRawOffset() / 60000;
				if (ro < 0) {
					ro = Math.abs(ro);
					buf.append("-");
				}
				else {
					buf.append("+");
				}
				buf.append(ro / 60);
				buf.append(":");
				if (ro % 60 < 10) {
					buf.append("0");
				}
				buf.append(ro % 60);
			}
			if (pos.field == TIMEZONE_FIELD) {
				pos.begin = cpos;
				pos.end = buf.length();
			}
			break;
		case '\'':
			if (patt[i] == '\'') {
				buf.append('\'');
				i++;
			}
			else {
				while (patt[i] != '\'') {
					buf.append(patt[i]);
					i++;
				}
				i++;
			}
			break;
		default:
			for (j = 0; j < plen; j++) {
				buf.append(letter);
			}
			break;
		}
	}

	return (buf);
}

public DateFormatSymbols getDateFormatSymbols() {
	return (syms);
}

public int hashCode() {
	return (super.hashCode());
}

public Date parse(String source, ParsePosition pos) {
	try {
		return DateParser.parse( source, syms);
	}
	catch ( ParseException _x) {
		return null;
	}
}


public void setDateFormatSymbols(DateFormatSymbols syms) {
	this.syms = syms;
}

public String toLocalizePattern() {
	StringBuffer buf = new StringBuffer();
	String locals = syms.getLocalPatternChars();

	for (int i = 0; i < pattern.length(); i++) {
		int idx = DEFAULTPATTERNCHARS.indexOf(pattern.charAt(i));
		if (idx >= 0) {
			buf.append(locals.charAt(idx));
		}
		else {
			buf.append(pattern.charAt(i));
		}
	}

	return (buf.toString());
}

public String toPattern() {
	return (pattern);
}
}
