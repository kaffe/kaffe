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

import java.io.Serializable;

public class Vector extends AbstractList
		implements Cloneable, Serializable {

private static final long serialVersionUID = -2767605614048989439L;

protected int capacityIncrement;
protected int elementCount;
protected Object[] elementData;

public Vector() {
	this( 10, 0);
}

public Vector ( int initialCapacity ) {
	this( initialCapacity, 0);
}

// Here "increment" may be 0 to indicate doubling on reallocation
public Vector(int initialCapacity, int increment) {
	elementData = new Object[initialCapacity];
	capacityIncrement = increment;
}

public Vector(Collection c) {
	this(c.size());
	for (Iterator i = c.iterator(); i.hasNext(); ) {
		addElement(i.next());
	}
}

public void addElement(Object obj) {
	insertElementAt(obj, size());
}

public boolean add(Object obj) {
	insertElementAt(obj, size());
	return true;
}

public void add(int index, Object obj) {
	insertElementAt(obj, index);
}

public boolean addAll(Collection c) {
  return addAll(size(), c);
}

public synchronized boolean addAll(int index, Collection c) {
  ensureCapacity(size() + c.size());
  for (Iterator i = c.iterator(); i.hasNext(); ) {
    add(index++, i.next());
  }
  return c.size() > 0;
}

public int capacity() {
	return elementData.length;
}

public void clear() {
	removeAllElements();
}

public synchronized Object clone () {
        Vector v;

        // we must invoke super.clone() to ensure that subclasses
        // such as Stack are cloned properly.
        try {
                v = (Vector)super.clone();
                v.elementData = (Object[]) elementData.clone();

                return v;
        }
        catch (CloneNotSupportedException _) {
                return null;
        }
}

public boolean contains(Object elem) {
	return (indexOf(elem) != -1);
}

public boolean containsAll(Collection c) {
	return super.containsAll(c);
}

public synchronized void copyInto ( Object anArray[] ) {
	System.arraycopy( elementData, 0, anArray, 0, elementCount);
}

public synchronized Object elementAt ( int index ) {
  // required because we might have a large enough, pre-allocated, empty element
  // array that doesn't give us (physical) access errors
  if ( index >= elementCount )
	  throw new ArrayIndexOutOfBoundsException("Array index out of range: " 
						   + Integer.toString(index)
						   + " >= " + elementCount);

  return elementData[index];
}

public synchronized Enumeration elements() {
	return new Enumeration() {
		int index = 0;
		public boolean hasMoreElements() {
			return index < elementCount;
		}
		public Object nextElement() {
			if (index >= elementCount) {
				throw new NoSuchElementException();
			}
			return elementData[index++];
		}
	};
}

public synchronized void ensureCapacity(int newCapacity) { 
	if (elementData.length < newCapacity) {
		modCount++;
		Object oldBuffer[] = elementData;
		elementData = new Object[newCapacity];
		System.arraycopy(oldBuffer, 0, elementData, 0, elementCount);
	}
}

public synchronized boolean equals(Object o) {
	// this is an optimisation when comparing against oneself.
	if (o == this) {
		return true;
	}

	if (o instanceof List) {
		List other = (List) o;

		// compare list size
		if (size() != other.size()) {
			return false;
		}

		// compare element by element.
		Iterator iter = other.iterator();
		/* walking the elementData array directly avoids
		 * allocation of a ListIterator, as would be the case
		 * in AbstractList.equals . That seems to be the whole
		 * point of reimplementing this method here.
		 */
		int i = 0;
		while (iter.hasNext() && i < elementCount) {
			Object this_elem = elementData[i];
			Object other_elem = iter.next();

			// if an element differs, return false.
			if ((this_elem == null && other_elem != null)
			    || !this_elem.equals(other_elem)) {
				return false;
			}

			++i;
		}

		/* since no element differs,
		 * and they have the same size,
		 * the lists are equal.
		 */
		return true;
	}

	// The object is not an instance of List.
	return false;
}

public synchronized Object firstElement () {
	if ( elementCount <= 0 ) {
		throw new NoSuchElementException();
	}
	return elementData[0];
}

public Object get(int idx) {
	return (elementAt(idx));
}

public int hashCode() {
	/* Algorithm found at page 965 of Java Class Libraries
	 * Second Edition Volume 1 Supplement.
	 */
	int hashCode = 1;
	for (int i = 0; i < size(); ++i) {
		Object elem = elementData[i];
		hashCode = 31 * hashCode
			+ (elem == null ? 0 : elem.hashCode());
	}

	return hashCode;
}

private void increaseCapacity() {
	int newCapacity = elementData.length;
	if (capacityIncrement > 0) {
		newCapacity += capacityIncrement;
	}
	else if (newCapacity == 0) {
		newCapacity = 10;
	}
	else {
		newCapacity *= 2;
	}
	ensureCapacity(newCapacity);
}

public int indexOf(Object elem) {
	return indexOf(elem, 0);
}

public synchronized int indexOf(Object elem, int index) {
	for (int pos = index; pos < elementCount; pos++) {
		Object obj = elementData[pos];
		if (elem == obj || (elem != null && elem.equals(obj))) {
			return (pos);
		}
	}
	return (-1);
}

public synchronized void insertElementAt ( Object obj, int index ) {

	int initialModCount = modCount;
	if ( elementCount == elementData.length ) {
		increaseCapacity();
	}
	modCount = initialModCount + 1;

	if ( (index > elementCount) || (index < 0) ) {
		throw new ArrayIndexOutOfBoundsException();
	}
	else if ( index < elementCount ) {
		System.arraycopy( elementData, index, elementData, index+1, elementCount-index);
	}

	elementData[index] = obj;
	elementCount++;
}

public boolean isEmpty () {
	return (elementCount == 0);
}

public synchronized Object lastElement () {
	if ( elementCount == 0 ) {
		throw new NoSuchElementException();
	}
	return elementData[elementCount-1];
}

public int lastIndexOf(Object elem) {
	return (lastIndexOf(elem, size()-1));
}

public synchronized int lastIndexOf(Object elem, int index) {
	if (index >= elementCount)
		throw new ArrayIndexOutOfBoundsException();	// per JDK 1.3
	for (int pos = index; pos >= 0; pos--) {
		Object obj = elementData[pos];
		if (elem == obj || elem.equals(obj)) {
			return (pos);
		}
	}
	return (-1);
}

public synchronized Object remove(int idx) {
	Object obj = elementAt(idx);
	removeElementAt(idx);
	return (obj);
}

public boolean remove(Object o) {
	int index = indexOf(o);

	if (index == -1) {
		return false;
	}
	else {
		remove(index);
		return true;
	}
}

public synchronized boolean removeAll(Collection c) {
	int oldSize = size();

	for (int i = 0; i < size(); ++i) {
		Object elem = elementData[i];
		if (c.contains(elem)) {
			remove(i);
			/* do this index again,
			 * after the removal the successor is in its place
			 */
			--i;
		}
	}

	return oldSize == size();
}

public synchronized void removeAllElements () {
	for ( int i=elementCount-1; i>= 0; i-- ) {
		elementData[i] = null;
	}
	elementCount = 0;
	modCount++;
}

public synchronized boolean removeElement(Object obj) {
	int index = indexOf(obj);
	if (index == -1)
		return false;
	removeElementAt(index);
	return true;
}

public synchronized void removeElementAt ( int index ) {
	if ( index >= elementCount ) {
		throw new ArrayIndexOutOfBoundsException();
	}
	modCount++;

	System.arraycopy( elementData, index+1, elementData, index, elementCount-index-1);
	elementCount--;

	elementData[elementCount] = null;
}

public Object set(int index, Object obj) {
	if (index >= elementCount) {
		throw new ArrayIndexOutOfBoundsException();
	}
	Object old = elementData[index];
	elementData[index] = obj;
	return old;
}

public synchronized void setElementAt(Object obj, int index) {
	set(index, obj);
}

public synchronized void setSize(int newSize) {
	int initialModCount = modCount;
	ensureCapacity(newSize);
	modCount = initialModCount + 1;
	elementCount = newSize;
}

public int size() {
	return elementCount;
}

public List subList(int from, int to) {
	return super.subList(from, to);
}

public synchronized Object[] toArray() {
    Object objs[] = new Object[elementCount];
    copyInto(objs);
    return objs;
}

public synchronized Object[] toArray( Object anArray[] ) {
    if (anArray.length < elementCount) {
	anArray = (Object[])java.lang.reflect.Array.newInstance(
		anArray.getClass().getComponentType(),
		elementCount);
    }
    copyInto(anArray);
    for (int i = anArray.length; i-- > elementCount; ) {
	anArray[i] = null;
    }
    return anArray;
}

public synchronized String toString() {
	StringBuffer result = new StringBuffer();

	result.append("[");
	for (int pos = 0; pos < elementCount; pos++) {
		if (pos > 0) {
			result.append(", ");
		}

		Object data = elementData[pos];
		/* if data is null, it is represented as "null".
		 * otherwise its toString() method is called.
		 */
		String data_as_string = (data == null) ? "null" : data.toString();
		result.append(data_as_string);
	}
	result.append("]");
	return (result.toString());
}

public synchronized void trimToSize() {
	if (elementCount != elementData.length) {
		modCount++;
		Object oldBuffer[] = elementData;
		elementData = new Object[elementCount];
		System.arraycopy(oldBuffer, 0, elementData, 0, elementCount);
	}
}
}

