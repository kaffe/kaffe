
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
import java.lang.reflect.Array;

public class LinkedList extends AbstractSequentialList
		implements Cloneable, Serializable {
	Elem head = null;
	Elem tail = null;
	int length = 0;

	static class Elem {
		public Object o;
		public Elem prev;
		public Elem next;
		Elem(Object o) {
			this.o = o;
			prev = null;
			next = null;
		}
	}

	public LinkedList() {
	}

	public LinkedList(Collection c) {
		for (Iterator i = c.iterator(); i.hasNext(); ) {
			addLast(i.next());
		}
	}

	public Object getFirst() {
		if (length == 0) {
			throw new NoSuchElementException();
		}
		return head.o;
	}

	public Object getLast() {
		if (length == 0) {
			throw new NoSuchElementException();
		}
		return tail.o;
	}

	public Object removeFirst() {
		if (length == 0) {
			throw new NoSuchElementException();
		}
		modCount++;
		Object rtn = head.o;
		head = head.next;
		if (head == null) {
			tail = null;
		} else {
			head.prev = null;
		}
		length--;
		return rtn;
	}

	public Object removeLast() {
		if (length == 0) {
			throw new NoSuchElementException();
		}
		modCount++;
		Object rtn = tail.o;
		tail = tail.prev;
		if (tail == null) {
			head = null;
		} else {
			tail.next = null;
		}
		length--;
		return rtn;
	}

	public void addFirst(Object o) {
		Elem e = new Elem(o);
		modCount++;
		if (length == 0) {
			head = tail = e;
		} else {
			e.next = head;
			head.prev = e;
			head = e;
		}
		length++;
	}

	public void addLast(Object o) {
		Elem e = new Elem(o);
		modCount++;
		if (length == 0) {
			head = tail = e;
		} else {
			e.prev = tail;
			tail.next = e;
			tail = e;
		}
		length++;
	}

	public boolean contains(Object o) {
		return findObject(o) != null;
	}

	public int size() {
		return length;
	}

	public boolean add(Object o) {
		this.addLast(o);
		return true;
	}

	public boolean remove(Object o) {
		Elem e = findObject(o);
		if (e != null) {
			remove(e);
			return true;
		}
		return false;
	}

	public boolean addAll(Collection c) {
		return addAll(0, c);
	}

	public boolean addAll(int index, Collection c) {
		if (index < 0 || index > length) {
			throw new IndexOutOfBoundsException();
		}

		// Create a new LinkedList from the Collection
		LinkedList clist = new LinkedList(c);
		if (clist.length == 0) {
			return false;
		}

		// Locate "before" and "after" elements
		Elem before, after;
		for (before = head; index-- > 0; before = before.next);
		after = (before == null) ? null : before.next;

		// Insert clist in between before and after
		modCount++;
		clist.head.prev = before;
		clist.tail.next = after;
		if (before == null) {
			head = clist.head;
		} else {
			before.next = clist.head;
		}
		if (after == null) {
			tail = clist.tail;
		} else {
			after.prev = clist.tail;
		}
		length += clist.length;
		return true;
	}

	public void clear() {
		modCount++;
		head = null;
		tail = null;
		length = 0;
	}

	public Object get(int index) {
		return findIndex(index).o;
	}

	public Object set(int index, Object element) {
		Elem e = findIndex(index);
		Object old = e.o;
		e.o = element;
		return old;
	}

	public void add(int index, Object element) {
		if (index == length) {
			this.addLast(element);
			return;
		}
		Elem after = findIndex(index);
		Elem e = new Elem(element);
		modCount++;
		e.prev = after.prev;
		e.next = after;
		after.prev = e;
		if (e.prev == null) {
			head = e;
		} else {
			e.prev.next = e;
		}
		length++;
	}

	public Object remove(int index) {
		Elem e = findIndex(index);
		Object old = e.o;
		remove(e);
		return old;
	}

	public int indexOf(Object o) {
		int index = 0;
		for (Elem e = head; e != null; e = e.next, index++) {
			if (o == null ? e.o == null : o.equals(e.o)) {
				return index;
			}
		}
		return -1;
	}

	public int lastIndexOf(Object o) {
		int index = length - 1;
		for (Elem e = tail; e != null; e = e.prev, index--) {
			if (o == null ? e.o == null : o.equals(e.o)) {
				return index;
			}
		}
		return -1;
	}

	public ListIterator listIterator(int index) {
		return new LinkedListIterator(this, index);
	}

	public Object clone() {
		LinkedList clone;
		try {
			clone = (LinkedList)super.clone();
		} catch (CloneNotSupportedException e) {
			throw new Error();
		}
		clone.clear();
		for (Iterator i = iterator(); i.hasNext(); ) {
			clone.addLast(i.next());
		}
		return clone;
	}

	public Object[] toArray() {
		Object[] a = new Object[length];
		int index = 0;
		for (Elem e = head; e != null; e = e.next) {
			a[index++] = e.o;
		}
		return a;
	}

	public Object[] toArray(Object[] a) {
		if (a.length < length) {
			a = (Object[])Array.newInstance(a.getClass().getComponentType(), length);
		}
		int index = 0;
		for (Elem e = head; e != null; e = e.next) {
			a[index++] = e.o;
		}
		if (a.length > length) {
			a[length] = null;
		}
		return a;
	}

	private Elem findObject(Object o) {
		for (Elem e = head; e != null; e = e.next) {
			if (o == null ? e.o == null : o.equals(e.o)) {
				return e;
			}
		}
		return null;
	}

	private Elem findIndex(int index) {
		if (index < 0 || index >= length) {
			throw new IndexOutOfBoundsException();
		}
		Elem e;
		for (e = head; index-- > 0; e = e.next);
		return e;
	}

	// Add a new element before (ie, in front of) the "after" element
	void add(Elem after, Object o) {
		if (after == null) {
			addLast(o);
			return;
		}
		Elem e = new Elem(o);
		modCount++;
		e.prev = after.prev;
		e.next = after;
		after.prev = e;
		if (e.prev == null) {
			head = e;
		} else {
			e.prev.next = e;
		}
		length++;
	}

	// Remove the element "e", returning the next element
	Elem remove(Elem e) {
		if (e.prev == null) {
			removeFirst();
			return head;
		} else if (e.next == null) {
			removeLast();
			return null;
		} else {
			modCount++;
			e.prev.next = e.next;
			e.next.prev = e.prev;
			length--;
			return e.next;
		}
	}
}

