/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 2001
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Checked Spec: JDK 1.3
 */

package java.lang.reflect;


public final class Field
  extends AccessibleObject
  implements Member
{
	private Class clazz;
	private int slot;
	private String name;
	private Class type;

private Field() {
}
    
public boolean equals(Object obj)
	{
	// Quick test for identity
	if (this == obj) {
		return (true);
	}

	if (!(obj instanceof Field)) {
		return (false);
	}

	Field fobj = (Field)obj;
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
	return (type);
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

public String toString()
	{
	StringBuffer str = new StringBuffer();
	int mod = getModifiers();

	// Modifier
	if (mod != 0) {
		str.append(Modifier.toString(mod));
		str.append(' ');
	}

	// Type
	str.append(Method.getPrettyName(type));
	str.append(' ');

	// Class name
	str.append(clazz.getName());
	str.append('.');
	// Field name
	str.append(name);

	return (str.toString());
}
}
