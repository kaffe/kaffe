
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

public class Collections {
	public static final Set EMPTY_SET = new AbstractSet() {
		public int size() {
			return 0;
		}
		public Iterator iterator() {
			return new EmptyIterator();
		}
	};
	public static final List EMPTY_LIST = new AbstractList() {
		public int size() {
			return 0;
		}
		public Object get(int index) {
			throw new IndexOutOfBoundsException();
		}
	};

	// An empty iterator
	private static class EmptyIterator implements Iterator {
		public boolean hasNext() {
			return false;
		}
		public Object next() {
			throw new NoSuchElementException();
		}
		public void remove() {
			throw new IllegalStateException();
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
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".unmodifiableCollection()");
	}

	public static Set unmodifiableSet(Set s) {
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".unmodifiableSet()");
	}

	public static SortedSet unmodifiableSortedSet(SortedSet s) {
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".unmodifiableSortedSet()");
	}

	public static List unmodifiableList(List list) {
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".unmodifiableList()");
	}

	public static Map unmodifiableMap(Map m) {
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".unmodifiableMap()");
	}

	public static SortedMap unmodifiableSortedMap(SortedMap m) {
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".unmodifiableSortedMap()");
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
				return new Iterator() {
					private int index = 0;
					public boolean hasNext() {
						return index < 1;
					}
					public Object next() {
						if (index == 1) {
							throw new NoSuchElementException();
						}
						index++;
						return o;
					}
					public void remove() {
						throw new UnsupportedOperationException();
					}
				};
			}
		};
	}

	public static List nCopies(int n, Object o) {
	  throw new kaffe.util.NotImplemented(Collections.class.getName()
		+ ".nCopies()");
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
}

