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

import java.lang.String;
import kaffe.util.NotImplemented;

public class ChoiceFormat extends NumberFormat {

private double[] limits;
private String[] strings;

public ChoiceFormat(String patt) {
	applyPattern(patt);
}

public ChoiceFormat(double[] limits, String[] strings) {
	setChoices(limits, strings);
}

public void applyPattern(String patt) {
	int len = patt.length();
	int argcount = 0;
	for (int i = 0; i < len; i++) {
		if (patt.charAt(i) == '|') {
			argcount++;
		}
		// We need to add an extra 'phantom' argument for '<'
		else if (patt.charAt(i) == '<') {
			argcount++;
		}
	}
	limits = new double[argcount];
	strings = new String[argcount];

	int s = 0;
	int c = 0;
	boolean needphantom = false;
	for (int i = 0; i < len; i++) {
		char ch = patt.charAt(i);
		if (ch == '#') {
			limits[c] = Double.valueOf(patt.substring(s, i)).doubleValue();
			s = c+1;
		}
		else if (ch == '<') {
			needphantom = true;
			limits[c] = Double.valueOf(patt.substring(s, i)).doubleValue();
			s = c+1;
		}
		else if (ch == '|') {
			strings[c] = patt.substring(s, i);
			c++;
			if (needphantom) {
				needphantom = false;
				limits[c] = nextDouble(limits[c-1]);
				strings[c] = strings[c-1];
				c++;
			}
		}
	}
}

public Object clone() {
	return (new ChoiceFormat(limits, strings));
}

public boolean equals(Object obj) {
	try {
		ChoiceFormat other = (ChoiceFormat)obj;
		for (int i = 0; i < limits.length; i++) {
			if (limits[i] != other.limits[i] || !strings[i].equals(other.strings[i])) {
				return (false);
			}
		}
		return (true);
	}
	catch (ClassCastException _) {
		return (false);
	}
}

public StringBuffer format(long num, StringBuffer buf, FieldPosition ign) {
	return (format((double)num, buf, ign));
}

public StringBuffer format(double num, StringBuffer buf, FieldPosition ign) {
	if (num < limits[0]) {
		buf.append(strings[0]);
		return (buf);
	}

	int len = limits.length - 1;
	for (int i = 0; i < len; i++) {
		if (limits[i] <= num && num < limits[i+1]) {
			buf.append(strings[i]);
			return (buf);
		}
	}

	buf.append(strings[len]);
	return (buf);
}

public Object[] getFormats() {
	return (strings);
}

public double[] getLimits() {
	return (limits);
}

public int hashCode() {
	return (super.hashCode());
}

public final static double nextDouble(double d) {
	return (d + Double.MIN_VALUE);
}

public final static double previousDouble(double d) {
	return (d - Double.MIN_VALUE);
}

public static double nextDouble(double d, boolean next) {
	if (next == true) {
		return (nextDouble(d));
	}
	else {
		return (previousDouble(d));
	}
}

public Number parse(String str, ParsePosition pos) {
	throw new NotImplemented();
}

public void setChoices(double[] limits, String[] strings) {
	this.limits = limits;
	this.strings = strings;
}

public String toPattern() {
	throw new NotImplemented();
}

}
