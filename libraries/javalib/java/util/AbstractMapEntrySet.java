
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

// This class is used by both HashMap and TreeMap to help with their
// implementation of the Map.entrySet() method, which is supposed to
// return a Set object backed by the set of Map.Entry's in the map.

abstract class AbstractMapEntrySet extends AbstractSet {
	private final Map map;

	AbstractMapEntrySet(Map map) {
		this.map = map;
	}

	// This should return an iterator over the Map.Entry's in the map
	public abstract Iterator iterator();

	// This should return the equivalent Map.Entry from the map, else null
	protected abstract Map.Entry find(Map.Entry e);

	public int size() {
		return map.size();
	}

	public boolean contains(Object o) {
		return (o instanceof Map.Entry) && find((Map.Entry)o) != null;
	}

	public boolean remove(Object o) {
		if (!(o instanceof Map.Entry)) {
			return false;
		}
		Map.Entry ent = find((Map.Entry)o);
		if (ent == null) {
			return false;
		}
		map.remove(ent.getKey());
		return true;
	}

	public void clear() {
		map.clear();
	}
}

