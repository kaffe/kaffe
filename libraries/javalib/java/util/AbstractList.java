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

public abstract class AbstractList
  extends AbstractCollection implements List {

protected int modCount;

protected AbstractList() {
}
    
public boolean add(Object o) {
  add(size(), o);
  return true;
}

public abstract Object get(int index);

public Object set(int index, Object element) {
  throw new UnsupportedOperationException();
}

public void add(int index, Object element) {
  throw new UnsupportedOperationException();
}

public Object remove(int index) {
  throw new UnsupportedOperationException();
}

public int indexOf(Object o) {
  ListIterator it = listIterator();
  for (int idx = 0; it.hasNext(); idx++) {
    Object next = it.next();
    if (o == null ? next == null : o.equals(next)) {
      return idx;
    }
  }
  return -1;
}

public int lastIndexOf(Object o) {
  ListIterator it = listIterator(size());
  for (int idx = size() - 1; it.hasPrevious(); idx--) {
    Object prev = it.previous();
    if (o == null ? prev == null : o.equals(prev)) {
      return (idx);
    }
  }
  return (-1);
}

public void clear() {
    removeRange(0, size());
}

public boolean addAll(int index, Collection c) {
  for (Iterator i = c.iterator(); i.hasNext(); ) {
    add(index++, i.next());
  }
  return c.size() != 0;
}

// This class is used below by iterator() and listIterator()
private static class ALI implements ListIterator  {
  private final AbstractList list;
  private int modCount;
  private int lastIndex = -1;
  private int index;

  ALI(AbstractList list, int index) {
    if (index < 0 || index > list.size()) {
      throw new IllegalArgumentException();
    }
    this.list = list;
    this.index = index;
    this.modCount = list.modCount;
  }

  public int nextIndex() {
    return index;
  }

  public int previousIndex() {
    return index - 1;
  }

  public boolean hasNext() {
    return index < list.size();
  }

  public Object next() {
    if (list.modCount != this.modCount) {
      throw new ConcurrentModificationException();
    }
    if (index >= list.size()) {
      throw new NoSuchElementException();
    }
    Object rtn = list.get(index);
    lastIndex = index++;
    return rtn;
  }

  public boolean hasPrevious() {
    return index > 0;
  }

  public Object previous() {
    if (list.modCount != this.modCount) {
      throw new ConcurrentModificationException();
    }
    if (index == 0) {
      throw new NoSuchElementException();
    }
    Object rtn = list.get(index - 1);
    lastIndex = --index;
    return rtn;
  }

  public void remove() {
    if (list.modCount != this.modCount) {
      throw new ConcurrentModificationException();
    }
    if (lastIndex == -1) {
      throw new IllegalStateException();
    }
    list.remove(lastIndex);
    modCount = list.modCount;
    if (lastIndex < index) {
      index--;
    }
    lastIndex = -1;
  }

  public void set(Object o) {
    if (list.modCount != this.modCount) {
      throw new ConcurrentModificationException();
    }
    if (lastIndex == -1) {
      throw new IllegalStateException();
    }
    list.set(lastIndex, o);
  }

  public void add(Object o) {
    if (list.modCount != this.modCount) {
      throw new ConcurrentModificationException();
    }
    if (lastIndex == -1) {
      throw new IllegalStateException();
    }
    list.add(index, o);
    modCount = list.modCount;
    index++;
    lastIndex = -1;
  }
}

public Iterator iterator() {
  return listIterator(0);
}

public ListIterator listIterator() {
  return listIterator(0);
}

public ListIterator listIterator(int index) {
  return new ALI(this, index);
}

public List subList(final int fromIndex, final int toIndex) {
  if (fromIndex < 0 || toIndex > size()) {
    throw new IndexOutOfBoundsException();
  }
  if (fromIndex > toIndex) {
    throw new IllegalArgumentException();
  }
  return new AbstractList() {
    private final AbstractList list = AbstractList.this;
    private int modCount = AbstractList.this.modCount;
    private final int off = fromIndex;
    private int len = toIndex - fromIndex;

    public int size() {
      return len;
    }

    public Object get(int index) {
      if (index < 0 || index >= len) {
	throw new IndexOutOfBoundsException();
      }
      return list.get(off + index);
    }

    public Object set(int index, Object element) {
      if (list.modCount != this.modCount) {
	throw new ConcurrentModificationException();
      }
      if (index < 0 || index >= len) {
	throw new IndexOutOfBoundsException();
      }
      return list.set(off + index, element);
    }

    public void add(int index, Object element) {
      if (list.modCount != this.modCount) {
	throw new ConcurrentModificationException();
      }
      if (index < 0 || index > len) {
	throw new IndexOutOfBoundsException();
      }
      list.add(off + index, element);
      modCount = AbstractList.this.modCount;
      len++;
    }

    public Object remove(int index) {
      if (list.modCount != this.modCount) {
	throw new ConcurrentModificationException();
      }
      if (index < 0 || index >= len) {
	throw new IndexOutOfBoundsException();
      }
      Object rtn = list.remove(off + index);
      modCount = AbstractList.this.modCount;
      len--;
      return rtn;
    }

    public int indexOf(Object o) {
      if (list.modCount != this.modCount) {
	throw new ConcurrentModificationException();
      }
      final ListIterator it = list.listIterator(off);
      for (int index = 0; index < len && it.hasNext(); index++) {
	Object next = it.next();
	if (o == null ? next == null : o.equals(next)) {
	  return index;
	}
      }
      return -1;
    }

    public int lastIndexOf(Object o) {
      if (list.modCount != this.modCount) {
	throw new ConcurrentModificationException();
      }
      final ListIterator it = listIterator(off + len);
      for (int index = len - 1; it.hasPrevious(); index--) {
	Object prev = it.previous();
	if (o == null ? prev == null : o.equals(prev)) {
	  return index;
	}
      }
      return -1;
    }

    protected void removeRange(int fromIndex0, int toIndex0) {
      if (list.modCount != this.modCount) {
	throw new ConcurrentModificationException();
      }
      if (fromIndex0 < 0 || toIndex0 > len) {
	throw new IndexOutOfBoundsException();
      }
      list.removeRange(off + fromIndex0, off + toIndex0);
      modCount = AbstractList.this.modCount;
      len -= toIndex0 - fromIndex0;
    }
  };
}

public boolean equals(Object o) {
  if (o == this) {
    return (true);
  }
  if (!(o instanceof List)) {
    return (false);
  }
  List other = (List)o;
  if (size() != other.size()) {
    return (false);
  }
  ListIterator mlist = listIterator();
  ListIterator olist = other.listIterator();
  while (mlist.hasNext()) {
    final Object o1 = mlist.next();
    final Object o2 = olist.next();
    if (!(o1 == null ? o2 == null : o1.equals(o2))) {
      return (false);
    }
  }
  return (true);
}

public int hashCode() {
  int hashCode = 1;
  for (Iterator i = iterator(); i.hasNext(); ) {
    final Object obj = i.next();
    hashCode = 31*hashCode + (obj==null ? 0 : obj.hashCode());
  }
  return hashCode;
}

protected void removeRange(int fromIndex, int toIndex) {
  final ListIterator i = listIterator(fromIndex);
  while (fromIndex < toIndex && i.hasNext()) {
    i.remove();
    fromIndex++;
  }
}
 
}
