package kaffe.util;

import java.util.Enumeration;
import java.util.NoSuchElementException;
import java.util.Vector;

/**
 * VectorSnapshot - class for (cached) Vector enumerations that can stand
 * overlapping element removal. A enumeration based on this is guaranteed to
 * touch all elements present at the time the iteration was started (regardless
 * of which elements will be removed from the Vector during the enumeration
 * process). Note that the standard Vector enumeration is index based, i.e.
 * it does not revert indices in case a element is removed as a consequence of
 * the enumeration process.
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */
public class VectorSnapshot
  implements Enumeration
{
	private Object[] elements;
	private int index;
	private int size;
	private VectorSnapshot next;
	private static VectorSnapshot cache;

public VectorSnapshot ( Vector v ) {
	size = v.size();
	elements = new Object[size];
	v.copyInto( elements);
}

public static synchronized VectorSnapshot getCached ( Vector v ) {
	VectorSnapshot e, l = null;

	if ( cache == null ) {
		return new VectorSnapshot( v);
	}
	else {
		int n = v.size();
		
		for ( e=cache; (e != null); l=e, e=e.next ){
			if ( e.elements.length <= n ) {
				if ( cache == e ) {
					cache = e.next;
				}
				else {
					l.next = e.next;
				}
				e.next = null;
				v.copyInto( e.elements);
				e.size = n;
				return e;
			}
		}
		
		return new VectorSnapshot( v);
	}
}

public boolean hasMoreElements () {
	if ( index < size ){
		return true;
	}
	else {
		recycle();  // nothing we can do with a completed SnapshotEnumerator
		return false;
	}
}

public Object nextElement () {
	if ( index >= size ) {
		recycle();
		throw new NoSuchElementException();
	}
	else {
		return elements[index++];
	}
}

public void recycle () {
	// bailout if this would waste too much memory
	if ( elements.length > 128 )
		return;

	for ( int i=0; i<size; i++ )  // avoid memory leak
		elements[i] = null;
	size = index = 0;

	synchronized ( VectorSnapshot.class ) {
		next = cache;
		cache = this;
	}
}
}
