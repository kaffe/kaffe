package java.lang;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

public final class Byte extends Number implements Comparable
{
	final public static byte MIN_VALUE = -0x80;
	final public static byte MAX_VALUE = 0x7F;
	final public static Class TYPE = Class.getPrimitiveClass("byte");
	final private byte value;

	/* This is what Sun's JDK1.1 "serialver java.lang.Byte" spits out */
	private static final long serialVersionUID = -7183698231559129828L;

public Byte(String s) throws NumberFormatException {
	this.value = parseByte(s);
}

public Byte(byte value) {
	this.value = value;
}

public byte byteValue() {
	return (value);
}

public static Byte decode(String nm) throws NumberFormatException {
	int val = Integer.decode(nm).intValue();
	if (val < MIN_VALUE || val > MAX_VALUE)
	    throw new NumberFormatException();
	return new Byte((byte) val);
}

public double doubleValue() {
	return ((double)value);
}

public boolean equals(Object obj) {
	return (obj instanceof Byte)
	    && (((Byte) obj).value == this.value);
}

public float floatValue() {
	return ((float)value);
}

public int hashCode() {
	return (value);
}

public int intValue() {
	return ((int)value);
}

public long longValue() {
	return ((long)value);
}

public static byte parseByte(String s) throws NumberFormatException {
	return (parseByte(s, 10));
}

public static byte parseByte(String s, int radix) throws NumberFormatException {
	int val = Integer.parseInt(s, radix);
	if (val < MIN_VALUE || val > MAX_VALUE) {
		throw new NumberFormatException();
	}
	return (byte) val;
}

public String toString() {
	return (toString(value));
}

public static String toString(byte b) {
	return Integer.toString((int)b);
}

public static Byte valueOf(String s) throws NumberFormatException {
	return (new Byte(parseByte(s)));
}

public static Byte valueOf(String s, int radix) throws NumberFormatException {
	return (new Byte(parseByte(s, radix)));
}

public int compareTo(Byte b) {
	return (int)value - (int)b.value;
}

public int compareTo(Object o) {
	return compareTo((Byte) o);
}
}
