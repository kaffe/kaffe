
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

public abstract class AbstractSequentialList extends AbstractList {

	protected AbstractSequentialList() {
	}

	public Object get(int index) {
		if (index == size()) {
			throw new IndexOutOfBoundsException();
		}
		return listIterator(index).next();
	}

	public Object set(int index, Object element) {
		if (index == size()) {
			throw new IndexOutOfBoundsException();
		}
		ListIterator i = listIterator(index);
		Object old = i.next();
		i.set(element);
		return old;
	}

	public void add(int index, Object element) {
		listIterator(index).add(element);
	}

	public Object remove(int index) {
		if (index == size()) {
			throw new IndexOutOfBoundsException();
		}
		ListIterator i = listIterator(index);
		Object old = i.next();
		i.remove();
		return old;
	}

	public boolean addAll(int index, Collection c) {
		ListIterator li = listIterator(index);
		boolean rtn = false;
		for (Iterator ci = c.iterator(); ci.hasNext(); ) {
			li.add(ci.next());
			li.next();
			rtn = true;
		}
		return rtn;
	}

	public Iterator iterator() {
		return listIterator();
	}

	public abstract ListIterator listIterator(int index);
}

