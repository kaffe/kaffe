/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
package java.text;

import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.Locale;
import java.util.TimeZone;
import java.util.ResourceBundle;

public class SimpleDateFormat
  extends DateFormat
{
	private static final long serialVersionUID = 4774881970558875024L;
	private static final String DEFAULTPATTERNCHARS = "GyMdkHmsSEDFwWahKz";
	private DateFormatSymbols syms;
	private String pattern;

private static String getDefaultPattern(Locale loc)
{
	ResourceBundle bundle = getResources("dateformat", loc);
	String date = ((String[])bundle.getObject("date"))[DateFormat.DEFAULT];
	String time = ((String[])bundle.getObject("time"))[DateFormat.DEFAULT];
	return date + " " + time;
}
    
public SimpleDateFormat() {
	this(getDefaultPattern(Locale.getDefault()), Locale.getDefault());
}

public SimpleDateFormat(String pattern) {
	this(pattern, Locale.getDefault());
}

public SimpleDateFormat(String pattern, DateFormatSymbols syms) {
	this.syms = syms;
	this.pattern = pattern;
	this.calendar = new GregorianCalendar();
	this.numberFormat = new DecimalFormat("0");
}

public SimpleDateFormat(String pattern, java.util.Locale loc) {
	this.syms = new DateFormatSymbols(loc);
	this.pattern = pattern;
	this.calendar = new GregorianCalendar(loc);
	this.numberFormat = new DecimalFormat("0", loc);
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
				val++;
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
			if (plen > 1 && val < 10) {
				buf.append('0');
			}
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
				int ro = zone.getOffset(calendar.get(Calendar.ERA),
							calendar.get(Calendar.YEAR),
							calendar.get(Calendar.MONTH),
							calendar.get(Calendar.DAY_OF_MONTH),
							calendar.get(Calendar.DAY_OF_WEEK),
							calendar.get(Calendar.MILLISECOND)) / 60000;
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

private static String NUMERIC_PATTERN = "ydhHmsSDFwWkK";
private static int[] NUMERIC_FIELD = {
	Calendar.MONTH,
	Calendar.YEAR, Calendar.DAY_OF_MONTH,
	Calendar.HOUR, Calendar.HOUR_OF_DAY,
	Calendar.MINUTE, Calendar.SECOND, Calendar.MILLISECOND,
	Calendar.DAY_OF_YEAR, Calendar.DAY_OF_WEEK_IN_MONTH,
	Calendar.WEEK_OF_YEAR, Calendar.DAY_OF_WEEK_IN_MONTH,
	Calendar.HOUR_OF_DAY, Calendar.HOUR
};
	
public Date parse(String source, ParsePosition pos) {
	calendar.clear();
	numberFormat.setParseIntegerOnly(true);
	numberFormat.setGroupingUsed(false);

	pos.setErrorIndex(-1);

	int startIndex = pos.getIndex();
	int index = startIndex;
	int endIndex = source.length();
	boolean ambigousYear = false;

	char[] patt = pattern.toCharArray();
	for (int i = 0; i < patt.length; ) {
		char letter = patt[i++];

		if (letter == '\'') {
			// quoted text
			if ((i < patt.length) && (patt[i] == '\'')) {
				if ((index >= endIndex) ||
				    (source.charAt(index) != '\'')) {
					pos.setIndex(startIndex);
					pos.setErrorIndex(index);
					return null;
				}
				index++;
				i++;
			}
			else {
				while ((i < patt.length) && patt[i] != '\'') {
					if ((index >= endIndex) ||
					    (source.charAt(index) != patt[i])) {
						pos.setIndex(startIndex);
						pos.setErrorIndex(index);
						return null;
					}
					index++;
					i++;
				}
				i++;
			}
		}
		else if (((letter >= 'a') && (letter <= 'z')) ||
			 ((letter >= 'A') && (letter <= 'Z'))) {
			// pattern Code
			int plen = 1;
			int val = -1;
			
			// count pattern len
			while ((i < patt.length) && (patt[i] == letter)) {
				plen++;
				i++;
			}
			
			// skip blanks
			for (;;) {
				if (index >= endIndex) {
					pos.setIndex(startIndex);
					pos.setErrorIndex(index);
					return null;
				}
				char c = source.charAt(index);
				if ((c != ' ') && (c != '\t'))
					break;
				index++;
			}
			pos.setIndex(index);

			// handle numeric values
			if ((NUMERIC_PATTERN.indexOf(letter) != -1) ||
			    ((letter == 'M') && (plen <= 2))) {
				Number number;
				if ((i < patt.length) &&
				    (DEFAULTPATTERNCHARS.indexOf(patt[i]) != -1)) {
					if (index + plen > endIndex) {
						pos.setIndex(startIndex);
						pos.setErrorIndex(index);
						return null;
					}
					number = numberFormat.parse(source.substring(0, index + plen), pos);
				}
				else {
					number = numberFormat.parse(source, pos);
				}
				if (number == null) {
					pos.setIndex(startIndex);
					pos.setErrorIndex(index);
					return null;
				}
				val = number.intValue();
				switch (letter) {
				case 'y':
					// 2 digits and only 2 digits
					ambigousYear = (plen < 3) &&
						(val >= 0) &&
						(pos.getIndex() - index <= 2);
					break;
				case 'M':
					val--;
					break;
				case 'k':
					if (val == 24)
						val = 0;
					break;
				case 'h':
					if (val == 12)
						val = 0;
					break;
				}
				calendar.set (NUMERIC_FIELD[NUMERIC_PATTERN.indexOf(letter) + 1],
					      val);
				index = pos.getIndex();
			}
			// handle strings values
			else {
				switch (letter) {
				case 'G':
					index = parseField (source, index, Calendar.ERA, syms.eras);
					break;
				case 'M':
					index = parseField (source, index, Calendar.MONTH, syms.months);
					if (index <= 0)
						index = parseField (source, -index, Calendar.MONTH, syms.shortMonths);
					break;
				case 'E':
					index = parseField (source, index, Calendar.DAY_OF_WEEK, syms.weekdays);
					if (index <= 0)
						index = parseField (source, -index, Calendar.DAY_OF_WEEK, syms.shortWeekdays);
					break;
				case 'a':
					index = parseField (source, index, Calendar.AM_PM, syms.amPmStrings);
					break;
				case 'z':
					index = parseTimeZone (source, index, endIndex);
					break;
				default:
					index = -index;
				}
				if (index <= 0) {
					pos.setIndex(startIndex);
					pos.setErrorIndex(-index);
					return null;
				}
				pos.setIndex(index);
			}
		}
		else {
			// match litteral
			if ((index >= endIndex) ||
			    (source.charAt(index) != letter)) {
				pos.setIndex(startIndex);
				pos.setErrorIndex(index);
				return null;
			}
			index++;
		}
	}

	if (ambigousYear && calendar.isSet(Calendar.YEAR)) {
		// adjust century to be within 80 years before and 20
		// years after current time
		int current = Calendar.getInstance().get(Calendar.YEAR);
		int epoch = current - current % 100;
		int year = epoch + calendar.get(Calendar.YEAR);
		if (year > current + 20)
			year -= 100;
		else if (year < current - 80)
			year += 100;
		calendar.set (Calendar.YEAR, year);
	}
	
	pos.setIndex (index);
	return calendar.getTime();
}

private int parseField (String source, int start, int field, String[] data) {
	int best = -1;
	int bestLen = 0;
	
	for (int i = data.length; i-- > 0; ) {
		int len = data[i].length();
		if ((len > bestLen) &&
		    source.regionMatches (true, start, data[i], 0, len)) {
			best = i;
			bestLen = len;
		}
	}

	if (best >= 0) {
		calendar.set (field, best);
		return start + bestLen;
	}
	return -start;
}

private int parseTimeZone (String source, int start, int endIndex) {
	// XXX better handle DST_OFFSET if tz.useDaylightTime() ?
	calendar.set(Calendar.DST_OFFSET, 0);

	if (source.regionMatches (start, "GMT", 0, 3)) {
		int index = start + 3;
		int sign = 0;

		if (index < endIndex) {
			switch (source.charAt(index)) {
			case '+':
				sign = +1;
				break;
			case '-':
				sign = -1;
			}
		}
		if (sign == 0) {
			calendar.set(Calendar.ZONE_OFFSET, 0);
			return index;
		}

		ParsePosition pos = new ParsePosition(index + 1);
		Number num = numberFormat.parse (source, pos);
		if (num == null)
			return -start;
		int offset = num.intValue();
		if ((pos.getIndex() < endIndex) &&
		    (source.charAt(pos.getIndex()) == ':')) {
			// GMT+hh:mm
			offset *= 60;
			pos.setIndex(pos.getIndex() + 1);
			num = numberFormat.parse (source, pos);
			if (num == null)
				return -start;
			offset += num.intValue();
		}
		else {
			// GMT+hh[mm]
			if (offset < 24)
				offset *= 60;
			else
				offset = (offset / 100) * 60 + offset % 100;
		}
		offset *= 60 * 1000;
		if (sign == -1)
			offset = -offset;
		calendar.set(Calendar.ZONE_OFFSET, offset);
		return pos.getIndex();
	}

	// search localized timezone
	int bestZone = -1;
	int bestLen = 0;
	int bestType = 0;
	for (int i = 0; i < syms.zoneStrings.length; i++) {
		String[] zone = syms.zoneStrings[i];

		for (int j = 1; j <= 4; j++) {
			int len = zone[j].length();

			if ((len > bestLen) &&
			    source.regionMatches (true, start, zone[j], 0, len)) {
				bestZone = i;
				bestType = j;
				bestLen = len;
			}
		}
	}
	if (bestZone != -1) {
		TimeZone tz = TimeZone.getTimeZone (syms.zoneStrings[bestZone][0]);
		calendar.set(Calendar.ZONE_OFFSET, tz.getRawOffset());
		return start + bestLen;
	}

	// try RFC 822 +hhmm format
	DecimalFormat rfc822 = new DecimalFormat("+####;-####");
	rfc822.setParseIntegerOnly(true);
	rfc822.setGroupingUsed(false);
	ParsePosition pos = new ParsePosition (start);
	Number num = rfc822.parse (source, pos);
	if (num == null)
		return -start;

	int offset = num.intValue();
	int sign = 1;
	if (offset < 0) {
		sign = -1;
		offset = -offset;
	}
	if (offset < 24)
		offset *= 60;
	else
		offset = (offset / 100) * 60 + offset % 100;

	offset *= 60 * 1000;
	if (sign == -1)
		offset = -offset;
	calendar.set(Calendar.ZONE_OFFSET, offset);
	return pos.getIndex();
}

public void setDateFormatSymbols(DateFormatSymbols syms) {
	this.syms = syms;
}

public String toLocalizedPattern() {
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
