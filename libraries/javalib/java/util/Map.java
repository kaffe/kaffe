
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

	public interface Entry {
		Object getKey();
		Object getValue();
		Object setValue(Object value);
		boolean equals(Object o);
		int hashCode();
	}

	int size();
	boolean isEmpty();
	boolean containsKey(Object o);
	boolean containsValue(Object o);
	Object get(Object key);
	Object put(Object key, Object value);
	void putAll(Map m);
	void clear();
	Set keySet();
	Set entrySet();
	Collection values();
	boolean equals(Object o);
	int hashCode();
	Object remove(Object o);

}

