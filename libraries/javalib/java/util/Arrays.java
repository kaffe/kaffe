
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.util;

import java.io.Serializable;

public class Arrays {

  // This class is not instantiable
  private Arrays() {
  }

  // The "default" Comparator
  private static class DefCmp implements Serializable, Comparator {
	public int compare(Object o1, Object o2) {
		return ((Comparable)o1).compareTo(o2);
	}
  }
  static final Comparator DEFAULT_COMPARATOR = new DefCmp();

  public static List asList(Object[] a) {
	return new ArrayList(a);
  }

  public static int binarySearch(byte[] a, byte key) {
	int base = 0;
	for (int lim = a.length; lim != 0; lim >>= 1) {
		final int index = base + (lim >> 1);
		if (key == a[index]) {
			return(index);
		} else if (key > a[index]) {
			base = index + 1;
			lim--;
		}
	}
	return ~base;
  }

  public static int binarySearch(char[] a, char key) {
	int base = 0;
	for (int lim = a.length; lim != 0; lim >>= 1) {
		final int index = base + (lim >> 1);
		if (key == a[index]) {
			return(index);
		} else if (key > a[index]) {
			base = index + 1;
			lim--;
		}
	}
	return ~base;
  }

  public static int binarySearch(double[] a, double key) {
	final long keyBits = Double.doubleToLongBits(key);
	int base = 0;
	for (int lim = a.length; lim != 0; lim >>= 1) {
		final int index = base + (lim >> 1);
		final long elemBits = Double.doubleToLongBits(a[index]);
		if (keyBits == elemBits) {
			return(index);
		} else if (keyBits > elemBits) {
			base = index + 1;
			lim--;
		}
	}
	return ~base;
  }

  public static int binarySearch(float[] a, float key) {
	final int keyBits = Float.floatToIntBits(key);
	int base = 0;
	for (int lim = a.length; lim != 0; lim >>= 1) {
		final int index = base + (lim >> 1);
		final int elemBits = Float.floatToIntBits(a[index]);
		if (keyBits == elemBits) {
			return(index);
		} else if (keyBits > elemBits) {
			base = index + 1;
			lim--;
		}
	}
	return ~base;
  }

  public static int binarySearch(int[] a, int key) {
	int base = 0;
	for (int lim = a.length; lim != 0; lim >>= 1) {
		final int index = base + (lim >> 1);
		if (key == a[index]) {
			return(index);
		} else if (key > a[index]) {
			base = index + 1;
			lim--;
		}
	}
	return ~base;
  }

  public static int binarySearch(short[] a, short key) {
	int base = 0;
	for (int lim = a.length; lim != 0; lim >>= 1) {
		final int index = base + (lim >> 1);
		if (key == a[index]) {
			return(index);
		} else if (key > a[index]) {
			base = index + 1;
			lim--;
		}
	}
	return ~base;
  }

  public static int binarySearch(long[] a, long key) {
	int base = 0;
	for (int lim = a.length; lim != 0; lim >>= 1) {
		final int index = base + (lim >> 1);
		if (key == a[index]) {
			return(index);
		} else if (key > a[index]) {
			base = index + 1;
			lim--;
		}
	}
	return ~base;
  }

  public static int binarySearch(Object[] a, Object key) {
  	return binarySearch(a, key, DEFAULT_COMPARATOR);
  }

  public static int binarySearch(Object[] a, Object key, Comparator c) {
	int base = 0;
	for (int lim = a.length; lim != 0; lim >>= 1) {
		final int index = base + (lim >> 1);
		final int diff = c.compare(key, a[index]);
		if (diff == 0) {
			return(index);
		} else if (diff > 0) {
			base = index + 1;
			lim--;
		}
	}
	return ~base;
  }

  public static boolean equals(boolean[] a, boolean[] a2) {
	try {
		if (a.length != a2.length) {
			return false;
		}
		for (int i = a.length; i-- > 0; ) {
			if (a[i] != a2[i]) {
				return false;
			}
		}
		return true;
	} catch (NullPointerException _) {
		return (a == a2);	// ie, they are both null
	}
  }

  public static boolean equals(byte[] a, byte[] a2) {
	try {
		if (a.length != a2.length) {
			return false;
		}
		for (int i = a.length; i-- > 0; ) {
			if (a[i] != a2[i]) {
				return false;
			}
		}
		return true;
	} catch (NullPointerException _) {
		return (a == a2);	// ie, they are both null
	}
  }

