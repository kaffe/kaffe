
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

public class Collections {

	// An empty set
	public static final Set EMPTY_SET = new AbstractSet() {
		public int size() {
			return 0;
		}
		public Iterator iterator() {
			return new Iterator() {
				public boolean hasNext() {
					return false;
				}
				public Object next() {
					throw new NoSuchElementException();
				}
				public void remove() {
					throw new IllegalStateException();
				}
			};
		}
	};

	// An empty list
	public static final List EMPTY_LIST = new AbstractList() {
		public int size() {
			return 0;
		}
		public Object get(int index) {
			throw new IndexOutOfBoundsException();
		}
	};

	// This class is not instantiable
	private Collections() {
	}

	public static void sort(List list) {
		sort(list, Arrays.DEFAULT_COMPARATOR);
	}

	public static void sort(List list, Comparator c) {
		Object a[] = list.toArray();
		Arrays.sort(a, c);
		for (int index = a.length - 1; index >= 0; index--) {
			list.set(index, a[index]);
		}
	}

	public static int binarySearch(List list, Object key) {
		return binarySearch(list, key, Arrays.DEFAULT_COMPARATOR);
	}

	public static int binarySearch(List list, Object key, Comparator c) {
		if (list instanceof AbstractSequentialList) {
			Iterator i = list.iterator();
			int index;
			for (index = 0; i.hasNext(); index++) {
				if (c.compare(i.next(), key) == 0) {
					return index;
				}
			}
			return ~index;
		} else {
			return Arrays.binarySearch(list.toArray(), key, c);
		}
	}

	public static void reverse(List list) {
		ListIterator fwd = list.listIterator(0);
		ListIterator rev = list.listIterator(list.size());
		for (int count = list.size() / 2; count > 0; count--) {
			Object o1 = fwd.next();
			Object o2 = rev.previous();
			fwd.set(o2);
			rev.set(o1);
		}
	}

	public static void shuffle(List list) {
		shuffle(list, new Random());
	}

	public static void shuffle(List list, Random rnd) {
		ListIterator i = list.listIterator(list.size());
		for (int pos = list.size() - 1; pos > 1; pos--) {
			int pos2 = rnd.nextInt(pos + 1);
			Object o1 = i.previous();
			if (pos2 != pos) {
				Object o2 = list.get(pos2);
				list.set(pos2, o1);
				list.set(pos, o2);
			}
		}
	}

	public static void fill(List list, Object o) {
		for (ListIterator i = list.listIterator(); i.hasNext(); ) {
			i.next();
			i.set(o);
		}
	}

	public static void copy(List dst, List src) {
		if (dst.size() < src.size()) {
			throw new IndexOutOfBoundsException();
		}
		ListIterator di = dst.listIterator();
		ListIterator si = src.listIterator();
		while (si.hasNext()) {
			di.next();
			di.set(si.next());
		}
	}

	public static Object min(Collection coll) {
		return min(coll, Arrays.DEFAULT_COMPARATOR);
	}

	public static Object min(Collection coll, Comparator comp) {
		if (coll.size() == 0) {
			throw new NoSuchElementException();
		}
		Iterator i = coll.iterator();
		Object min = i.next();
		while (i.hasNext()) {
			Object next = i.next();
			if (comp.compare(next, min) < 0) {
				min = next;
			}
		}
		return min;
	}

	public static Object max(Collection coll) {
		return max(coll, Arrays.DEFAULT_COMPARATOR);
	}

	public static Object max(Collection coll, Comparator comp) {
		if (coll.size() == 0) {
			throw new NoSuchElementException();
		}
		Iterator i = coll.iterator();
		Object max = i.next();
		while (i.hasNext()) {
			Object next = i.next();
			if (comp.compare(next, max) > 0) {
				max = next;
			}
		}
		return max;
	}

	private static class UMCollection extends AbstractCollection
			implements Collection, Serializable {
		private final Collection c;

		UMCollection(Collection c) {
			this.c = c;
		}
		public Iterator iterator() {
			return unmodifiableIterator(c.iterator());
		}
		public int size() {
			return c.size();
		}
		public boolean contains(Object o) {
			return c.contains(o);
		}
		public Object[] toArray(Object[] a) {
			return c.toArray(a);
		}
		public Object[] toArray() {
			return c.toArray();
		}
		public boolean remove(Object o) {
			throw new UnsupportedOperationException();
		}
		public boolean equals(Object o) {
			return ((Object)this).equals(o);
		}
		public int hashCode() {
			return ((Object)this).hashCode();
		}
	}

