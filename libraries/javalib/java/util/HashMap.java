
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

// Simple implementation of a hash table. We keep an array of buckets,
// where each bucket points to a singly linked list of Entry's (see below).

public class HashMap extends AbstractMap
		implements Map, Cloneable, Serializable {
	private static final int DEFAULT_CAPACITY = 11;
	private static final float DEFAULT_LOADFACTOR = 0.75f;
	float loadFactor;
	Entry[] table;
	private int modCount;
	private int size;

	// The buckets point to a linked list of these
	private class Entry extends AbstractMapEntry {
		private Entry next;

		Entry(Object key, Object value) {
			super(key, value);
			next = null;
		}

		public void changeValue(Object newValue) {
			HashMap.this.put(key, newValue);
		}
	}

	public HashMap() {
		this(DEFAULT_CAPACITY, DEFAULT_LOADFACTOR);
	}

	public HashMap(int initialCapacity) {
		this(initialCapacity, DEFAULT_LOADFACTOR);
	}

	public HashMap(int initialCapacity, float loadFactor) {
		if (initialCapacity < 0 || loadFactor <= 0.0f) {
			throw new IllegalArgumentException();
		}
		if (initialCapacity == 0) {
			initialCapacity = 1;
		}
		table = new Entry[initialCapacity];
		this.loadFactor = loadFactor;
	}

	public HashMap(Map t) {
		int initialCapacity = t.size() * 2;
		if (initialCapacity < 11) {
			initialCapacity = 11;
		}
		table = new Entry[initialCapacity];
		this.loadFactor = DEFAULT_LOADFACTOR;
		putAll(t);
	}

	public int size() {
		return size;
	}

	public boolean isEmpty() {
		return size == 0;
	}

	public boolean containsValue(Object val) {
		for (int bucket = 0; bucket < table.length; bucket++) {
			for (Entry e = table[bucket]; e != null; e = e.next) {
				if (val == null ?
				    e.value == null : val.equals(e.value)) {
					return true;
				}
			}
		}
		return false;
	}

	public boolean containsKey(Object key) {
		return find(key) != null;
	}

	public Object get(Object key) {
		Entry e = find(key);
		return e == null ? null : e.value;
	}

	public Object put(Object key, Object val) {

		// See if key already exists
		int bucket = bucket(key, table.length);
		Entry e = find(key, bucket);
		if (e != null) {
			Object old = e.value;
			e.value = val;
			return old;
		}

		// See if we need to increase capacity
		if ((float) size / (float) table.length >= loadFactor) {
			rehash();
			bucket = bucket(key, table.length);
		}

		// Create and add new entry
		e = new Entry(key, val);
		e.next = table[bucket];
		table[bucket] = e;
		modCount++;
		size++;
		return null;
	}

	public Object remove(Object key) {
		int bucket = bucket(key, table.length);
		Entry first = table[bucket];
		if (first == null) {
			return null;
		}

		// Special case first entry in chain
		if (key == null ? first.key == null : key.equals(first.key)) {
			Object val = first.value;
			table[bucket] = first.next;
			modCount++;
			size--;
			return val;
		}

		// Maintain pointer to previous entry while going down list
		for (Entry e = first; e.next != null; e = e.next) {
			if (key == null ?
			    e.next.key == null : key.equals(e.next.key)) {
				Object val = e.next.value;
				e.next = e.next.next;
				modCount++;
				size--;
				return val;
			}
		}
		return null;
	}

	public void putAll(Map t) {
		for (Iterator i = t.entrySet().iterator(); i.hasNext(); ) {
			Map.Entry e = (Map.Entry)i.next();
			put(e.getKey(), e.getValue());
		}
	}

	public void clear() {
		table = new Entry[DEFAULT_CAPACITY];
		modCount++;
		size = 0;
	}

	public Object clone() {

		// Clone this object
		HashMap clone;
		try {
			clone = (HashMap)super.clone();
		} catch (CloneNotSupportedException e) {
			return null;			// should never happen
		}

		// Make a shallow copy of hashmap (ie, just the buckets)
		clone.table = new Entry[table.length];
		for (int bucket = 0; bucket < table.length; bucket++) {
			Entry first = table[bucket];
			if (first == null) {
				continue;
			}
			Entry newent = new Entry(first.key, first.value);
			clone.table[bucket] = newent;
			for (Entry e = first.next; e != null; e = e.next) {
				newent.next = new Entry(e.key, e.value);
				newent = newent.next;
			}
		}
		return clone;
	}

	public Set entrySet() {
		return new AbstractMapEntrySet(this) {
			public Iterator iterator() {
				return new EntryIterator();
			}
			protected Map.Entry find(Map.Entry oent) {
				Entry myent = HashMap.this.find(oent.getKey());
				return oent.equals(myent) ? myent : null;
			}
		};
	}

	void rehash() {
		Entry[] newtab = new Entry[table.length * 2];
		for (int buck2 = 0; buck2 < table.length; buck2++) {
			for (Entry e2 = table[buck2]; e2 != null; ) {
				int newbuck;
				Entry next = e2.next;
				newbuck = bucket(e2.key, newtab.length);
				e2.next = newtab[newbuck];
				newtab[newbuck] = e2;
				e2 = next;
			}
		}
		table = newtab;
	}

	private Entry find(Object key) {
		return find(key, bucket(key, table.length));
	}

	private Entry find(Object key, int bucket) {
		for (Entry e = table[bucket]; e != null; e = e.next) {
			if (key == null ? e.key == null : key.equals(e.key)) {
				return e;
			}
		}
		return null;
	}

	private final static int bucket(Object key, int length) {
		int hash = (key == null) ? 0 : key.hashCode();
		if (hash < 0) {
			hash = -hash;
		}
		return hash % length;
	}

	// An iterator over all the Entry's in this hashtable.
	// This iterator is "fail-fast".
	private class EntryIterator implements Iterator {
		private int bucket;
		private int modCount;
		private Entry next, prev;

		EntryIterator() {
			modCount = HashMap.this.modCount;
			bucket = -1;
			nextBucket();
		}

		public boolean hasNext() {
			if (modCount != HashMap.this.modCount) {
				throw new ConcurrentModificationException();
			}
			return next != null;
		}

		public Object next() {
			if (modCount != HashMap.this.modCount) {
				throw new ConcurrentModificationException();
			}
			if (next == null) {
				throw new NoSuchElementException();
			}
			prev = next;
			if ((next = next.next) == null) {
				nextBucket();
			}
			return prev;
		}

		public void remove() {
			if (modCount != HashMap.this.modCount) {
				throw new ConcurrentModificationException();
			}
			if (prev == null) {
				throw new IllegalStateException();
			}
			HashMap.this.remove(prev.key);
			modCount++;
			prev = null;
		}

		private void nextBucket() {
			while (++bucket < table.length
			    && table[bucket] == null);
			if (bucket < table.length) {
				next = table[bucket];
			}
		}
	}
}

