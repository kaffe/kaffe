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

final public class Integer
  extends Number
{
	private int value;
	final public static Class TYPE = Class.getPrimitiveClass("int");
	final public static int MIN_VALUE = 0x80000000;
	final public static int MAX_VALUE = 0x7fffffff;
	final private static long serialVersionUID = 1360826667806852920L;

public Integer ( String s ) throws NumberFormatException
{
	this.value = parseInt(s);
}

public Integer(int value)
{
	this.value = value;
}

public static Integer decode(String nm) throws NumberFormatException
{
	/* Strip off negative sign, if any */
	int sign = 1;
	if (nm.startsWith("-")) {
		sign = -1;
		nm = nm.substring(1);
	}
	if (nm.equals("0")) {
		return (new Integer(0));
	}

	/* Strip off base indicator, if any */
	int base = 10;
	if (nm.equals("0")) {
		return new Integer(0);
	}
	else if (nm.startsWith("0x")) {
		base = 16;
		nm = nm.substring(2);
	}
	else if (nm.startsWith("#")) {
		base = 16;
		nm = nm.substring(1);
	}
	else if (nm.startsWith("0")) {
		base = 8;
		nm = nm.substring(1);
	}
	/* A string like "0x-1234" must generate an error; disallow it here */
	if (nm.startsWith("-")) {
		throw new NumberFormatException();
	}
	return (new Integer(parseUnsignedInt(nm, base, sign)));
}

public double doubleValue() {
	return ((double)value);
}

public boolean equals(Object obj) {
	return (obj != null) &&
		(obj instanceof Integer) &&
		(((Integer)obj).value == this.value);
}

public float floatValue() {
	return (float )value;
}

public static Integer getInteger(String nm) {
	return getInteger(nm, (Integer)null);
}

public static Integer getInteger(String nm, Integer val) {
	String prop = System.getProperty(nm);
	if (prop != null) {
		try {
			return (decode(prop));
		}
		catch (NumberFormatException e) {
		}
	}
	return (val);
}

public static Integer getInteger(String nm, int val) {
	return getInteger(nm, new Integer(val));
}

public int hashCode() {
	return this.intValue();
}

public int intValue() {
	return value;
}

public long longValue() {
	return (long )value;
}

public static int parseInt(String s) throws NumberFormatException
{
	return (parseInt(s, 10));
}

public static int parseInt(String s, int radix) throws NumberFormatException
{
	if (s == null || s.length() <= 0) {
		throw new NumberFormatException();
	}

	/* Check for negativity */
	if (s.charAt(0)=='-') {
		return (parseUnsignedInt(s.substring(1), radix, -1));
	}
	else {
		return (parseUnsignedInt(s, radix, 1));
	}
}

private static int parseUnsignedInt(String s, int radix, int sign) throws NumberFormatException
{
	if (s == null || s.length() <= 0) {
		throw new NumberFormatException();
	}
	int result = 0;
	for (int pos = 0; pos < s.length(); pos++) {
		int digit = Character.digit(s.charAt(pos), radix);
		if (digit == -1) {
			throw new NumberFormatException();
		}
		int nresult = (result * radix) + (sign * digit);
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

public static String toBinaryString(int i) {
	return toUnsignedString(i, 1);
}

public static String toHexString(int i) {
	return toUnsignedString(i, 4);
}

public static String toOctalString(int i) {
	return toUnsignedString(i, 3);
}

public String toString() {
	return toString(this.value);
}

public static String toString(int i) {
	return toString(i, 10);
}

public static String toString(int i, int radix) {
	if (i == 0) {
		return ("0");
	}

	if (radix < Character.MIN_RADIX || radix > Character.MAX_RADIX) {
		radix = 10;
	}

	StringBuffer buf = new StringBuffer();
	int sign = (i < 0) ? -1 : 1;
	while (i != 0) {
		char digit = Character.forDigit(Math.abs(i % radix), radix);
		i = i / radix;
		buf.append(digit);
	}
	if (sign == -1) {
		buf.append('-');
	}
	buf.reverse();
	return (buf.toString());
}

private static String toUnsignedString(int i, int bits) {
	if (i == 0) {
		return ("0");
	}

	StringBuffer buf = new StringBuffer();
	int radix = 1 << bits, mask = radix-1;
	while (i != 0) {
		char digit = Character.forDigit(i & mask, radix);
		i >>>= bits;
		buf.append(digit);
	}
	buf.reverse();
	return (buf.toString());
}

public static Integer valueOf(String s) throws NumberFormatException
{
	return valueOf(s, 10);
}

public static Integer valueOf(String s, int radix) throws NumberFormatException
{
	return new Integer(parseInt(s, radix));
}
}
