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
  return (true);
}

public abstract Object get(int index);

public Object set(int index, Object element) {
  throw new UnsupportedOperationException();
}

public void add(int index, Object element) {
  throw new UnsupportedOperationException();
}

public boolean remove(Object o) {
  throw new UnsupportedOperationException();
}

public Object remove(int index) {
  throw new UnsupportedOperationException();
}

public int indexOf(Object o) {
  ListIterator it = listIterator();
  int idx = 0;
  while (it.hasNext()) {
    if (it.next() == o) {
      return (idx);
    }
    idx++;
  }
  return (-1);
}

public int lastIndexOf(Object o) {
  ListIterator it = listIterator(size());
  int idx = size() - 1;
  while (it.hasPrevious()) {
    if (it.previous() == o) {
      return (idx);
    }
    idx--;
  }
  return (-1);
}

public void clear() {
    removeRange(0, size());
}

public boolean addAll(int index, Collection c) {
  Object[] objs = c.toArray();
  for (int i = 0; i < objs.length; i++) {
    add(index + i, objs[i]);
  }
  return (true);
}

public Iterator iterator() {
  throw new kaffe.util.NotImplemented();
}

public ListIterator listIterator() {
  return (listIterator(0));
}

public ListIterator listIterator(int index) {
  throw new kaffe.util.NotImplemented();
}

public List subList(int fromIndex, int toIndex) {
  throw new kaffe.util.NotImplemented();
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
  throw new kaffe.util.NotImplemented();
}
 
}
