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

public interface Set extends Collection {

public int size();
public boolean isEmpty();
public boolean contains(Object o);
public Iterator iterator();
public Object[] toArray();
public Object[] toArray(Object[] a);
public boolean add(Object o);
public boolean remove(Object o);
public boolean containsAll(Collection c);
public boolean addAll(Collection c);
public boolean removeAll(Collection c);
public boolean retainAll(Collection c);
public void clear();
public boolean equals(Object o);
public int hashCode();

}
