package java.text;

import java.util.Locale;
import java.util.ResourceBundle;

public abstract class NumberFormat extends Format implements Cloneable {

public static final int INTEGER_FIELD = 0;
public static final int FRACTION_FIELD = 1;

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
	try {
		NumberFormat other = (NumberFormat)obj;
		if (grouping == other.grouping && intonly == other.intonly && maxfrac == other.maxfrac && maxint == other.maxint && minfrac == other.minfrac && minint == other.minint) {
			return (true);
		}
	}
	catch (ClassCastException _) {
	}
	return (false);
}

public final String format(double num) {
	return (format(num, new StringBuffer(), new FieldPosition(0)).toString());
}

public final String format(long num) {
	return (format(num, new StringBuffer(), new FieldPosition(0)).toString());
}

public abstract StringBuffer format(double num, StringBuffer buf, FieldPosition pos);
public abstract StringBuffer format(long num, StringBuffer buf, FieldPosition pos);

public final StringBuffer format(Object num, StringBuffer buf, FieldPosition pos) {
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

public static synchronized Locale[] getAvailableLocales() {
	return (Format.getAvailableLocales("numberformat"));
}

public static final NumberFormat getCurrencyInstance() {
	return (getCurrencyInstance(Locale.getDefault()));
}

public static NumberFormat getCurrencyInstance(Locale loc) {
	ResourceBundle bundle = getResources("numberformat", loc);
	return (new DecimalFormat("\u00a40.00", loc));
}

public static final NumberFormat getInstance() {
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

public static final NumberFormat getNumberInstance() {
	return (getNumberInstance(Locale.getDefault()));
}

public static NumberFormat getNumberInstance(Locale loc) {
	ResourceBundle bundle = getResources("numberformat", loc);
	return (new DecimalFormat("0.#", loc));
}

public static final NumberFormat getPercentageInstance() {
	return (getPercentageInstance(Locale.getDefault()));
}

public static NumberFormat getPercentageInstance(Locale loc) {
	ResourceBundle bundle = getResources("numberformat", loc);
	return (new DecimalFormat("0%", loc));
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

public abstract Number parse(String str, ParsePosition pos);

public Number parse(String str) throws ParseException {
	ParsePosition pos = new ParsePosition(0);
	Number num = parse(str, pos);
	if (num == null) {
		throw new ParseException("", pos.getIndex());
	}
	return (num);
}

public final Object parseObject(String str, ParsePosition pos) {
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
