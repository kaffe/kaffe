package java.text;

import java.lang.String;
import java.util.Locale;
import java.util.ResourceBundle;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
abstract public class NumberFormat
  extends Format
{
	private static final long serialVersionUID = -2308460125733713944L;
	final public static int INTEGER_FIELD = 0;
	final public static int FRACTION_FIELD = 1;
	boolean grouping;
	boolean intonly;
	int maxfrac;
	int maxint;
	int minfrac;
	int minint;

public NumberFormat() {
	grouping = false;
	intonly = false;
	maxfrac = 0;
	maxint = 0;
	minfrac = 0;
	minint = 1;
}

public Object clone() {
	return (super.clone());
}

public boolean equals(Object obj) {
	if (obj instanceof NumberFormat) {
		NumberFormat other = (NumberFormat)obj;
		if (grouping == other.grouping && intonly == other.intonly && maxfrac == other.maxfrac && maxint == other.maxint && minfrac == other.minfrac && minint == other.minint) {
			return (true);
		}
	}
	return (false);
}

final public StringBuffer format(Object num, StringBuffer buf, FieldPosition pos) {
	if (num instanceof Double) {
		return (format(((Double)num).doubleValue(), buf, pos));
	}
	else if (num instanceof Long) {
		return (format(((Long)num).longValue(), buf, pos));
	}
	else {
		buf.append("<unknown type>");
		return (buf);
	}
}

final public String format(double num) {
	return (format(num, new StringBuffer(), new FieldPosition(0)).toString());
}

abstract public StringBuffer format(double num, StringBuffer buf, FieldPosition pos);

final public String format(long num) {
	return (format(num, new StringBuffer(), new FieldPosition(0)).toString());
}

abstract public StringBuffer format(long num, StringBuffer buf, FieldPosition pos);

public static synchronized Locale[] getAvailableLocales() {
	return (Format.getAvailableLocales("numberformat"));
}

final public static NumberFormat getCurrencyInstance() {
	return (getCurrencyInstance(Locale.getDefault()));
}

public static NumberFormat getCurrencyInstance(Locale loc) {
	ResourceBundle bundle = getResources("numberformat", loc);
	String cs = bundle.getString("\u00a4");
	return (new DecimalFormat(cs + "#,##0.00;(" + cs + "#,##0.00)", loc));
}

final public static NumberFormat getInstance() {
	return (getInstance(Locale.getDefault()));
}

public static NumberFormat getInstance(Locale loc) {
	return (getNumberInstance(loc));
}

public int getMaximumFractionDigits() {
	return (maxfrac);
}

public int getMaximumIntegerDigits() {
	return (maxint);
}

public int getMinimumFractionDigits() {
	return (minfrac);
}

public int getMinimumIntegerDigits() {
	return (minint);
}

final public static NumberFormat getNumberInstance() {
	return (getNumberInstance(Locale.getDefault()));
}

public static NumberFormat getNumberInstance(Locale loc) {
	ResourceBundle bundle = getResources("numberformat", loc);
	return (new DecimalFormat("#,##0.###;-#,##0.###", loc));
}

final public static NumberFormat getPercentInstance() {
	return (getPercentInstance(Locale.getDefault()));
}

public static NumberFormat getPercentInstance(Locale loc) {
	ResourceBundle bundle = getResources("numberformat", loc);
	String cs = bundle.getString("%");
	return (new DecimalFormat("#,##0" + cs, loc));
}

public int hashCode() {
	return (super.hashCode());
}

public boolean isGroupingUsed() {
	return (grouping);
}

public boolean isParseIntegerOnly() {
	return (intonly);
}

public Number parse(String str) throws ParseException {
	ParsePosition pos = new ParsePosition(0);
	Number num = parse(str, pos);
	if (num == null) {
		throw new ParseException("", pos.getIndex());
	}
	return (num);
}

abstract public Number parse(String str, ParsePosition pos);

final public Object parseObject(String str, ParsePosition pos) {
	return (parse(str, pos));
}

public void setGroupingUsed(boolean val) {
	grouping = val;
}

public void setMaximumFractionDigits(int val) {
	maxfrac = val;
}

public void setMaximumIntegerDigits(int val) {
	maxint = val;
}

public void setMinimumFractionDigits(int val) {
	minfrac = val;
}

public void setMinimumIntegerDigits(int val) {
	minint = val;
}

public void setParseIntegerOnly(boolean val) {
	intonly = val;
}
}
