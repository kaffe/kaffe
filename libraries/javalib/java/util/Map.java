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

public interface Map {

public static interface Entry {

public Object getKey();
public Object getValue();
public Object setValue(Object value);
public boolean equals(Object o);
public int hashCode();

}

public int size();
public boolean isEmpty();
public boolean containsKey(Object o);
public boolean containsValue(Object o);
public Object get(Object key);
public Object put(Object key, Object value);
public void putAll(Map m);
public void clear();
public Set keySet();
public Set entrySet();
public Collection values();
public boolean equals(Object o);
public int hashCode();
public Object remove(Object o);

}
