/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;


final public class Long
  extends Number
{
	final private long value;
	final public static long MIN_VALUE = 0x8000000000000000L;
	final public static long MAX_VALUE = 0x7fffffffffffffffL;
	final public static Class TYPE = Class.getPrimitiveClass("long");

	/* This is what Sun's JDK1.1 "serialver java.lang.Long" spits out */ 
	private static final long serialVersionUID = 4290774380558885855L;

public Long(String s) throws NumberFormatException {
	this.value = valueOf(s).value;
}

public Long(long value) {
	this.value=value;
}

public double doubleValue() {
	return (double )value;
}

public boolean equals(Object obj) {
	return (obj != null) &&
		(obj instanceof Long) &&
		(((Long)obj).value == this.value);
}

public float floatValue() {
	return (float )value;
}

public static Long getLong(String nm) {
	return getLong(nm, (Long )null);
}

public static Long getLong(String nm, Long val) {
	String property = System.getProperty(nm);
	if (property != null) {
		long sign = 1L;
		if (property.startsWith("-")) {
			sign = -1L;
			property = property.substring(1);
		}
		if (property.equals("0")) {
			return (new Long(0L));
		}
		int base = 10;
		if (property.startsWith("0x")) {
			base = 16;
			property = property.substring(2);
		}
		else if (property.startsWith("#")) {
			base = 16;
			property = property.substring(1);
		}
		else if (property.startsWith("0")) {
			base = 8;
			property = property.substring(1);
		}
		try {
			return (new Long(Long.parseUnsignedLong(property, base, sign)));
		}
		catch (NumberFormatException e1) {
		}
	}
	return (val);
}

public static Long getLong(String nm, long val) {
	return getLong(nm, new Long(val));
}

public int hashCode() {
	return this.intValue();
}

public int intValue() {
	return (int )value;	
}

public long longValue() {
	return value;
}

public static long parseLong(String s) throws NumberFormatException {
	return parseLong(s, 10);
}

public static long parseLong(String s, int radix) throws NumberFormatException {
	if (s.length() <= 0) {
		throw new NumberFormatException();
	}

	/* Check for negativity */
	if (s.charAt(0) == '-') {
		return (parseUnsignedLong(s.substring(1), radix, -1L));
	}
	else {
		return (parseUnsignedLong(s, radix, 1L));
	}
}

private static long parseUnsignedLong(String s, int radix, long sign) throws NumberFormatException {
	long result = 0;
	for (int pos = 0; pos < s.length(); pos++) {
		int digit = Character.digit(s.charAt(pos), radix);
		if (digit == -1) {
			throw new NumberFormatException();
		}
		long nresult = (result * radix) + (sign * digit);
		if (nresult < 0 && sign == 1) {
			throw new NumberFormatException();
		}
		else if (nresult > 0 && sign == -1) {
			throw new NumberFormatException();
		}
		result = nresult;
	}
	return (result);
}

public static String toBinaryString ( long i ) {
	return toUnsignedString(i, 1);
}

public static String toHexString ( long i ) {
	return toUnsignedString(i, 4);
}

public static String toOctalString ( long i ) {
	return toUnsignedString(i, 3);
}

public String toString() {
	return toString(this.value);
}

public static String toString(long i) {
	return toString(i, 10);
}

public static String toString(long i, int radix) {
	if (i == 0) {
		return ("0");
	}

	if (radix < Character.MIN_RADIX || radix > Character.MAX_RADIX) {
		radix = 10;
	}

	StringBuffer buf = new StringBuffer();
	int sign = (i < 0) ? -1 : 1;
	while (i != 0) {
		char digit = Character.forDigit(Math.abs((int)(i % radix)), radix);
		i = i / (long)radix;
		buf.append(digit);
	}
	if (sign == -1) {
		buf.append('-');
	}
	buf.reverse();
	return (buf.toString());
}

private static String toUnsignedString(long i, int bits) {
	if (i == 0) {
		return ("0");
	}

	StringBuffer buf = new StringBuffer();
	int radix = 1 << bits, mask = radix-1;
	while (i != 0) {
		char digit = Character.forDigit(((int)i) & mask, radix);
		i >>>= bits;
		buf.append(digit);
	}
	buf.reverse();
	return (buf.toString());
}

public static Long valueOf(String s) throws NumberFormatException {
	return valueOf(s, 10);
}

public static Long valueOf(String s, int radix) throws NumberFormatException {
	return new Long(parseLong(s, radix));
}
}
