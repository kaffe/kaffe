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

public interface SortedMap extends Map {

public Comparator comparator();
public SortedMap subMap(Object forKey, Object toKey);
public SortedMap headMap(Object toKey);
public SortedMap tailMap(Object fromKey);
public Object firstKey();
public Object lastKey();

}
