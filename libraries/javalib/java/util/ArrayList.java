
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.util;

import java.io.Serializable;
import java.lang.reflect.Array;

public class ArrayList extends AbstractList
		implements Cloneable, Serializable {

	private static final int DEFAULT_CAPACITY = 32;
	private boolean fixed;		// means that size must remain fixed
	private Object[] a;
	private int off;
	private int len;

	public ArrayList() {
		this(DEFAULT_CAPACITY);
	}

	public ArrayList(Collection c) {
		final Iterator i = c.iterator();
		a = new Object[(c.size() * 11) / 10];
		int count;
		for (count = 0; i.hasNext(); count++) {
		      a[count] = i.next();
		}
		len = count;
	}

	public ArrayList(int initialCapacity) {
		if (initialCapacity < 0) {
		    throw new IllegalArgumentException("initialCapacity < 0");
		}
		a = new Object[initialCapacity];
	}

	// Used by Arrays.asList()
	ArrayList(Object[] a) {
		this.a = a;
		len = a.length;
		fixed = true;
	}

	public void trimToSize() {
		if (off != 0 || len != a.length) {
			Object[] newa = new Object[len];
			System.arraycopy(a, off, newa, 0, len);
			modCount++;
			off = 0;
			a = newa;
		}
	}

	public void ensureCapacity(int minCapacity) {

		// Check current capacity
		if (fixed || a.length - off >= minCapacity) {
			return;
		}

		// Round up desired capacity to nearest power of 2 (- 12) to
		// avoid n^2 behavior when adding one element at a time. The
		// -12 part should be optimized from empirical tests.
		int bit, newSize;
		for (bit = 0x10; bit != 0x80000000; bit <<= 1) {
			if (bit - 12 >= minCapacity) {
				break;
			}
		}
		if (bit == 0x80000000) {
			throw new IllegalArgumentException("too big");
		}
		newSize = bit - 12;

		// Allocate new array
		Object[] newa = new Object[newSize];
		System.arraycopy(a, off, newa, 0, len);
		modCount++;
		a = newa;
		off = 0;
	}

	public int size() {
		return len;
	}

	public boolean isEmpty() {
		return len == 0;
	}

	public boolean contains(Object elem) {
		return indexOf(elem) != -1;
	}

	public int indexOf(Object elem) {
		for (int i = off; i < off + len; i++) {
			if (elem == null ? a[i] == null : elem.equals(a[i])) {
				return i - off;
			}
		}
		return -1;
	}

	public int lastIndexOf(Object elem) {
		for (int i = off + len - 1; i >= off; i--) {
			if (elem == null ? a[i] == null : elem.equals(a[i])) {
				return i - off;
			}
		}
		return -1;
	}

	public Object clone() {
		ArrayList clone;
		try {
			clone = (ArrayList)super.clone();
		} catch (CloneNotSupportedException e) {
			throw new Error();
		}
		clone.a = new Object[len];
		System.arraycopy(a, off, clone.a, 0, len);
		clone.off = 0;
		clone.fixed = false;
		return clone;
	}

	public Object[] toArray() {
		Object[] newa = new Object[len];
		System.arraycopy(a, off, newa, 0, len);
		return newa;
	}

	public Object[] toArray(Object[] ary) {
		if (ary.length < len) {
			ary = (Object[])Array.newInstance(
				ary.getClass().getComponentType(), len);
		}
		System.arraycopy(a, off, ary, 0, len);
		if (ary.length > len) {
			ary[len] = null;
		}
		return ary;
	}

	public Object get(int index) {
		if (index < 0 || index >= len) {
			throw new IndexOutOfBoundsException();
		}
		return a[off + index];
	}

	public Object set(int index, Object element) {
		if (index < 0 || index >= len) {
			throw new IndexOutOfBoundsException();
		}
		Object old = a[off + index];
		a[off + index] = element;
		return old;
	}

	public boolean add(Object element) {
		if (fixed) {
			throw new UnsupportedOperationException();
		}
		add(len, element);
		return true;
	}

	public void add(int index, Object element) {
		if (fixed) {
			throw new UnsupportedOperationException();
		}
		if (index < 0 || index > len) {
			throw new IndexOutOfBoundsException();
		}
		final int initialModCount = modCount;
		if (off > 0 && index < len / 2) {
			System.arraycopy(a, off, a, off - 1, index);
			off--;
		} else {
			ensureCapacity(len + 1);
			System.arraycopy(a, off + index,
				a, off + index + 1, len - index);
		}
		modCount = initialModCount + 1;
		a[off + index] = element;
		len++;
	}

	public Object remove(int index) {
		if (fixed) {
			throw new UnsupportedOperationException();
		}
		Object old = a[off + index];	// exception here is OK
		removeRange(index, index + 1);
		return old;
	}

	public void clear() {
		if (fixed) {
			throw new UnsupportedOperationException();
		}
		modCount++;
		off = len = 0;
	}

	public boolean addAll(Collection c) {
		if (fixed) {
			throw new UnsupportedOperationException();
		}
		addAll(len, c);
		return true;
	}

	public boolean addAll(int index, Collection c) {
		if (fixed) {
			throw new UnsupportedOperationException();
		}
		if (index < 0 || index > len) {
			throw new IndexOutOfBoundsException();
		}
		final Iterator i = c.iterator();
		final int increase = c.size();
		final int initialModCount = modCount;
		if (off >= increase && index < len / 2) {
			modCount++;
			System.arraycopy(a, off, a, off - increase, index);
			off -= increase;
		} else {
			ensureCapacity(len + increase);
			modCount = initialModCount + 1;
			System.arraycopy(a, off + index,
				a, off + index + increase, len - index);
		}
		for (int index2 = off + index; i.hasNext(); index2++) {
			a[index2] = i.next();
		}
		len += increase;
		return true;
	}

	protected void removeRange(int fromIndex, int toIndex) {
		if (fixed) {
			throw new UnsupportedOperationException();
		}
		if (fromIndex < 0 || toIndex > len) {
			throw new IndexOutOfBoundsException();
		}
		final int decrease = toIndex - fromIndex;
		if (decrease <= 0) {
			return;
		}
		modCount++;
		if (fromIndex == 0) {
			off += toIndex;
		} else {
			System.arraycopy(a, off + toIndex,
				a, off + fromIndex, len - toIndex);
		}
		len -= decrease;
	}
}

