
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

public class HashMap extends AbstractMap
		implements Map, Cloneable, Serializable {
	private static final int DEFAULT_CAPACITY = 11;
	private static final float DEFAULT_LOADFACTOR = 0.75f;
	private float loadFactor;
	private Entry[] table;
	private int size;

	private static class Entry implements Map.Entry {
		Object key;
		Object val;
		Entry next;

		Entry(Object key, Object val) {
			this.key = key;
			this.val = val;
			this.next = null;
		}

		public Object getKey() {
			return key;
		}

		public Object getValue() {
			return val;
		}

		public Object setValue(Object val) {
			Object old = val;
			this.val = val;
			return old;
		}

		public boolean equals(Object o) {
			if (!(o instanceof Map.Entry)) {
				return false;
			}
			Map.Entry me = (Map.Entry)o;
			return (this.key == null ? me.getKey() == null
				: this.key.equals(me.getKey()))
			    && (this.val == null ? me.getValue() == null
				: this.val.equals(me.getValue()));
		}

		public int hashCode() {
			return (key == null ? 0 : key.hashCode())
			    ^ (val == null ? 0 : val.hashCode());
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
				    e.val == null : val.equals(e.val)) {
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
		return e == null ? null : e.val;
	}

	public Object put(Object key, Object val) {

		// See if key already exists
		int bucket = bucket(key);
		Entry e = find(key, bucket);
		if (e != null) {
			Object old = e.val;
			e.val = val;
			return old;
		}

		// See if we need to increase capacity
		if ((float) size / (float) table.length >= loadFactor) {
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
			bucket = bucket(key);
		}

		// Create and add new entry
		e = new Entry(key, val);
		e.next = table[bucket];
		table[bucket] = e;
		size++;
		return null;
	}

	public Object remove(Object key) {
		int bucket = bucket(key);
		Entry first = table[bucket];
		if (first == null) {
			return null;
		}
		if (key == null ? first.key == null : key.equals(first.key)) {
			Object val = first.val;
			table[bucket] = first.next;
			size--;
			return val;
		}
		for (Entry e = first; e.next != null; e = e.next) {
			if (key == null ?
			    e.next.key == null : key.equals(e.next.key)) {
				Object val = e.next.val;
				e.next = e.next.next;
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
		size = 0;
	}

	public Object clone() {
		HashMap newmap = new HashMap(table.length, loadFactor);
		for (int bucket = 0; bucket < table.length; bucket++) {
			Entry first = table[bucket];
			if (first == null) {
				continue;
			}
			Entry newent = new Entry(first.key, first.val);
			newmap.table[bucket] = newent;
			for (Entry e = first.next; e != null; e = e.next) {
				newent.next = new Entry(e.key, e.val);
				newent = newent.next;
			}
		}
		return newmap;
	}

	public Set keySet() {
		return new AbstractSet() {
			public int size() {
				return HashMap.this.size;
			}
			public boolean contains(Object o) {
				return containsKey(o);
			}
			public boolean remove(Object o) {
				int beforeSize = size;
				HashMap.this.remove(o);
				return size != beforeSize;
			}
			public void clear() {
				HashMap.this.clear();
			}
			public Iterator iterator() {
				return new Iterator() {
					private Iterator i = new EntryIterator();
					public boolean hasNext() {
						return i.hasNext();
					}
					public Object next() {
						return ((Entry)i.next()).key;
					}
					public void remove() {
						i.remove();
					}
				};
			}
		};
	}

	public Collection values() {
		return new AbstractCollection() {
			public int size() {
				return HashMap.this.size;
			}
			public void clear() {
				HashMap.this.clear();
			}
			public Iterator iterator() {
				return new Iterator() {
					Iterator ei = new EntryIterator();
					public boolean hasNext() {
						return ei.hasNext();
					}
					public Object next() {
						return ((Entry)ei.next()).val;
					}
					public void remove() {
						ei.remove();
					}
				};
			}
		};
	}

	public Set entrySet() {
		return new AbstractSet() {
			public int size() {
				return HashMap.this.size;
			}
			public boolean contains(Object o) {
				if (!(o instanceof Map.Entry)) {
					return false;
				}
				return find((Map.Entry)o) != null;
			}
			public boolean remove(Object o) {
				if (!(o instanceof Map.Entry)) {
					return false;
				}
				Entry ent = find((Map.Entry)o);
				if (ent == null) {
					return false;
				}
				HashMap.this.remove(ent.key);
				return true;
			}
			public void clear() {
				HashMap.this.clear();
			}
			public Iterator iterator() {
				return new EntryIterator();
			}
			private Entry find(Map.Entry oent) {
				Entry myent = HashMap.this.find(oent.getKey());
				if (myent != null
				  && (myent.val == null ?
				    oent.getValue() == null :
				    myent.val.equals(oent.getValue()))) {
					return myent;
				}
				return null;
			}
		};
	}

	private Entry find(Object key) {
		return find(key, bucket(key));
	}

	private Entry find(Object key, int bucket) {
		for (Entry e = table[bucket]; e != null; e = e.next) {
			if (key == null ? e.key == null : key.equals(e.key)) {
				return e;
			}
		}
		return null;
	}

	private int bucket(Object key) {
		return ((key == null) ? 0 : key.hashCode()) % table.length;
	}

	private int bucket(Object key, int length) {
		return ((key == null) ? 0 : key.hashCode()) % length;
	}

	// An iterator over all the Map.Entry's in this hashtable
	private class EntryIterator implements Iterator {
		private int bucket;
		private Entry next, prev;

		EntryIterator() {
			bucket = -1;
			nextBucket();
		}

		public boolean hasNext() {
			return next != null;
		}

		public Object next() {
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
			if (prev == null) {
				throw new IllegalStateException();
			}
			HashMap.this.remove(prev.key);
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