	public static Collection unmodifiableCollection(Collection c) {
		return new UMCollection(c);
	}

	private static class UMSet extends AbstractSet
			implements Set, Serializable {
		private final Set s;

		UMSet(Set s) {
			this.s = s;
		}
		public Iterator iterator() {
			return unmodifiableIterator(s.iterator());
		}
		public int size() {
			return s.size();
		}
		public boolean contains(Object o) {
			return s.contains(o);
		}
		public Object[] toArray(Object[] a) {
			return s.toArray(a);
		}
		public boolean remove(Object o) {
			throw new UnsupportedOperationException();
		}
	}

	public static Set unmodifiableSet(Set s) {
		return new UMSet(s);
	}

	private static class UMSortedSet implements SortedSet, Serializable {
		private final SortedSet s;

		UMSortedSet(SortedSet s) {
			this.s = s;
		}
		public int size() {
			return s.size();
		}
		public boolean isEmpty() {
			return s.isEmpty();
		}
		public boolean contains(Object o) {
			return s.contains(o);
		}
		public Iterator iterator() {
			return unmodifiableIterator(s.iterator());
		}
		public Object[] toArray() {
			return s.toArray();
		}
		public Object[] toArray(Object[] a) {
			return s.toArray(a);
		}
		public boolean add(Object o) {
			throw new UnsupportedOperationException();
		}
		public boolean remove(Object o) {
			throw new UnsupportedOperationException();
		}
		public boolean containsAll(Collection c) {
			return s.containsAll(c);
		}
		public boolean addAll(Collection c) {
			throw new UnsupportedOperationException();
		}
		public boolean removeAll(Collection c) {
			throw new UnsupportedOperationException();
		}
		public boolean retainAll(Collection c) {
			throw new UnsupportedOperationException();
		}
		public void clear() {
			throw new UnsupportedOperationException();
		}
		public boolean equals(Object o) {
			return s.equals(o);
		}
		public int hashCode() {
			return s.hashCode();
		}
		public Comparator comparator() {
			return s.comparator();
		}
		public SortedSet subSet(Object fromElement, Object toElement) {
			return unmodifiableSortedSet(
				s.subSet(fromElement, toElement));
		}
		public SortedSet headSet(Object toElement) {
			return unmodifiableSortedSet(s.headSet(toElement));
		}
		public SortedSet tailSet(Object fromElement) {
			return unmodifiableSortedSet(s.tailSet(fromElement));
		}
		public Object first() {
			return s.first();
		}
		public Object last() {
			return s.last();
		}
	}

	public static SortedSet unmodifiableSortedSet(SortedSet s) {
		return new UMSortedSet(s);
	}

	public static class UMList extends AbstractList
			implements List, Serializable {
		private final List list;

		UMList(List list) {
			this.list = list;
		}
		public int size() {
			return list.size();
		}
		public Object get(int index) {
			return list.get(index);
		}
		public int indexOf(Object o) {
			return list.indexOf(o);
		}
		public int lastIndexOf(Object o) {
			return list.lastIndexOf(o);
		}
	}

	public static List unmodifiableList(List list) {
		return new UMList(list);
	}

	private static class UMMap extends AbstractMap
			implements Map, Serializable {
		private final Map m;

		UMMap(Map m) {
			this.m = m;
		}
		public int size() {
			return m.size();
		}
		public boolean containsValue(Object value) {
			return m.containsValue(value);
		}
		public boolean containsKey(Object key) {
			return m.containsKey(key);
		}
		public Object get(Object key) {
			return m.get(key);
		}
		public Object remove(Object key) {
			throw new UnsupportedOperationException();
		}
		public void putAll(Map t) {
			throw new UnsupportedOperationException();
		}
		public void clear() {
			throw new UnsupportedOperationException();
		}
		public Set keySet() {
			return unmodifiableSet(m.keySet());
		}
		public Collection values() {
			return unmodifiableCollection(m.values());
		}
		public Set entrySet() {
			return unmodifiableSet(m.entrySet());
		}
	}

	public static Map unmodifiableMap(Map m) {
		return new UMMap(m);
	}

	private static class UMSortedMap implements SortedMap, Serializable {
		private final SortedMap m;

