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
final public class Boolean
{
	final public static Boolean TRUE = new Boolean(true);
	final public static Boolean FALSE = new Boolean(false);
	final public static Class TYPE = Class.getPrimitiveClass("boolean");
	private boolean value;

public Boolean(String s) {
	value = (s != null && s.toLowerCase().equals("true"));
}

public Boolean(boolean value) {
	this.value=value;
}

public boolean booleanValue()
	{
	return (value);
}

public boolean equals(Object obj) {
	if ((obj!=null) && (obj instanceof Boolean)) {
		return (this.booleanValue()==((Boolean )obj).booleanValue());
	}
	else {
		return false;
	}
}

public static boolean getBoolean(String name)
	{
	String value = System.getProperty(name);
	if (value == null) {
		return (false);
	}
	else {
		return (value.equals("true"));
	}
}

public int hashCode()
	{
	return (value ? 1231 : 1237);
}

public String toString()
	{
	return (value ? new String("true") : new String("false"));
}

public static Boolean valueOf(String s)
	{
	return new Boolean(s);
}
}
