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

// This class is used by LinkedList.listIterator()
// The methods below are overridden in order to be more efficient.
// We can be more efficient because we keep a pointer to the currently
// indexed element in the linked list.

class LinkedListIterator extends AbstractListIterator {
	LinkedList.Elem elem;		// The element corresponding to index,
					//   equal to null if index == length

	LinkedListIterator(LinkedList list, int index) {
		super(list, index);
		elem = list.head;
		// Position elem to the index'th element.
		// We know that index is a valid index, because that was 
		// checked in the constructor in AbstractListIterator.
		for (; index > 0; index--)
		  elem = elem.next;
	}

	public Object next() {
		if (list.modCount != modCount) {
			throw new ConcurrentModificationException();
		}
		if (index >= ((LinkedList)list).length) {
			throw new NoSuchElementException();
		}
		Object rtn = elem.o;
		lastIndex = index++;
		elem = elem.next;
		return rtn;
	}

	public Object previous() {
		if (list.modCount != modCount) {
			throw new ConcurrentModificationException();
		}
		if (index <= 0) {
			throw new NoSuchElementException();
		}
		elem = (elem == null) ? ((LinkedList)list).tail : elem.prev;
		lastIndex = --index;
		return elem.o;
	}

	public void remove() {
		if (list.modCount != modCount) {
			throw new ConcurrentModificationException();
		}
		if (lastIndex == -1) {
			throw new IllegalStateException();
		}
		if (lastIndex == index) {
			elem = ((LinkedList)list).remove(elem);
		} else {			// lastIndex == index - 1
			((LinkedList)list).remove((elem == null) ?
				((LinkedList)list).tail : elem.prev);
			index--;
		}
		modCount++;
		lastIndex = -1;
	}

	public void set(Object o) {
		if (list.modCount != modCount) {
			throw new ConcurrentModificationException();
		}
		if (lastIndex == -1) {
			throw new IllegalStateException();
		}
		if (lastIndex == index) {
			elem.o = o;
		} else {			// lastIndex == index - 1
			((elem == null) ?
			    ((LinkedList)list).tail : elem.prev).o = o;
		}
	}

	public void add(Object o) {
		if (list.modCount != modCount) {
			throw new ConcurrentModificationException();
		}
		((LinkedList)list).add(elem, o);	// OK if elem == null
		modCount++;
		index++;
		if (elem != null) {
			elem = elem.next;
		}
	}
}

