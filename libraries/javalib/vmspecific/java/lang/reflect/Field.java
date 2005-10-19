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

private void checkFinal() throws IllegalAccessException {    
	if (Modifier.isFinal(getModifiers())) {
		throw new IllegalAccessException("trying to set final field " + toString());
	}
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

public Object get(Object obj) throws IllegalArgumentException, IllegalAccessException {
	if (type == Double.TYPE) {
		return (new Double(getDouble0(obj)));
	}
	else if (type == Float.TYPE) {
		return (new Float(getFloat0(obj)));
	}
	else if (type == Long.TYPE) {
		return (new Long(getLong0(obj)));
	}
	else if (type == Integer.TYPE) {
		return (new Integer(getInt0(obj)));
	}
	else if (type == Short.TYPE) {
		return (new Short(getShort0(obj)));
	}
	else if (type == Byte.TYPE) {
		return (new Byte(getByte0(obj)));
	}
	else if (type == Character.TYPE) {
		return (new Character(getChar0(obj)));
	}
	else if (type == Boolean.TYPE) {
	    return (new Boolean(getBoolean0(obj)));
	}
	else {
		return (getObject0(obj));
	}
}

public boolean getBoolean(Object obj) throws IllegalArgumentException, IllegalAccessException {
	if (type == Boolean.TYPE) {
		return (getBoolean0(obj));
	}
	else {
		throw new IllegalArgumentException();
	}
}

public byte getByte(Object obj) throws IllegalArgumentException, IllegalAccessException {
	if (type == Byte.TYPE) {
		return (getByte0(obj));
	}
	else {
		throw new IllegalArgumentException();
	}
}

public char getChar(Object obj) throws IllegalArgumentException, IllegalAccessException {
	if (type == Character.TYPE) {
		return (getChar0(obj));
	}
	else {
		throw new IllegalArgumentException();
	}
}

public double getDouble(Object obj) throws IllegalArgumentException, IllegalAccessException {
	if (type == Double.TYPE) {
		return (getDouble0(obj));
	}
	else if (type == Float.TYPE) {
		return ((double)getFloat0(obj));
	}
	else if (type == Long.TYPE) {
		return ((double)getLong0(obj));
	}
	else if (type == Integer.TYPE) {
		return ((double)getInt0(obj));
	}
	else if (type == Short.TYPE) {
		return ((double)getShort0(obj));
	}
	else if (type == Byte.TYPE) {
		return ((double)getByte0(obj));
	}
	else if (type == Character.TYPE) {
		return ((double)getChar0(obj));
	}
	else {
		throw new IllegalArgumentException();
	}
}

public float getFloat(Object obj) throws IllegalArgumentException, IllegalAccessException {
	if (type == Float.TYPE) {
		return (getFloat0(obj));
	}
	else if (type == Long.TYPE) {
		return ((float)getLong0(obj));
	}
	else if (type == Integer.TYPE) {
		return ((float)getInt0(obj));
	}
	else if (type == Short.TYPE) {
		return ((float)getShort0(obj));
	}
	else if (type == Byte.TYPE) {
		return ((float)getByte0(obj));
	}
	else if (type == Character.TYPE) {
		return ((float)getChar0(obj));
	}
	else {
		throw new IllegalArgumentException();
	}
}

public int getInt(Object obj) throws IllegalArgumentException, IllegalAccessException {
	if (type == Integer.TYPE) {
		return (getInt0(obj));
	}
	else if (type == Short.TYPE) {
		return ((int)getShort0(obj));
	}
	else if (type == Byte.TYPE) {
		return ((int)getByte0(obj));
	}
	else if (type == Character.TYPE) {
		return ((int)getChar0(obj));
	}
	else {
		throw new IllegalArgumentException();
	}
}

public long getLong(Object obj) throws IllegalArgumentException, IllegalAccessException {
	if (type == Long.TYPE) {
		return (getLong0(obj));
	}
	else if (type == Integer.TYPE) {
		return ((long)getInt0(obj));
	}
	else if (type == Short.TYPE) {
		return ((long)getShort0(obj));
	}
	else if (type == Byte.TYPE) {
		return ((long)getByte0(obj));
	}
	else if (type == Character.TYPE) {
		return ((long)getChar0(obj));
	}
	else {
		throw new IllegalArgumentException();
	}
}

public short getShort(Object obj) throws IllegalArgumentException, IllegalAccessException {
	if (type == Short.TYPE) {
		return (getShort0(obj));
	}
	else if (type == Integer.TYPE) {
		return ((short)getInt0(obj));
	}
	else if (type == Long.TYPE) {
		return ((short)getLong0(obj));
	}
	else if (type == Float.TYPE) {
		return ((short)getFloat0(obj));
	}
	else if (type == Double.TYPE) {
		return ((short)getDouble0(obj));
	}
	else {
		throw new IllegalArgumentException();
	}
}

private void setInternal(Object obj, Object value) throws IllegalArgumentException, IllegalAccessException {
	if (type.isPrimitive()) {
		if (value instanceof Boolean) {
			setBooleanInternal(obj, ((Boolean)value).booleanValue());
		}
		else if (value instanceof Byte) {
			setByteInternal(obj, ((Byte)value).byteValue());
		}
		else if (value instanceof Short) {
			setShortInternal(obj, ((Short)value).shortValue());
		}
		else if (value instanceof Character) {
			setCharInternal(obj, ((Character)value).charValue());
		}
		else if (value instanceof Integer) {
			setIntInternal(obj, ((Integer)value).intValue());
		}
		else if (value instanceof Long) {
			setLongInternal(obj, ((Long)value).longValue());
		}
		else if (value instanceof Float) {
			setFloatInternal(obj, ((Float)value).floatValue());
		}
		else {
			setDoubleInternal(obj, ((Double)value).doubleValue());
		}
	}
	else {
		if (value!=null && !type.isInstance(value)) {
			throw new IllegalArgumentException("field type mismatch: Trying to assign a " + value.getClass().getName() + " to " + toString());
		}

		setObject0(obj, value);
	}
}

public void set(Object obj, Object value) throws IllegalArgumentException, IllegalAccessException {
	checkFinal();
	setInternal(obj, value);
}

public void setBooleanInternal(Object obj, boolean z) throws IllegalArgumentException, IllegalAccessException {
	if (type == Boolean.TYPE) {
		setBoolean0(obj, z);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public void setBoolean(Object obj, boolean z) throws IllegalArgumentException, IllegalAccessException {
        checkFinal();
	setBooleanInternal(obj, z);
}

public void setByteInternal(Object obj, byte b) throws IllegalArgumentException, IllegalAccessException {
	if (type == Byte.TYPE) {
		setByte0(obj, b);
	}
	else if (type == Short.TYPE) {
		setShort0(obj, (short)b);
	}
	else if (type == Integer.TYPE) {
		setInt0(obj, (int)b);
	}
	else if (type == Long.TYPE) {
		setLong0(obj, (long)b);
	}
	else if (type == Float.TYPE) {
		setFloat0(obj, (float)b);
	}
	else if (type == Double.TYPE) {
		setDouble0(obj, (double)b);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public void setByte(Object obj, byte b) throws IllegalArgumentException, IllegalAccessException {
        checkFinal();
	setByteInternal(obj, b);
}

public void setCharInternal(Object obj, char c) throws IllegalArgumentException, IllegalAccessException {
	if (type == Character.TYPE) {
		setChar0(obj, c);
	}
	else if (type == Integer.TYPE) {
		setInt0(obj, (int)c);
	}
	else if (type == Long.TYPE) {
		setLong0(obj, (long)c);
	}
	else if (type == Float.TYPE) {
		setFloat0(obj, (float)c);
	}
	else if (type == Double.TYPE) {
		setDouble0(obj, (double)c);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public void setChar(Object obj, char c) throws IllegalArgumentException, IllegalAccessException {
        checkFinal();
	setCharInternal(obj, c);
}

public void setDoubleInternal(Object obj, double d) throws IllegalArgumentException, IllegalAccessException {
	if (type == Double.TYPE) {
		setDouble0(obj, d);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public void setDouble(Object obj, double d) throws IllegalArgumentException, IllegalAccessException {
        checkFinal();
	setDoubleInternal(obj, d);
}

public void setFloatInternal(Object obj, float f) throws IllegalArgumentException, IllegalAccessException {
	if (type == Float.TYPE) {
		setFloat0(obj, f);
	}
	else if (type == Double.TYPE) {
		setDouble0(obj, (double)f);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public void setFloat(Object obj, float f) throws IllegalArgumentException, IllegalAccessException {
        checkFinal();
	setFloatInternal(obj, f);
}

public void setIntInternal(Object obj, int i) throws IllegalArgumentException, IllegalAccessException {
        if (type == Integer.TYPE) {
		setInt0(obj, i);
	}
	else if (type == Long.TYPE) {
		setLong0(obj, (long)i);
	}
	else if (type == Float.TYPE) {
		setFloat0(obj, (float)i);
	}
	else if (type == Double.TYPE) {
		setDouble0(obj, (double)i);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public void setInt(Object obj, int i) throws IllegalArgumentException, IllegalAccessException {
        checkFinal();
	setIntInternal(obj, i);
}

public void setLongInternal(Object obj, long l) throws IllegalArgumentException, IllegalAccessException {
        if (type == Long.TYPE) {
		setLong0(obj, l);
	}
	else if (type == Float.TYPE) {
		setFloat0(obj, (float)l);
	}
	else if (type == Double.TYPE) {
		setDouble0(obj, (double)l);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public void setLong(Object obj, long l) throws IllegalArgumentException, IllegalAccessException {
        checkFinal();
	setLongInternal(obj, l);
}

public void setShortInternal(Object obj, short s) throws IllegalArgumentException, IllegalAccessException {
        checkFinal();
	if (type == Short.TYPE) {
		setShort0(obj, s);
	}
	else if (type == Integer.TYPE) {
		setInt0(obj, (int)s);
	}
	else if (type == Long.TYPE) {
		setLong0(obj, (long)s);
	}
	else if (type == Float.TYPE) {
		setFloat0(obj, (float)s);
	}
	else if (type == Double.TYPE) {
		setDouble0(obj, (double)s);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public void setShort(Object obj, short s) throws IllegalArgumentException, IllegalAccessException {
        checkFinal();
	setShortInternal(obj, s);
}

public Class getDeclaringClass()
{
	return (clazz);
}

native public int getModifiers();

public String getName()
{
	return (name);
}

public Class getType()
{
	return (type);
}

public int hashCode()
{
	return (clazz.getName().hashCode() ^ name.hashCode());
}

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

native private boolean getBoolean0(Object obj);
native private byte getByte0(Object obj);
native private char getChar0(Object obj);
native private short getShort0(Object obj);
native private int getInt0(Object obj);
native private long getLong0(Object obj);
native private float getFloat0(Object obj);
native private double getDouble0(Object obj);
native private Object getObject0(Object obj);

native private void setBoolean0(Object obj, boolean v);
native private void setByte0(Object obj, byte v);
native private void setChar0(Object obj, char v);
native private void setShort0(Object obj, short v);
native private void setInt0(Object obj, int v);
native private void setLong0(Object obj, long v);
native private void setFloat0(Object obj, float v);
native private void setDouble0(Object obj, double v);
native private void setObject0(Object obj, Object v);
}
