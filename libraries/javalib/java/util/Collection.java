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

public interface Collection {

int size();
boolean isEmpty();
boolean contains(Object o);
Iterator iterator();
Object[] toArray();
Object[] toArray(Object[] a);
boolean add(Object o);
boolean remove(Object o);
boolean containsAll(Collection c);
boolean addAll(Collection c);
boolean removeAll(Collection c);
boolean retainAll(Collection c);
void clear();
boolean equals(Object o);
int hashCode();

}
