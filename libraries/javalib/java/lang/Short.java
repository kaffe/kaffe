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
	final private short value;

public Short(String s) throws NumberFormatException
{
	value = parseShort(s);
}

public Short(short value)
{
	this.value = value;
}

public static Short decode(String nm) throws NumberFormatException
{
	int val = Integer.decode(nm).intValue();
	if (val < MIN_VALUE || val > MAX_VALUE)
	    throw new NumberFormatException();
	return new Short((short) val);
}

public double doubleValue()
	{
	return ((double)value);
}

public boolean equals(Object obj) {
	return (obj != null)
	    && (obj instanceof Short)
	    && (((Short) obj).value == this.value);
}

public float floatValue()
	{
	return ((float)value);
}

public int hashCode()
	{
	return (value);	// What should this be do you suppose ???
}

public int intValue()
	{
	return ((int)value);
}

public long longValue()
	{
	return ((long)value);
}

public static short parseShort(String s) throws NumberFormatException
{
	return ((short)parseShort(s, 10));
}

public static short parseShort(String s, int radix) throws NumberFormatException
{
	int val = Integer.parseInt(s, radix);
	if (val < MIN_VALUE || val > MAX_VALUE)
		throw new NumberFormatException();
	return (short)val;
}

public String toString()
	{
	return (toString(value));
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
