/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util;

public interface List extends Collection {

	void add(int index, Object element);
	boolean addAll(int index, Collection c);
	Object get(int index);
	int indexOf(Object o);
	int lastIndexOf(Object o);
	ListIterator listIterator();
	ListIterator listIterator(int index);
	Object remove(int index);
	Object set(int index, Object element);
	List subList(int fromIndex, int toIndex);

}

