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
final public class Byte
{
	final public static byte MIN_VALUE = -0x80;
	final public static byte MAX_VALUE = 0x7F;
	final public static Class TYPE = Class.getPrimitiveClass("byte");
	private byte val;

public Byte(String s) throws NumberFormatException
{
	this(parseByte(s));
}

public Byte(byte value)
	{
	val = value;
}

public static Byte decode(String nm) throws NumberFormatException
{
	byte val;

	if (nm.value[nm.offset] == '#') {
		val = parseByte(nm.substring(1), 16);
	}
	else if (nm.value[nm.offset] == '0') {
		if ( (nm.count > 1) && (nm.value[nm.offset+1] == 'x') )
			val = parseByte( nm.substring(2), 16);
		else
			val = parseByte(nm.substring(1), 8);
	}
	else {
		val = parseByte(nm.substring(1), 10);
	}

	return new Byte(val);
}

public double doubleValue()
	{
	return ((double)val);
}

public boolean equals(Object obj)
	{
	try {
		if (((Byte)obj).val == val) {
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

public static byte parseByte(String s) throws NumberFormatException
{
	return (parseByte(s, 10));
}

public static byte parseByte(String s, int radix) throws NumberFormatException
{
	return ((byte)Integer.parseInt(s, radix));
}

public String toString()
	{
	return (toString(val));
}

public static String toString(byte s)
	{
	return Integer.toString((int)s);
}

public static Byte valueOf(String s) throws NumberFormatException
{
	return (new Byte(parseByte(s)));
}

public static Byte valueOf(String s, int radix) throws NumberFormatException
{
	return (new Byte(parseByte(s, radix)));
}
}
