package java.lang.reflect;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public final class Array
{
private Array() {
}
    
native public static Object get(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static boolean getBoolean(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static byte getByte(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static char getChar(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static double getDouble(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static float getFloat(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static int getInt(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static int getLength(Object array) throws IllegalArgumentException;

native public static long getLong(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static short getShort(Object array, int index) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native private static Object multiNewArray(Class componentType, int dimensions[]);

native private static Object newArray(Class componentType, int len);

public static Object newInstance(Class componentType, int len) throws NegativeArraySizeException
{
	if (componentType == null) {
		throw new NullPointerException();
	}
	if (len < 0) {
		throw new NegativeArraySizeException();
	}
	return (newArray(componentType, len));
}

public static Object newInstance(Class componentType, int dimensions[]) throws IllegalArgumentException, NegativeArraySizeException
{
	if (componentType == null) {
		throw new NullPointerException();
	}
	if (dimensions.length == 0 || dimensions.length > 255) {
		throw new IllegalArgumentException();
	}
	for (int i = 0; i < dimensions.length; i++) {
		if (dimensions[i] < 0) {
			throw new NegativeArraySizeException();
		}
	}
	return (multiNewArray(componentType, dimensions));
}

native public static void set(Object array, int index, Object value) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static void setBoolean(Object array, int index, boolean z) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static void setByte(Object array, int index, byte b) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static void setChar(Object array, int index, char c) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static void setDouble(Object array, int index, double d) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static void setFloat(Object array, int index, float f) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static void setInt(Object array, int index, int i) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static void setLong(Object array, int index, long l) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;

native public static void setShort(Object array, int index, short s) throws IllegalArgumentException, ArrayIndexOutOfBoundsException;
}
