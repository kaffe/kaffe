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

public abstract class AbstractCollection implements Collection {

public AbstractCollection() {
}

public abstract Iterator iterator();

public abstract int size();

public boolean isEmpty() {
  return (size() == 0);
}

public boolean contains(Object o) {
  Iterator it = iterator();
  while (it.hasNext()) {
    if (it.next() == o) {
      return (true);
    }
  }
  return (false);
}

public Object[] toArray() {
  Object[] objs = new Object[size()];
  Iterator it = iterator();
  for (int i = 0; i < objs.length; i++) {
    objs[i] = it.next();
  }
  return (objs);
}

public Object[] toArray(Object[] a) {
  throw new kaffe.util.NotImplemented();
}

public boolean add(Object o) {
  throw new UnsupportedOperationException();
}

public boolean remove(Object o) {
  Iterator it = iterator();
  while (it.hasNext()) {
    if (it.next() == o) {
      it.remove();
      return (true);
    }
  }
  return (false);
}

public boolean containsAll(Collection c) {
  Iterator it = c.iterator();
  while (it.hasNext()) {
    if (!contains(it.next())) {
      return (false);
    }
  }
  return (true);
}

public boolean addAll(Collection c) {
  Iterator it = c.iterator();
  while (it.hasNext()) {
    add(it.next());
  }
  return (true);
}

public boolean removeAll(Collection c) {
  Iterator it = c.iterator();
  boolean status = false;
  while (it.hasNext()) {
    if (remove(it.next())) {
      status = true;
    }
  }
  return (status);
}

public boolean retainAll(Collection c) {
  Iterator it = iterator();
  boolean status = false;
  while (it.hasNext()) {
    if (!c.contains(it.next())) {
      it.remove();
      status = true;
    }
  }
  return (status);
}

public void clear() {
  Iterator it = iterator();
  while (it.hasNext()) {
    it.remove();
  }
}

public String toString() {
  StringBuffer buf = new StringBuffer();
  Iterator it = iterator();
  buf.append("[");
  while (it.hasNext()) {
    Object o = it.next();
    buf.append(String.valueOf(o));
    if (it.hasNext()) {
      buf.append(", ");
    }
  }
  buf.append("]");

  return (buf.toString());
}

}
