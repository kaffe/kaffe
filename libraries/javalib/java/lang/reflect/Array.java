/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 2001, 2002
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Checked Spec: JDK 1.3
 */

package java.lang.reflect;

import java.util.HashMap;
import java.util.Map;

public final class Array
{
/* This map is used to cache array types, since repeated native calls to
 * resolve them are expensive. Key format is baseClass + "." + dimensions.
 */
private static final Map typeCache = new HashMap();
private static final String SEPARATOR = ".";

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


/* A quick method to get an array type from a component type.
 * This is the bottleneck of the multiArrayCreation. We speed it up
 * using an array type cache.
 * dimensions must be >= 1
 */
private static Class getArrayType(Class componentType, int dimensions) {
	Class cachedType =  getCachedType(componentType, dimensions);
	if (cachedType != null) {
		return cachedType;
	}

	/* get a simple array of the component type first. If dimensions
	 * is 1, that's all we'll need. If it isn't, then we only need to
	 * append enough [ to the class name to get the array type name.
	 */
	Class simpleArrayType = newInstance(componentType, 0).getClass();
	if (dimensions == 1) {
		putTypeInCache(componentType, dimensions, simpleArrayType);
		return simpleArrayType;
	}
	else {
		StringBuffer buf = new StringBuffer(dimensions - 1);
		for (int i = 0; i < dimensions - 1; ++ i) {
			buf.append('[');
		}
		buf.append(simpleArrayType.getName());
		try {
			Class type = Class.forName(buf.toString());
			putTypeInCache(componentType, dimensions, type);
			return type;
		}
		catch (ClassNotFoundException e) {
			throw new InternalError(e.getMessage());
		}
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

private static Class getCachedType(Class componentType, int dimensions) {
	return (Class) typeCache.get(mangle(componentType, dimensions));
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

private static String mangle(Class componentType, int dimensions) {
	return componentType.getName() + SEPARATOR + dimensions;
}

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
	if (offset == (dimensions.length - 1)) {
		return newInstance(componentType, dimensions[offset]);
	}

	Class arrayType = getArrayType(componentType, dimensions.length - 1 - offset);
	Object[] array = (Object[]) newInstance(arrayType, dimensions[offset]);

	for (int i = dimensions[offset] - 1; i >= 0; i--) {
//		System.out.println("arr " + array.getClass());
//		System.out.println("obj " + newInstanceInternal(componentType, dimensions, offset + 1).getClass());
		array[i] = newInstanceInternal(componentType, dimensions, offset + 1);
	}

	return array;
}

private static void putTypeInCache(Class componentType, int dimensions, Class type) {
	typeCache.put(mangle(componentType, dimensions), type);
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
	else if (value instanceof Boolean) {
		setBoolean(array, index, ((Boolean)value).booleanValue());
	}
	else if (value instanceof Byte) {
		setByte(array, index, ((Byte)value).byteValue());
	}
	else if (value instanceof Short) {
		setShort(array, index, ((Short)value).shortValue());
	}
	else if (value instanceof Character) {
		setChar(array, index, ((Character)value).charValue());
	}
	else if (value instanceof Integer) {
		setInt(array, index, ((Integer)value).intValue());
	}
	else if (value instanceof Long) {
		setLong(array, index, ((Long)value).longValue());
	}
	else if (value instanceof Float) {
		setFloat(array, index, ((Float)value).floatValue());
	}
	else if (value instanceof Double) {
		setDouble(array, index,  ((Double)value).doubleValue());
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