  public static boolean equals(char[] a, char[] a2) {
	try {
		if (a.length != a2.length) {
			return false;
		}
		for (int i = a.length; i-- > 0; ) {
			if (a[i] != a2[i]) {
				return false;
			}
		}
		return true;
	} catch (NullPointerException _) {
		return (a == a2);	// ie, they are both null
	}
  }

  public static boolean equals(double[] a, double[] a2) {
	try {
		if (a.length != a2.length) {
			return false;
		}
		for (int i = a.length; i-- > 0; ) {
			if (a[i] != a2[i]) {
				return false;
			}
		}
		return true;
	} catch (NullPointerException _) {
		return (a == a2);	// ie, they are both null
	}
  }

  public static boolean equals(float[] a, float[] a2) {
	try {
		if (a.length != a2.length) {
			return false;
		}
		for (int i = a.length; i-- > 0; ) {
			if (a[i] != a2[i]) {
				return false;
			}
		}
		return true;
	} catch (NullPointerException _) {
		return (a == a2);	// ie, they are both null
	}
  }

  public static boolean equals(int[] a, int[] a2) {
	try {
		if (a.length != a2.length) {
			return false;
		}
		for (int i = a.length; i-- > 0; ) {
			if (a[i] != a2[i]) {
				return false;
			}
		}
		return true;
	} catch (NullPointerException _) {
		return (a == a2);	// ie, they are both null
	}
  }

  public static boolean equals(short[] a, short[] a2) {
	try {
		if (a.length != a2.length) {
			return false;
		}
		for (int i = a.length; i-- > 0; ) {
			if (a[i] != a2[i]) {
				return false;
			}
		}
		return true;
	} catch (NullPointerException _) {
		return (a == a2);	// ie, they are both null
	}
  }

  public static boolean equals(long[] a, long[] a2) {
	try {
		if (a.length != a2.length) {
			return false;
		}
		for (int i = a.length; i-- > 0; ) {
			if (a[i] != a2[i]) {
				return false;
			}
		}
		return true;
	} catch (NullPointerException _) {
		return (a == a2);	// ie, they are both null
	}
  }

  public static boolean equals(Object[] a, Object[] a2) {
		if (a == a2) {
			return true;
		}

		if (a == null || a2 == null) {      
			return false; // One is null and not the other.
		}

		if (a.length != a2.length) {
			return false;
		}

		for (int i = a.length; i-- > 0; ) {
			if (a[i] == null) {
				if (a2[i] != null) {
					return false;
				}
			} else if (!a[i].equals(a2[i])) {
				return false;
			}
		}

		return true;
  }

  public static void fill(boolean[] a, boolean val) {
	fill(a, 0, a.length, val);
  }

  public static void fill(boolean[] a, int fromIndex, int toIndex, boolean val) {
	for (int i = toIndex; i-- > fromIndex; ) {
		a[i] = val;
	}
  }

  public static void fill(byte[] a, byte val) {
	fill(a, 0, a.length, val);
  }

  public static void fill(byte[] a, int fromIndex, int toIndex, byte val) {
	for (int i = toIndex; i-- > fromIndex; ) {
		a[i] = val;
	}
  }

  public static void fill(char[] a, char val) {
	fill(a, 0, a.length, val);
  }

  public static void fill(char[] a, int fromIndex, int toIndex, char val) {
	for (int i = toIndex; i-- > fromIndex; ) {
		a[i] = val;
	}
  }

  public static void fill(double[] a, double val) {
	fill(a, 0, a.length, val);
  }

  public static void fill(double[] a, int fromIndex, int toIndex, double val) {
	for (int i = toIndex; i-- > fromIndex; ) {
		a[i] = val;
	}
  }

  public static void fill(float[] a, float val) {
	fill(a, 0, a.length, val);
  }

  public static void fill(float[] a, int fromIndex, int toIndex, float val) {
	for (int i = toIndex; i-- > fromIndex; ) {
		a[i] = val;
	}
  }

  public static void fill(int[] a, int val) {
	fill(a, 0, a.length, val);
  }

  public static void fill(int[] a, int fromIndex, int toIndex, int val) {
	for (int i = toIndex; i-- > fromIndex; ) {
		a[i] = val;
	}
  }

  public static void fill(short[] a, short val) {
	fill(a, 0, a.length, val);
  }

