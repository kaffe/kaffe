
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

public class TreeSet extends AbstractSet
		implements SortedSet, Cloneable, Serializable {
	private final SortedMap map;

	public TreeSet() {
		map = new TreeMap();
	}

	public TreeSet(Comparator c) {
		map = new TreeMap(c);
	}

	public TreeSet(Collection c) {
		map = new TreeMap();
		for (Iterator i = c.iterator(); i.hasNext(); ) {
			map.put(i.next(), null);
		}
	}

	public TreeSet(SortedSet s) {
		map = new TreeMap(s.comparator());
		for (Iterator i = s.iterator(); i.hasNext(); ) {
			map.put(i.next(), null);
		}
	}

	TreeSet(SortedMap map) {
		this.map = map;
	}

	public Iterator iterator() {
		return new Iterator() {
			private final Iterator i = map.entrySet().iterator();
			public boolean hasNext() {
				return i.hasNext();
			}
			public Object next() {
				Map.Entry e = (Map.Entry)i.next();
				return e.getKey();
			}
			public void remove() {
				i.remove();
			}
		};
	}

	public int size() {
		return map.size();
	}

	public boolean isEmpty() {
		return map.isEmpty();
	}

	public boolean contains(Object o) {
		return map.containsKey(o);
	}

	public boolean add(Object o) {
		boolean rtn = !map.containsKey(o);
		map.put(o, null);
		return rtn;
	}

	public boolean remove(Object o) {
		if (map.containsKey(o)) {
			map.remove(o);
			return true;
		}
		return false;
	}

	public void clear() {
		map.clear();
	}

	public boolean addAll(Collection c) {
		int originalSize = map.size();
		for (Iterator i = c.iterator(); i.hasNext(); ) {
			map.put(i.next(), null);
		}
		return map.size() != originalSize;
	}

	public SortedSet subSet(Object fromElement, Object toElement) {
		return new TreeSet(map.subMap(fromElement, toElement));
	}

	public SortedSet headSet(Object toElement) {
		return new TreeSet(map.headMap(toElement));
	}

	public SortedSet tailSet(Object fromElement) {
		return new TreeSet(map.tailMap(fromElement));
	}

	public Comparator comparator() {
		return map.comparator();
	}

	public Object first() {
		return map.firstKey();
	}

	public Object last() {
		return map.lastKey();
	}

	public Object clone() {
		return new TreeSet((SortedMap)((TreeMap)map).clone());
	}
}

