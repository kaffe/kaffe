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
	private long value;
	final public static long MIN_VALUE = 0x8000000000000000L;
	final public static long MAX_VALUE = 0x7fffffffffffffffL;
	final public static Class TYPE = Class.getPrimitiveClass("long");

public Long(String s) throws NumberFormatException {
	this(valueOf(s).intValue());
}

public Long(long value) {
	this.value=value;
}

public double doubleValue() {
	return (double )value;
}

public boolean equals(Object obj) {
	if ((obj!=null) && (obj instanceof Long)) {
		return (this.longValue()==((Long )obj).longValue());
	}
	else {
		return false;
	}
}

public float floatValue() {
	return (float )value;
}

public static Long getLong(String nm) {
	return getLong(nm, (Long )null);
}

public static Long getLong(String nm, Long val) {
	String arg;

	if (val==null) arg=null; else arg=val.toString();

	String property=System.getProperty(nm, arg);
	if (property==null) return val;
	else {
		String toParse;
		int radixToParse;

		/* Exception rules in the definition */

		if (property.startsWith("0x-")) return val;
		if (property.startsWith("#-")) return val;
		if (property.endsWith("l") || property.endsWith("L")) return val;

		if (property.startsWith("0x")) {
			toParse=property.substring(2);
			radixToParse=16;
		}
		else if (property.startsWith("#")) {
			toParse=property.substring(1);
			radixToParse=16;
		}
		else if (property.startsWith("0")) {
			toParse=property.substring(1);
			radixToParse=8;
		}
		else {
			toParse=property;
			radixToParse=10;
		}

		if (toParse.length()==0) {
			return val;
		}
		else {
			try {
				return Long.valueOf(toParse, radixToParse);
			}
			catch (NumberFormatException e1) {
				return val;
			}
		}
	}
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

public static long parseLong(String s, int radix) {
	if (s.length()<=0) throw new NumberFormatException();

	/* Check for negativity */
	if (s.charAt(0)=='-') {
		return parseLong(s.substring(1))*-1;
	}
	else {
		long result=0;
		int position;

		for (position=0; position<s.length(); position++) {
			int digit=Character.digit(s.charAt(position), radix);
			if (digit==-1) throw new NumberFormatException();

			result=(result*radix)+digit;
		}

		return result;
	}			
}

public static String toBinaryString ( long i ) {
	return toString(i, 2);
}

public static String toHexString ( long i ) {
	return toString(i, 16);
}

public static String toOctalString ( long i ) {
	return toString(i, 8);
}

public String toString() {
	return toString(this.longValue());
}

public static String toString(long i) {
	return toString(i, 10);
}

public static String toString(long i, int radix)
	{
	if (i == 0) {
		return ("0");
	}

	if (radix < Character.MIN_RADIX || radix > Character.MAX_RADIX) {
		radix = 10;
	}

	StringBuffer buf = new StringBuffer();
	int sign = 1;
	if (i < 0) {
		sign = -1;
	}
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

public static Long valueOf(String s) throws NumberFormatException {
	return valueOf(s, 10);
}

public static Long valueOf(String s, int radix) throws NumberFormatException {
	return new Long(parseLong(s, radix));
}
}
