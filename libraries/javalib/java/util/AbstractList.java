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
  int idx = 0;
  while (it.hasPrevious()) {
    if (it.previous() == o) {
      return (idx);
    }
    idx++;
  }
  return (-1);
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
    if (mlist.next() != olist.next()) {
      return (false);
    }
  }
  return (true);
}

public int hashCode() {
  return (super.hashCode());
}

}
