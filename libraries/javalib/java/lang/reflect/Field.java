/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang.reflect;

public class Field implements Member {
	private Class clazz;
	private int slot;
	private String name;
	private Class type;

public boolean equals(Object obj)
	{
	// Quick test for identity
	if ((Object)this == obj) {
		return (true);
	}

	Field fobj;
	try {
		fobj = (Field)obj;
	}
	catch (ClassCastException _) {
		return (false);
	}
	if (clazz == fobj.clazz && type == fobj.type && name.equals(fobj.name)) {
		return (true);
	}
	return (false);
}

native public Object get(Object obj) throws IllegalArgumentException, IllegalAccessException;

native public boolean getBoolean(Object obj) throws IllegalArgumentException, IllegalAccessException;

native public byte getByte(Object obj) throws IllegalArgumentException, IllegalAccessException;

native public char getChar(Object obj) throws IllegalArgumentException, IllegalAccessException;

public Class getDeclaringClass()
	{
	return (clazz);
}

native public double getDouble(Object obj) throws IllegalArgumentException, IllegalAccessException;

native public float getFloat(Object obj) throws IllegalArgumentException, IllegalAccessException;

native public int getInt(Object obj) throws IllegalArgumentException, IllegalAccessException;

native public long getLong(Object obj) throws IllegalArgumentException, IllegalAccessException;

native public int getModifiers();

public String getName()
	{
	return (name);
}

native public short getShort(Object obj) throws IllegalArgumentException, IllegalAccessException;

public Class getType()
	{
	return (clazz);
}

public int hashCode()
	{
	return (clazz.getName().hashCode() ^ name.hashCode());
}

native public void set(Object obj, Object value) throws IllegalArgumentException, IllegalAccessException;

native public void setBoolean(Object obj, boolean z) throws IllegalArgumentException, IllegalAccessException;

native public void setByte(Object obj, byte b) throws IllegalArgumentException, IllegalAccessException;

native public void setChar(Object obj, char c) throws IllegalArgumentException, IllegalAccessException;

native public void setDouble(Object obj, double d) throws IllegalArgumentException, IllegalAccessException;

native public void setFloat(Object obj, float f) throws IllegalArgumentException, IllegalAccessException;

native public void setInt(Object obj, int i) throws IllegalArgumentException, IllegalAccessException;

native public void setLong(Object obj, long l) throws IllegalArgumentException, IllegalAccessException;

native public void setShort(Object obj, short s) throws IllegalArgumentException, IllegalAccessException;

public String toString() {
	StringBuffer str = new StringBuffer();
	int mod = getModifiers();

	if ((mod & Modifier.PUBLIC) != 0) {
		str.append("public ");
	}
	else if ((mod & Modifier.PRIVATE) != 0) {
		str.append("private ");
	}
	else if ((mod & Modifier.PROTECTED) != 0) {
		str.append("protected ");
	}

	if ((mod & Modifier.STATIC) != 0) {
		str.append("static ");
	}
	if ((mod & Modifier.FINAL) != 0) {
		str.append("final ");
	}
	if ((mod & Modifier.TRANSIENT) != 0) {
		str.append("transient ");
	}
	if ((mod & Modifier.VOLATILE) != 0) {
		str.append("volatile ");
	}

	// Type
	str.append(type.getName());
	str.append(' ');

	// Class name
	str.append(clazz.getName());
	str.append(".");
	// Field name
	str.append(name);

	return (str.toString());
}
}
