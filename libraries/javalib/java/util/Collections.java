
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

	// An empty Set
	public static final Set EMPTY_SET = new EmptySet();

	private static class EmptySet extends AbstractSet
			implements Serializable {
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
	}

	// An empty List
	public static final List EMPTY_LIST = new EmptyList();

	private static class EmptyList extends AbstractList
			implements Serializable {
		public int size() {
			return 0;
		}
		public Object get(int index) {
			throw new IndexOutOfBoundsException();
		}
	}

	// An empty Map
	public static final Map EMPTY_MAP = new EmptyMap();

	private static class EmptyMap extends AbstractMap
			implements Serializable {
		public Set entrySet() {
			return EMPTY_SET;
		}
	}

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

	/** @since 1.4 */
	public static ArrayList list(Enumeration e) {
		ArrayList list = new ArrayList();

		while (e.hasMoreElements()) {
			list.add(e.nextElement());
		}

		return list;
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

	public static Collection unmodifiableCollection(Collection c) {
		return new UMCollection(c);
	}

	public static Set unmodifiableSet(Set s) {
		return new UMSet(s);
	}

	public static SortedSet unmodifiableSortedSet(SortedSet s) {
		return new UMSortedSet(s);
	}

	public static List unmodifiableList(List list) {
		return new UMList(list);
	}

	public static Map unmodifiableMap(Map m) {
		return new UMMap(m);
	}

	public static SortedMap unmodifiableSortedMap(SortedMap m) {
		return new UMSortedMap(m);
	}

	public static Collection synchronizedCollection(Collection c) {
		return new SyncCollection(c);
	}

	public static Set synchronizedSet(Set s) {
		return new SyncSet(s);
	}

	public static SortedSet synchronizedSortedSet(SortedSet s) {
		return new SyncSortedSet(s);
	}

	public static List synchronizedList(List l) {
		return new SyncList(l);
	}

	public static Map synchronizedMap(Map m) {
		return new SyncMap(m);
	}

	public static SortedMap synchronizedSortedMap(SortedMap m) {
		return new SyncSortedMap(m);
	}

	public static Set singleton(final Object o) {
		return new AbstractSet() {
			public int size() {
				return 1;
			}
			public Iterator iterator() {
				return nCopies(1, o).iterator();
			}
		};
	}

	public static List singletonList(Object o) {
		return new CopyList(1, o);
	}

	public static Map singletonMap(Object key, Object value) {
		HashMap map = new HashMap(1);
		map.put(key, value);
		return unmodifiableMap(map);
	}

	public static List nCopies(final int num, final Object o) {
		if (num < 0) {
			throw new IllegalArgumentException();
		}
		if (num == 0) {
			return EMPTY_LIST;
		}
		return new CopyList(num, o);
	}

	private static class CopyList extends AbstractList
			implements Serializable {
		private final Object o;
		private final int num;
		CopyList(int num, Object o) {
			this.num = num;
			this.o = o;
		}
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
			return new AbstractListIterator(this, index);
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

	// Synchronized wrapper classes:

	private static class SyncCollection
			implements Collection, Serializable {
		protected final Collection c;

		SyncCollection(Collection c) {
			this.c = c;
		}
		public synchronized int size() {
			return c.size();
		}
		public synchronized boolean isEmpty() {
			return c.isEmpty();
		}
		public synchronized boolean contains(Object o) {
			return c.contains(o);
		}
		public synchronized Iterator iterator() {
			return c.iterator();
		}
		public synchronized Object[] toArray() {
			return c.toArray();
		}
		public synchronized Object[] toArray(Object[] a) {
			return c.toArray(a);
		}
		public synchronized boolean add(Object o) {
			return c.add(o);
		}
		public synchronized boolean remove(Object o) {
			return c.remove(o);
		}
		public synchronized boolean containsAll(Collection c2) {
			return c.containsAll(c2);
		}
		public synchronized boolean addAll(Collection c2) {
			return c.addAll(c2);
		}
		public synchronized boolean removeAll(Collection c2) {
			return c.removeAll(c2);
		}
		public synchronized boolean retainAll(Collection c2) {
			return c.retainAll(c2);
		}
		public synchronized void clear() {
			c.clear();
		}
		public synchronized boolean equals(Object o) {
			return c.equals(o);
		}
		public synchronized int hashCode() {
			return c.hashCode();
		}
	}

	private static class SyncSet extends SyncCollection
			implements Set {
		SyncSet(Set s) {
			super(s);
		}
	}

	private static class SyncSortedSet extends SyncSet
			implements SortedSet {
		SyncSortedSet(SortedSet s) {
			super(s);
		}
		public synchronized Comparator comparator() {
			return ((SortedSet)c).comparator();
		}
		public synchronized SortedSet
		    subSet(Object fromElement, Object toElement) {
			return ((SortedSet)c).subSet(fromElement, toElement);
		}
		public synchronized SortedSet headSet(Object toElement) {
			return ((SortedSet)c).headSet(toElement);
		}
		public synchronized SortedSet tailSet(Object fromElement) {
			return ((SortedSet)c).tailSet(fromElement);
		}
		public synchronized Object first() {
			return ((SortedSet)c).first();
		}
		public synchronized Object last() {
			return ((SortedSet)c).last();
		}
	}

	private static class SyncList extends SyncCollection
			implements List {
		SyncList(List l) {
			super(l);
		}
		public synchronized void add(int index, Object element) {
			((List)c).add(index, element);
		}
		public synchronized boolean addAll(int index, Collection c2) {
			return ((List)c).addAll(index, c2);
		}
		public synchronized Object get(int index) {
			return ((List)c).get(index);
		}
		public synchronized int indexOf(Object o) {
			return ((List)c).indexOf(o);
		}
		public synchronized int lastIndexOf(Object o) {
			return ((List)c).lastIndexOf(o);
		}
		public synchronized ListIterator listIterator() {
			return ((List)c).listIterator();
		}
		public synchronized ListIterator listIterator(int index) {
			return ((List)c).listIterator(index);
		}
		public synchronized Object remove(int index) {
			return ((List)c).remove(index);
		}
		public synchronized Object set(int index, Object element) {
			return ((List)c).set(index, element);
		}
		public synchronized List subList(int fromIndex, int toIndex) {
			return ((List)c).subList(fromIndex, toIndex);
		}
	}

	private static class SyncMap implements Map, Serializable {
		protected final Map m;

		SyncMap(Map m) {
			this.m = m;
		}
		public synchronized int size() {
			return m.size();
		}
		public synchronized boolean isEmpty() {
			return m.isEmpty();
		}
		public synchronized boolean containsKey(Object o) {
			return m.containsKey(o);
		}
		public synchronized boolean containsValue(Object o) {
			return m.containsValue(o);
		}
		public synchronized Object get(Object key) {
			return m.get(key);
		}
		public synchronized Object put(Object key, Object value) {
			return m.put(key, value);
		}
		public synchronized void putAll(Map m2) {
			m.putAll(m2);
		}
		public synchronized void clear() {
			m.clear();
		}
		public synchronized Set keySet() {
			return m.keySet();
		}
		public synchronized Set entrySet() {
			return m.entrySet();
		}
		public synchronized Collection values() {
			return m.values();
		}
		public synchronized boolean equals(Object o) {
			return m.equals(o);
		}
		public synchronized int hashCode() {
			return m.hashCode();
		}
		public synchronized Object remove(Object o) {
			return m.remove(o);
		}
	}

	private static class SyncSortedMap extends SyncMap
			implements SortedMap {
		SyncSortedMap(SortedMap m) {
			super(m);
		}
		public synchronized Comparator comparator() {
			return ((SortedMap)m).comparator();
		}
		public synchronized SortedMap
		    subMap(Object fromKey, Object toKey) {
			return ((SortedMap)m).subMap(fromKey, toKey);
		}
		public synchronized SortedMap headMap(Object toKey) {
			return ((SortedMap)m).headMap(toKey);
		}
		public synchronized SortedMap tailMap(Object fromKey) {
			return ((SortedMap)m).tailMap(fromKey);
		}
		public synchronized Object firstKey() {
			return ((SortedMap)m).firstKey();
		}
		public synchronized Object lastKey() {
			return ((SortedMap)m).lastKey();
		}
	}

	// Unmodifiable wrapper classes

	private static class UMCollection implements Collection, Serializable {
		protected final Collection c;

		UMCollection(Collection c) {
			this.c = c;
		}
		public int size() {
			return c.size();
		}
		public boolean isEmpty() {
			return c.isEmpty();
		}
		public boolean contains(Object o) {
			return c.contains(o);
		}
		public Iterator iterator() {
			return new UMIterator(c.iterator());
		}
		public Object[] toArray() {
			return c.toArray();
		}
		public Object[] toArray(Object[] a) {
			return c.toArray(a);
		}
		public boolean add(Object o) {
			throw new UnsupportedOperationException();
		}
		public boolean remove(Object o) {
			throw new UnsupportedOperationException();
		}
		public boolean containsAll(Collection c2) {
			return c.containsAll(c2);
		}
		public boolean addAll(Collection c2) {
			throw new UnsupportedOperationException();
		}
		public boolean removeAll(Collection c2) {
			throw new UnsupportedOperationException();
		}
		public boolean retainAll(Collection c2) {
			throw new UnsupportedOperationException();
		}
		public void clear() {
			throw new UnsupportedOperationException();
		}
		public boolean equals(Object o) {
			return c.equals(o);
		}
		public int hashCode() {
			return c.hashCode();
		}
	}

	private static class UMSet extends UMCollection
			implements Set {
		UMSet(Set s) {
			super(s);
		}
	}

	private static class UMSortedSet extends UMSet
			implements SortedSet {
		UMSortedSet(SortedSet s) {
			super(s);
		}
		public Comparator comparator() {
			return ((SortedSet)c).comparator();
		}
		public SortedSet subSet(Object fromElement, Object toElement) {
			return new UMSortedSet(
				((SortedSet)c).subSet(fromElement, toElement));
		}
		public SortedSet headSet(Object toElement) {
			return new UMSortedSet(
				((SortedSet)c).headSet(toElement));
		}
		public SortedSet tailSet(Object fromElement) {
			return new UMSortedSet(
				((SortedSet)c).tailSet(fromElement));
		}
		public Object first() {
			return ((SortedSet)c).first();
		}
		public Object last() {
			return ((SortedSet)c).last();
		}
	}

	public static class UMList extends UMCollection
			implements List {
		UMList(List l) {
			super(l);
		}
		public void add(int index, Object element) {
			throw new UnsupportedOperationException();
		}
		public boolean addAll(int index, Collection c2) {
			throw new UnsupportedOperationException();
		}
		public Object get(int index) {
			return ((List)c).get(index);
		}
		public int indexOf(Object o) {
			return ((List)c).indexOf(o);
		}
		public int lastIndexOf(Object o) {
			return ((List)c).lastIndexOf(o);
		}
		public ListIterator listIterator() {
			return new UMListIterator(((List)c).listIterator());
		}
		public ListIterator listIterator(int index) {
			return new UMListIterator(
				((List)c).listIterator(index));
		}
		public Object remove(int index) {
			throw new UnsupportedOperationException();
		}
		public Object set(int index, Object element) {
			throw new UnsupportedOperationException();
		}
		public List subList(int fromIndex, int toIndex) {
			return new UMList(
				((List)c).subList(fromIndex, toIndex));
		}
	}

	private static class UMMap implements Map, Serializable {
		protected final Map m;

		UMMap(Map m) {
			this.m = m;
		}
		public int size() {
			return m.size();
		}
		public boolean isEmpty() {
			return m.isEmpty();
		}
		public boolean containsKey(Object key) {
			return m.containsKey(key);
		}
		public boolean containsValue(Object value) {
			return m.containsValue(value);
		}
		public Object get(Object key) {
			return m.get(key);
		}
		public Object put(Object key, Object value) {
			throw new UnsupportedOperationException();
		}
		public void putAll(Map m2) {
			throw new UnsupportedOperationException();
		}
		public void clear() {
			throw new UnsupportedOperationException();
		}
		public Set keySet() {
			return new UMSet(m.keySet());
		}
		public Set entrySet() {
			return new UMSet(m.entrySet());
		}
		public Collection values() {
			return new UMCollection(m.values());
		}
		public boolean equals(Object o) {
			return m.equals(o);
		}
		public int hashCode() {
			return m.hashCode();
		}
		public Object remove(Object key) {
			throw new UnsupportedOperationException();
		}
	}

	private static class UMSortedMap extends UMMap
			implements SortedMap {
		UMSortedMap(SortedMap m) {
			super(m);
		}
		public Comparator comparator() {
			return ((SortedMap)m).comparator();
		}
		public SortedMap subMap(Object fromKey, Object toKey) {
			return new UMSortedMap(
				((SortedMap)m).subMap(fromKey, toKey));
		}
		public SortedMap headMap(Object toKey) {
			return new UMSortedMap(((SortedMap)m).headMap(toKey));
		}
		public SortedMap tailMap(Object fromKey) {
			return new UMSortedMap(((SortedMap)m).tailMap(fromKey));
		}
		public Object firstKey() {
			return ((SortedMap)m).firstKey();
		}
		public Object lastKey() {
			return ((SortedMap)m).lastKey();
		}
	}

	private static class UMIterator implements Iterator {
		protected final Iterator i;

		UMIterator(Iterator i) {
			this.i = i;
		}
		public boolean hasNext() {
			return i.hasNext();
		}
		public Object next() {
			return i.next();
		}
		public void remove() {
			throw new UnsupportedOperationException();
		}
	}

	private static class UMListIterator extends UMIterator
			implements ListIterator {
		UMListIterator(ListIterator i) {
			super(i);
		}
		public boolean hasPrevious() {
			return ((ListIterator)i).hasPrevious();
		}
		public Object previous() {
			return ((ListIterator)i).previous();
		}
		public int nextIndex() {
			return ((ListIterator)i).nextIndex();
		}
		public int previousIndex() {
			return ((ListIterator)i).previousIndex();
		}
		public void set(Object o) {
			throw new UnsupportedOperationException();
		}
		public void add(Object o) {
			throw new UnsupportedOperationException();
		}
	}
}

