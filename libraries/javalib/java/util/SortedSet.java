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

Comparator comparator();
SortedSet subSet(Object fromElement, Object toElement);
SortedSet headSet(Object toElement);
SortedSet tailSet(Object fromElement);
Object first();
Object last();

}
