
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

public abstract class AbstractMap implements Map {
	private Set keyset;
	private Collection valcol;

	protected AbstractMap() {
	}

	public int size() {
		return entrySet().size();
	}

	public boolean isEmpty() {
		return size() == 0;
	}

	public boolean containsValue(Object value) {
		for (Iterator i = entrySet().iterator(); i.hasNext(); ) {
			Object next = ((Map.Entry)i.next()).getValue();
			if (value == null ? next == null : value.equals(next)) {
				return true;
			}
		}
		return false;
	}

	public boolean containsKey(Object key) {
		for (Iterator i = entrySet().iterator(); i.hasNext(); ) {
			Object next = ((Map.Entry)i.next()).getKey();
			if (key == null ? next == null : key.equals(next)) {
				return true;
			}
		}
		return false;
	}

	public Object get(Object key) {
		for (Iterator i = entrySet().iterator(); i.hasNext(); ) {
			Map.Entry e = (Map.Entry)i.next();
			Object ekey = e.getKey();
			if (key == null ? ekey == null : key.equals(ekey)) {
				return e.getValue();
			}
		}
		return null;
	}

	public Object put(Object key, Object value) {
		throw new UnsupportedOperationException();
	}

	public Object remove(Object key) {
		for (Iterator i = entrySet().iterator(); i.hasNext(); ) {
			Map.Entry e = (Map.Entry)i.next();
			Object ekey = e.getKey();
			if (key == null ? ekey == null : key.equals(ekey)) {
				Object value = e.getValue();
				i.remove();
				return value;
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
		entrySet().clear();
	}

	public Set keySet() {
		if (keyset != null) {
			return keyset;
		}
		keyset = new AbstractSet() {
			public int size() {
				return AbstractMap.this.size();
			}
			public boolean contains(Object o) {
				return containsKey(o);
			}
			public boolean remove(Object o) {
				int beforeSize = size();
				AbstractMap.this.remove(o);
				return size() != beforeSize;
			}
			public void clear() {
				AbstractMap.this.clear();
			}
			public Iterator iterator() {
				return new Iterator() {
					private Iterator i = entrySet().iterator();
					public boolean hasNext() {
						return i.hasNext();
					}
					public Object next() {
						return ((Map.Entry)i.next()).getKey();
					}
					public void remove() {
						i.remove();
					}
				};
			}
		};
		return keyset;
	}

	public Collection values() {
		if (keyset != null) {
			return keyset;
		}
		valcol = new AbstractCollection() {
			public int size() {
				return AbstractMap.this.size();
			}
			public void clear() {
				AbstractMap.this.clear();
			}
			public Iterator iterator() {
				return new Iterator() {
					private Iterator i = entrySet().iterator();
					public boolean hasNext() {
						return i.hasNext();
					}
					public Object next() {
						return ((Map.Entry)i.next()).getValue();
					}
					public void remove() {
						i.remove();
					}
				};
			}
		};
		return valcol;
	}

	public abstract Set entrySet();

	public boolean equals(Object o) {
		if (!(o instanceof Map)) {
			return false;
		}
		Map omap = (Map)o;
		if (omap.size() != size()) {
			return false;
		}
		for (Iterator i = entrySet().iterator(); i.hasNext(); ) {
			Map.Entry ent = (Map.Entry)i.next();
			if (!omap.containsKey(ent.getKey())) {
				return false;
			}
			Object oval = omap.get(ent.getKey());
			Object myval = ent.getValue();
			if (!(myval == null ?
			    oval == null : myval.equals(oval))) {
				return false;
			}
		}
		return true;
	}

	public int hashCode() {
		int hash = 0;
		for (Iterator i = entrySet().iterator(); i.hasNext(); ) {
			hash += i.next().hashCode();
		}
		return hash;
	}

	public String toString() {
		StringBuffer b = new StringBuffer(5 * size());
		b.append("{");
		boolean first = true;
		for (Iterator i = entrySet().iterator(); i.hasNext(); ) {
			Map.Entry ent = (Map.Entry)i.next();
			if (!first) {
				b.append(", ");
			}
			b.append(ent.getKey() + "=" + ent.getValue());
			first = false;
		}
		b.append("}");
		return b.toString();
	}

}

