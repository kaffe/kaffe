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

	public void add(int index, Object element);
	public boolean addAll(int index, Collection c);
	public Object get(int index);
	public int indexOf(Object o);
	public int lastIndexOf(Object o);
	public ListIterator listIterator();
	public ListIterator listIterator(int index);
	public Object remove(int index);
	public Object set(int index, Object element);
	public List subList(int fromIndex, int toIndex);

}

