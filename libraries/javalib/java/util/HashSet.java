
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.util;

import java.io.Serializable;

public class HashSet extends AbstractSet
		implements Set, Cloneable, Serializable {
	private static final float DEFAULT_LOADFACTOR = 0.75f;
	private HashMap map;

	public HashSet() {
		map = new HashMap();
	}

	public HashSet(Collection c) {
		map = new HashMap();
		for (Iterator i = c.iterator(); i.hasNext(); ) {
			map.put(i.next(), null);
		}
	}

	public HashSet(int initialCapacity) {
		map = new HashMap(initialCapacity, DEFAULT_LOADFACTOR);
	}

	public HashSet(int initialCapacity, float loadFactor) {
		map = new HashMap(initialCapacity, loadFactor);
	}

	public Iterator iterator() {
		return map.keySet().iterator();
	}

	public int size() {
		return map.size();
	}

	public boolean isEmpty() {
		return size() == 0;
	}

	public boolean contains(Object o) {
		return map.containsKey(o);
	}

	public boolean add(Object o) {
		if (map.containsKey(o)) {
			return false;
		}
		map.put(o, null);
		return true;
	}

	public boolean remove(Object o) {
		if (!map.containsKey(o)) {
			return false;
		}
		map.remove(o);
		return true;
	}

	public void clear() {
		map.clear();
	}

	public Object clone() {
		HashSet clone;
		try {
			clone = (HashSet)super.clone();
		} catch (CloneNotSupportedException e) {
			return null;			// should never happen
		}
		clone.map = (HashMap)map.clone();
		return clone;
	}
}