  public static void fill(short[] a, int fromIndex, int toIndex, short val) {
	for (int i = toIndex; i-- > fromIndex; ) {
		a[i] = val;
	}
  }

  public static void fill(long[] a, long val) {
	fill(a, 0, a.length, val);
  }

  public static void fill(long[] a, int fromIndex, int toIndex, long val) {
	for (int i = toIndex; i-- > fromIndex; ) {
		a[i] = val;
	}
  }

  public static void fill(Object[] a, Object val) {
	fill(a, 0, a.length, val);
  }

  public static void fill(Object[] a, int fromIndex, int toIndex, Object val) {
	for (int i = toIndex; i-- > fromIndex; ) {
		a[i] = val;
	}
  }

  public static void sort(byte[] a) {
	sort(a, 0, a.length);
  }

  public static void sort(byte[] a, int fromIndex, int toIndex) {
	if (fromIndex < 0 || toIndex > a.length || fromIndex > toIndex) {
		throw new ArrayIndexOutOfBoundsException();
	}
	sortByte(a, fromIndex, toIndex);
  }

  public static void sort(char[] a) {
	sort(a, 0, a.length);
  }

  public static void sort(char[] a, int fromIndex, int toIndex) {
	if (fromIndex < 0 || toIndex > a.length || fromIndex > toIndex) {
		throw new ArrayIndexOutOfBoundsException();
	}
	sortChar(a, fromIndex, toIndex);
  }

  public static void sort(double[] a) {
	sort(a, 0, a.length);
  }

  public static void sort(double[] a, int fromIndex, int toIndex) {
	if (fromIndex < 0 || toIndex > a.length || fromIndex > toIndex) {
		throw new ArrayIndexOutOfBoundsException();
	}
	sortDouble(a, fromIndex, toIndex);
  }

  public static void sort(float[] a) {
	sort(a, 0, a.length);
  }

  public static void sort(float[] a, int fromIndex, int toIndex) {
	if (fromIndex < 0 || toIndex > a.length || fromIndex > toIndex) {
		throw new ArrayIndexOutOfBoundsException();
	}
	sortFloat(a, fromIndex, toIndex);
  }

  public static void sort(int[] a) {
	sort(a, 0, a.length);
  }

  public static void sort(int[] a, int fromIndex, int toIndex) {
	if (fromIndex < 0 || toIndex > a.length || fromIndex > toIndex) {
		throw new ArrayIndexOutOfBoundsException();
	}
	sortInt(a, fromIndex, toIndex);
  }

  public static void sort(short[] a) {
	sort(a, 0, a.length);
  }

  public static void sort(short[] a, int fromIndex, int toIndex) {
	if (fromIndex < 0 || toIndex > a.length || fromIndex > toIndex) {
		throw new ArrayIndexOutOfBoundsException();
	}
	sortShort(a, fromIndex, toIndex);
  }

  public static void sort(long[] a) {
	sort(a, 0, a.length);
  }

  public static void sort(long[] a, int fromIndex, int toIndex) {
	if (fromIndex < 0 || toIndex > a.length || fromIndex > toIndex) {
		throw new ArrayIndexOutOfBoundsException();
	}
	sortLong(a, fromIndex, toIndex);
  }

  public static void sort(Object[] a) {
	sort(a, 0, a.length, DEFAULT_COMPARATOR);
  }

  public static void sort(Object[] a, Comparator c) {
	sort(a, 0, a.length, c);
  }

  public static void sort(Object[] a, int fromIndex, int toIndex) {
	sort(a, fromIndex, toIndex, DEFAULT_COMPARATOR);
  }

  public static void sort(Object[] a, int fromIndex, int toIndex, Comparator c) {
	if (fromIndex < 0 || toIndex > a.length || fromIndex > toIndex) {
		throw new ArrayIndexOutOfBoundsException();
	}
	sortObject(a, fromIndex, toIndex, c);
  }

  private static native void sortByte(byte[] a, int fromIndex, int toIndex);
  private static native void sortChar(char[] a, int fromIndex, int toIndex);
  private static native void sortDouble(double[] a, int fromIndex, int toIndex);
  private static native void sortFloat(float[] a, int fromIndex, int toIndex);
  private static native void sortInt(int[] a, int fromIndex, int toIndex);
  private static native void sortShort(short[] a, int fromIndex, int toIndex);
  private static native void sortLong(long[] a, int fromIndex, int toIndex);
  private static native void sortObject(Object[] a, int fromIndex, int toIndex, Comparator c);

}


