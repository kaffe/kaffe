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

// This class is used internally by this package
class AbstractListIterator implements ListIterator {
	protected final AbstractList list;
	protected int lastIndex = -1;
	protected int modCount;
	protected int index;

	AbstractListIterator(AbstractList list, int index) {
		if (index < 0 || index > list.size()) {
			throw new IllegalArgumentException();
		}
		this.list = list;
		this.index = index;
		this.modCount = list.modCount;
	}

	public int nextIndex() {
		return index;
	}

	public int previousIndex() {
		return index - 1;
	}

	public boolean hasNext() {
		return index < list.size();
	}

	public Object next() {
		if (list.modCount != modCount) {
			throw new ConcurrentModificationException();
		}
		if (index >= list.size()) {
			throw new NoSuchElementException();
		}
		Object rtn = list.get(index);
		lastIndex = index++;
		return rtn;
	}

	public boolean hasPrevious() {
		return index > 0;
	}

	public Object previous() {
		if (list.modCount != modCount) {
			throw new ConcurrentModificationException();
		}
		if (index <= 0) {
			throw new NoSuchElementException();
		}
		Object rtn = list.get(index - 1);
		lastIndex = --index;
		return rtn;
	}

	public void remove() {
		if (list.modCount != modCount) {
			throw new ConcurrentModificationException();
		}
		if (lastIndex == -1) {
			throw new IllegalStateException();
		}
		list.remove(lastIndex);
		modCount++;
		if (lastIndex < index) {
			index--;
		}
		lastIndex = -1;
	}

	public void set(Object o) {
		if (list.modCount != modCount) {
			throw new ConcurrentModificationException();
		}
		if (lastIndex == -1) {
			throw new IllegalStateException();
		}
		list.set(lastIndex, o);
	}

	public void add(Object o) {
		if (list.modCount != modCount) {
			throw new ConcurrentModificationException();
		}
		list.add(index, o);
		modCount++;
		index++;
	}
}

