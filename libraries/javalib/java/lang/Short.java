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
final public class Short
  extends Number
{
	final public static short MIN_VALUE = -0x8000;
	final public static short MAX_VALUE = 0x7FFF;
	final public static Class TYPE = Class.getPrimitiveClass("short");
	private short val;

public Short(String s) throws NumberFormatException
{
	this(parseShort(s));
}

public Short(short value)
	{
	val = value;
}

public byte byteValue()
	{
	return ((byte)val);
}

public static Short decode(String nm) throws NumberFormatException
{
	short val;

	if (nm.startsWith("0x")) {
		val = parseShort(nm.substring(2), 16);
	}
	else if (nm.startsWith("#")) {
		val = parseShort(nm.substring(1), 16);
	}
	else if (nm.startsWith("0")) {
		val = parseShort(nm.substring(1), 8);
	}
	else {
		val = parseShort(nm.substring(1), 10);
	}

	return (new Short(val));
}

public double doubleValue()
	{
	return ((double)val);
}

public boolean equals(Object obj)
	{
	try {
		if (((Short)obj).val == val) {
			return (true);
		}
	}
	catch (ClassCastException _) {
	}
	return (false);
}

public float floatValue()
	{
	return ((float)val);
}

public int hashCode()
	{
	return (val);	// What should this be do you suppose ???
}

public int intValue()
	{
	return ((int)val);
}

public long longValue()
	{
	return ((long)val);
}

public static short parseShort(String s) throws NumberFormatException
{
	return ((short)parseShort(s, 10));
}

public static short parseShort(String s, int radix) throws NumberFormatException
{
	return ((short)Integer.parseInt(s, radix));
}

public short shortValue()
	{
	return ((short)val);
}

public String toString()
	{
	return (toString(val));
}

public static String toString(short s)
	{
	return Integer.toString((int)s);
}

public static Short valueOf(String s) throws NumberFormatException
{
	return (new Short(parseShort(s)));
}

public static Short valueOf(String s, int radix) throws NumberFormatException
{
	return (new Short(parseShort(s, radix)));
}
}
