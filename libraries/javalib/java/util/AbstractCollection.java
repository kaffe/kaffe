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

import java.lang.reflect.Array;

public abstract class AbstractCollection implements Collection {

protected AbstractCollection() {
}

public abstract Iterator iterator();

public abstract int size();

public boolean isEmpty() {
  return (size() == 0);
}

public boolean contains(Object o) {
  for (Iterator it = iterator(); it.hasNext(); ) {
    Object next = it.next();
    if (o == null ? next == null : o.equals(next)) {
      return true;
    }
  }
  return false;
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
  final int len = size();
  if (a.length < len) {
    a = (Object[])Array.newInstance(a.getClass().getComponentType(), len);
  }
  Iterator i = iterator();
  for (int index = 0; i.hasNext(); ) {
    a[index] = i.next();
  }
  if (a.length > len) {
    a[len] = null;
  }
  return a;
}

public boolean add(Object o) {
  throw new UnsupportedOperationException();
}

public boolean remove(Object o) {
  for (Iterator it = iterator(); it.hasNext(); ) {
    Object next = it.next();
    if (o == null ? next == null : o.equals(next)) {
      it.remove();
      return true;
    }
  }
  return false;
}

public boolean containsAll(Collection c) {
  for (Iterator it = c.iterator(); it.hasNext(); ) {
    if (!contains(it.next())) {
      return false;
    }
  }
  return true;
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
