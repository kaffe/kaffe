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

public interface ListIterator extends Iterator {

public boolean hasNext();
public Object next();
public boolean hasPrevious();
public Object previous();
public int nextIndex();
public int previousIndex();
public void remove();
public void set(Object o);
public void add(Object o);

}