		UMSortedMap(SortedMap m) {
			this.m = m;
		}
		public int size() {
			return m.size();
		}
		public boolean isEmpty() {
			return m.isEmpty();
		}
		public boolean containsValue(Object value) {
			return m.containsValue(value);
		}
		public boolean containsKey(Object key) {
			return m.containsKey(key);
		}
		public Object get(Object key) {
			return m.get(key);
		}
		public Object put(Object key, Object value) {
			throw new UnsupportedOperationException();
		}
		public Object remove(Object key) {
			throw new UnsupportedOperationException();
		}
		public void putAll(Map t) {
			throw new UnsupportedOperationException();
		}
		public void clear() {
			throw new UnsupportedOperationException();
		}
		public Set keySet() {
			return unmodifiableSet(m.keySet());
		}
		public Collection values() {
			return unmodifiableCollection(m.values());
		}
		public Set entrySet() {
			return unmodifiableSet(m.entrySet());
		}
		public Comparator comparator() {
			return m.comparator();
		}
		public SortedMap subMap(Object fromKey, Object toKey) {
			return unmodifiableSortedMap(m.subMap(fromKey, toKey));
		}
		public SortedMap headMap(Object toKey) {
			return unmodifiableSortedMap(m.headMap(toKey));
		}
		public SortedMap tailMap(Object fromKey) {
			return unmodifiableSortedMap(m.tailMap(fromKey));
		}
		public Object firstKey() {
			return m.firstKey();
		}
		public Object lastKey() {
			return m.lastKey();
		}
	}

	public static SortedMap unmodifiableSortedMap(SortedMap m) {
		return new UMSortedMap(m);
	}

	public static Collection synchronizedCollection(Collection c) {
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".synchronizedCollection()");
	}

	public static Set synchronizedSet(Set s) {
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".synchronizedSet()");
	}

	public static SortedSet synchronizedSortedSet(SortedSet s) {
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".synchronizedSortedSet()");
	}

	public static List synchronizedList(List l) {
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".synchronizedList()");
	}

	public static Map synchronizedMap(Map m) {
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".synchronizedMap()");
	}

	public static SortedMap synchronizedSortedMap(SortedMap m) {
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".synchronizedSortedMap()");
	}

	public static Set singleton(final Object o) {
		return new AbstractSet() {
			private final Object object = o;
			public int size() {
				return 1;
			}
			public Iterator iterator() {
				return nCopies(1, o).iterator();
			}
		};
	}

	public static List nCopies(final int num, final Object o) {
		if (num < 0) {
			throw new IllegalArgumentException();
		}
		if (num == 0) {
			return EMPTY_LIST;
		}
		return new AbstractList() {
			public int size() {
				return num;
			}
			public Object get(int index) {
				if (index < 0 || index >= num) {
					throw new IndexOutOfBoundsException();
				}
				return o;
			}
			public int indexOf(Object o2) {
				if (o == null ? o2 == null : o.equals(o2)) {
					return 0;
				}
				return -1;
			}
			public int lastIndexOf(Object o2) {
				if (o == null ? o2 == null : o.equals(o2)) {
					return num - 1;
				}
				return -1;
			}
			public ListIterator listIterator(int index) {
				if (index < 0 || index > num) {
					throw new IndexOutOfBoundsException();
				}
				return new AbstractListIterator(this, num - index);
			}
			public List subList(int fromIndex, int toIndex) {
				if (fromIndex < 0 || toIndex > num) {
					throw new IndexOutOfBoundsException();
				}
				if (fromIndex > toIndex) {
					throw new IllegalArgumentException();
				}
				return nCopies(toIndex - fromIndex, o);
			}
		};
	}

	private static final Comparator REVERSE_COMPARATOR = new Comparator() {
		public int compare(Object o1, Object o2) {
			return -((Comparable)o1).compareTo(o2);
		}
	};

	public static Comparator reverseOrder() {
		return REVERSE_COMPARATOR;
	}

	public static Enumeration enumeration(final Collection c) {
		return new Enumeration() {
			private final Iterator i = c.iterator();
			public boolean hasMoreElements() {
				return i.hasNext();
			}
			public Object nextElement() {
				return i.next();
			}
		};
	}

	private static Iterator unmodifiableIterator(final Iterator i) {
		return new Iterator() {
			public boolean hasNext() {
				return i.hasNext();
			}
			public Object next() {
				return i.next();
			}
			public void remove() {
  				throw new UnsupportedOperationException();
			}
		};
	}
}

