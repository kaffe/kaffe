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

public interface SortedSet extends Set {

public Comparator comparator();
public SortedSet subSet(Object fromElement, Object toElement);
public SortedSet headSet(Object toElement);
public SortedSet tailSet(Object fromElement);
public Object first();
public Object last();

}
