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

public final class Array
{
private Array() {
}
    
private static void checkComponentType(Class componentType) {
	if (componentType == null) {
		throw new NullPointerException();
	}
	if (componentType == Void.TYPE) {
		throw new IllegalArgumentException();
	}
}

public static Object get(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof Object[]) {
		return (((Object[])array)[index]);
	}
	else if (array instanceof boolean[]) {
		return new Boolean(((boolean[])array)[index]);
	}
	else if (array instanceof byte[]) {
		return new Byte(((byte[])array)[index]);
	}
	else if (array instanceof short[]) {
		return new Short(((short[])array)[index]);
	}
	else if (array instanceof char[]) {
		return new Character(((char[])array)[index]);
	}
	else if (array instanceof int[]) {
		return new Integer(((int[])array)[index]);
	}
	else if (array instanceof long[]) {
		return new Long(((long[])array)[index]);
	}
	else if (array instanceof float[]) {
		return new Float(((float[])array)[index]);
	}
	else if (array instanceof double[]) {
		return new Double(((double[])array)[index]);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static boolean getBoolean(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof boolean[]) {
		return (((boolean[])array)[index]);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static byte getByte(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof byte[]) {
		return (((byte[])array)[index]);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static char getChar(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof char[]) {
		return (((char[])array)[index]);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static double getDouble(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof double[]) {
		return (((double[])array)[index]);
	}
	else if (array instanceof byte[]) {
		return (((byte[])array)[index]);
	}
	else if (array instanceof short[]) {
		return (((short[])array)[index]);
	}
	else if (array instanceof char[]) {
		return (((char[])array)[index]);
	}
	else if (array instanceof int[]) {
		return (((int[])array)[index]);
	}
	else if (array instanceof long[]) {
		return (((long[])array)[index]);
	}
	else if (array instanceof float[]) {
		return (((float[])array)[index]);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static float getFloat(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof float[]) {
		return (((float[])array)[index]);
	}
	else if (array instanceof byte[]) {
		return (((byte[])array)[index]);
	}
	else if (array instanceof short[]) {
		return (((short[])array)[index]);
	}
	else if (array instanceof char[]) {
		return (((char[])array)[index]);
	}
	else if (array instanceof int[]) {
		return (((int[])array)[index]);
	}
	else if (array instanceof long[]) {
		return (((long[])array)[index]);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static int getInt(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof int[]) {
		return (((int[])array)[index]);
	}
	else if (array instanceof byte[]) {
		return (((byte[])array)[index]);
	}
	else if (array instanceof short[]) {
		return (((short[])array)[index]);
	}
	else if (array instanceof char[]) {
		return (((char[])array)[index]);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static long getLong(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof long[]) {
		return (((long[])array)[index]);
	}
	else if (array instanceof byte[]) {
		return (((byte[])array)[index]);
	}
	else if (array instanceof short[]) {
		return (((short[])array)[index]);
	}
	else if (array instanceof char[]) {
		return (((char[])array)[index]);
	}
	else if (array instanceof int[]) {
		return (((int[])array)[index]);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static short getShort(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof short[]) {
		return (((short[])array)[index]);
	}
	else if (array instanceof byte[]) {
		return (((byte[])array)[index]);
	}
	else {
		throw new IllegalArgumentException();
	}
}

native public static int getLength(Object array) throws IllegalArgumentException;

native private static Object multiNewArray(Class componentType, int dimensions[]);

native private static Object newArray(Class componentType, int len);

public static Object newInstance(Class componentType, int len) throws NegativeArraySizeException
{
	checkComponentType(componentType);

	if (len < 0) {
		throw new NegativeArraySizeException();
	}

	if (!componentType.isPrimitive()) {
		return (newArray(componentType, len));
	}
	else if (componentType == Boolean.TYPE) {
		return (new boolean[len]);
	}
	else if (componentType == Byte.TYPE) {
		return (new byte[len]);
	}
	else if (componentType == Short.TYPE) {
		return (new short[len]);
	}
	else if (componentType == Character.TYPE) {
		return (new char[len]);
	}
	else if (componentType == Integer.TYPE) {
		return (new int[len]);
	}
	else if (componentType == Long.TYPE) {
		return (new long[len]);
	}
	else if (componentType == Float.TYPE) {
		return (new float[len]);
	}
	else if (componentType == Double.TYPE) {
		return (new double[len]);
	}
	else {
		throw new Error();
	}
}

public static Object newInstance(Class componentType, int dimensions[]) throws IllegalArgumentException, NegativeArraySizeException
{
	checkComponentType(componentType);

	if (dimensions.length == 0 || dimensions.length > 255) {
		throw new IllegalArgumentException();
	}
	for (int i = 0; i < dimensions.length; i++) {
		if (dimensions[i] < 0) {
			throw new NegativeArraySizeException();
		}
	}

	return (newInstanceInternal(componentType, dimensions, 0));
}

private static Object newInstanceInternal(Class componentType, int[] dimensions, int offset) {
	Object obj = newInstance(componentType, dimensions[offset]);
	offset++;
	if (offset < dimensions.length) {
		Object[] array = (Object[])obj;
		componentType = componentType.getComponentType();
		for (int i = dimensions[offset]; i >= 0; i--) {
			array[i] = newInstanceInternal(componentType, dimensions, offset);
		}
	}
	return (obj);
}

public static void set(Object array, int index, Object value) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof Object[]) {
		try {
			((Object[])array)[index] = value;
		}
		catch (ArrayStoreException _) {
			throw new IllegalArgumentException();
		}
	}
	else if (array instanceof boolean[] && value instanceof Boolean) {
		((boolean[])array)[index] = ((Boolean)value).booleanValue();
	}
	else if (array instanceof byte[] && value instanceof Byte) {
		((byte[])array)[index] = ((Byte)value).byteValue();
	}
	else if (array instanceof short[] && value instanceof Short) {
		((short[])array)[index] = ((Short)value).shortValue();
	}
	else if (array instanceof char[] && value instanceof Character) {
		((char[])array)[index] = ((Character)value).charValue();
	}
	else if (array instanceof int[] && value instanceof Integer) {
		((int[])array)[index] = ((Integer)value).intValue();
	}
	else if (array instanceof long[] && value instanceof Long) {
		((long[])array)[index] = ((Long)value).longValue();
	}
	else if (array instanceof float[] && value instanceof Float) {
		((float[])array)[index] = ((Float)value).floatValue();
	}
	else if (array instanceof double[] && value instanceof Double) {
		((double[])array)[index] = ((Double)value).doubleValue();
	}
	else {
		throw new IllegalArgumentException();
	}
}
public static void setBoolean(Object array, int index, boolean z) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof boolean[]) {
		((boolean[])array)[index] = z;
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static void setByte(Object array, int index, byte b) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof byte[]) {
		((byte[])array)[index] = b;
	}
	else if (array instanceof short[]) {
		((short[])array) [index] = b;
	}
       	else if (array instanceof int[]) {
		((int[])array) [index] = b;
	}
	else if (array instanceof long[]) {
		((long[])array) [index] = b;
	}
	else if (array instanceof float[]) {
		((float[])array) [index] = b;
	}
	else if (array instanceof double[]) {
		((double[])array) [index] = b;
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static void setChar(Object array, int index, char c) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof char[]) {
		((char[])array)[index] = c;
	}
	else if (array instanceof int[]) {
		((int[])array) [index] = c;
	}
	else if (array instanceof long[]) {
		((long[])array) [index] = c;
	}
	else if (array instanceof float[]) {
		((float[])array) [index] = c;
	}
	else if (array instanceof double[]) {
		((double[])array) [index] = c;
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static void setDouble(Object array, int index, double d) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof double[]) {
		((double[])array)[index] = d;
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static void setFloat(Object array, int index, float f) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof float[]) {
		((float[])array)[index] = f;
	}
	else if (array instanceof double[]) {
		((double[])array) [index] = f;
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static void setInt(Object array, int index, int i) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof int[]) {
		((int[])array)[index] = i;
	}
	else if (array instanceof long[]) {
		((long[])array) [index] = i;
	}
	else if (array instanceof float[]) {
		((float[])array) [index] = i;
	}
	else if (array instanceof double[]) {
		((double[])array) [index] = i;
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static void setLong(Object array, int index, long l) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof long[]) {
		((long[])array)[index] = l;
	}
	else if (array instanceof float[]) {
		((float[])array) [index] = l;
	}
	else if (array instanceof double[]) {
		((double[])array) [index] = l;
	}
	else {
		throw new IllegalArgumentException();
	}
}

public static void setShort(Object array, int index, short s) throws IllegalArgumentException, ArrayIndexOutOfBoundsException {
	if (array instanceof short[]) {
		((short[])array)[index] = s;
	}
	else if (array instanceof int[]) {
		((int[])array) [index] = s;
	}
	else if (array instanceof long[]) {
		((long[])array) [index] = s;
	}
	else if (array instanceof float[]) {
		((float[])array) [index] = s;
	}
	else if (array instanceof double[]) {
		((double[])array) [index] = s;
	}
	else {
		throw new IllegalArgumentException();
	}
}
}
