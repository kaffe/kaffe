/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.text;

import java.util.TimeZone;
import java.util.Locale;
import java.util.Date;
import java.util.ResourceBundle;
import java.util.Calendar;

public abstract class DateFormat extends Format {

private static final long serialVersionUID = 7218322306649953788L;
public final static int FULL = 0;
public final static int LONG = 1;
public final static int MEDIUM = 2;
public final static int SHORT = 3;
public final static int DEFAULT = MEDIUM;

public final static int ERA_FIELD = 0;
public final static int YEAR_FIELD = 1;
public final static int MONTH_FIELD = 2;
public final static int DATE_FIELD = 3;
public final static int HOUR_OF_DAY1_FIELD = 4;
public final static int HOUR_OF_DAY0_FIELD = 5;
public final static int MINUTE_FIELD = 6;
public final static int SECOND_FIELD = 7;
public final static int MILLISECOND_FIELD = 8;
public final static int DAY_OF_WEEK_FIELD = 9;
public final static int DAY_OF_YEAR_FIELD = 10;
public final static int DAY_OF_WEEK_IN_MONTH_FIELD = 11;
public final static int WEEK_OF_YEAR_FIELD = 12;
public final static int WEEK_OF_MONTH_FIELD = 13;
public final static int AM_PM_FIELD = 14;
public final static int HOUR1_FIELD = 15;
public final static int HOUR0_FIELD = 16;
public final static int TIMEZONE_FIELD = 17;

/* locales in kaffe.text.dateformat */
private static final Locale [] LOCALES = new Locale[] {
	Locale.US
};

protected Calendar calendar;
protected NumberFormat numberFormat;

private boolean lenient = true;

protected DateFormat() {
}

public Object clone() {
	return (super.clone());
}

public boolean equals(Object obj) {
	if (obj instanceof DateFormat) {
		DateFormat other = (DateFormat)obj;
		if (calendar == other.calendar && numberFormat == other.numberFormat) {
			return (true);
		}
	}
	return (false);
}

public final StringBuffer format(Object obj, StringBuffer buf, FieldPosition pos) {
	Date date;

	if (obj instanceof Number) {
		date = new Date(((Number)obj).intValue());
	}
	else {
		date = (Date)obj;
	}
	return (format(date, buf, pos));
}

public abstract StringBuffer format (Date date, StringBuffer buf, FieldPosition pos);

public final String format(Date date) {
	StringBuffer buf = format(date, new StringBuffer(), new FieldPosition(0));
	return (buf.toString());
}

public final static DateFormat getDateInstance() {
	return (getDateInstance(DEFAULT, Locale.getDefault()));
}

public final static DateFormat getDateInstance(int style) {
	return (getDateInstance(style, Locale.getDefault()));
}

public final static DateFormat getDateInstance(int style, Locale loc) {
	ResourceBundle bundle = getResources("dateformat", loc);
	String time = ((String[])bundle.getObject("date"))[style];
	return (new SimpleDateFormat(time, loc));
}

public final static DateFormat getDateTimeInstance() {
	return (getDateTimeInstance(DEFAULT, DEFAULT, Locale.getDefault()));
}

public final static DateFormat getDateTimeInstance(int dateStyle, int timeStyle) {
	return (getDateTimeInstance(dateStyle, timeStyle, Locale.getDefault()));
}

public final static DateFormat getDateTimeInstance(int dateStyle, int timeStyle, Locale loc) {
	ResourceBundle bundle = getResources("dateformat", loc);
	String date = ((String[])bundle.getObject("date"))[dateStyle];
	String time = ((String[])bundle.getObject("time"))[timeStyle];
	return (new SimpleDateFormat(date + " " + time, loc));
}

public final static DateFormat getTimeInstance() {
	return (getTimeInstance(DEFAULT, Locale.getDefault()));
}

public final static DateFormat getTimeInstance(int style) {
	return (getTimeInstance(style, Locale.getDefault()));
}

public final static DateFormat getTimeInstance(int style, Locale loc) {
	ResourceBundle bundle = getResources("dateformat", loc);
	String time = ((String[])bundle.getObject("time"))[style];
	return (new SimpleDateFormat(time, loc));
}

public final static DateFormat getInstance() {
	return (getDateTimeInstance());
}

public NumberFormat getNumberFormat() {
	return (numberFormat);
}

public TimeZone getTimeZone() {
	return (calendar.getTimeZone());
}

public int hashCode() {
	return (super.hashCode());
}

public boolean isLenient() {
	return (lenient);
}

public Date parse(String src) throws ParseException {
	ParsePosition pos = new ParsePosition(0);
	Date date = parse(src, pos);
	if (date == null) {
		throw new ParseException("", pos.getIndex());
	}
	return (date);
}

public abstract Date parse(String src, ParsePosition pos);

public Object parseObject(String source, ParsePosition status) {
	return parse(source, status);
}

public Date parse(Object src, ParsePosition pos) {
	return (parse((String)src, pos));
}

public static Locale[] getAvailableLocales() {
	return (LOCALES);
}

public void setCalendar(Calendar cal) {
	calendar = cal;
}

public Calendar getCalendar() {
	return (calendar);
}

public void setLenient(boolean val) {
	lenient = val;
}

public void setNumberFormat(NumberFormat form) {
	numberFormat = form;
}

public void setTimeZone(TimeZone zone) {
	calendar.setTimeZone(zone);
}

}
