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

import java.lang.String;
import java.io.Serializable;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.lang.ClassNotFoundException;

/* Hashtable (NOT tree) with simple clustering */

public class Hashtable extends Dictionary implements Map, Cloneable, Serializable {
  private Object keys[];
  private Object elements[];
  private float loadFactor;
  private int numberOfKeys;
  private int rehashLimit;

  private static final int DEFAULTCAPACITY = 101;
  private static final float DEFAULTLOADFACTOR = (float)0.75;
  private static final Object removed = new Object();
  private static final Object free = null;

  /* This is what Sun's JDK1.1 "serialver java.util.Hashtable" spits out */
  private static final long serialVersionUID = 1421746759512286392L;

  public Hashtable() {
    this(DEFAULTCAPACITY, DEFAULTLOADFACTOR);
  }
  
  public Hashtable(int initialCapacity) {
    this(initialCapacity, DEFAULTLOADFACTOR);
  }
  
  public Hashtable(int initialCapacity, float loadFactor)
  {
    if (initialCapacity <= 0) {
      throw new Error("Initial capacity is <= 0");
    }
    if (loadFactor <= 0.0) {
      throw new Error("Load Factor is <= 0");
    }
    this.loadFactor = loadFactor;
    this.keys = new Object[initialCapacity];
    this.elements = new Object[initialCapacity];
    this.numberOfKeys = 0;
    this.rehashLimit = (int)(loadFactor * (float)initialCapacity);
  }
  
  public int size() {
    return (numberOfKeys);
  }
  
  public boolean isEmpty() {
    return (numberOfKeys == 0);
  }
  
  public synchronized Enumeration keys() {
    Vector vector = new Vector(numberOfKeys);

    for (int pos = keys.length-1; pos >= 0; pos--) {
      if (keys[pos] != free && keys[pos] != removed) {
	vector.addElement(keys[pos]);
      }
    }

    return (new HashtableEnumeration(vector));
  }


  public synchronized Enumeration elements() {
    Vector vector = new Vector(numberOfKeys);

    for (int pos = elements.length-1; pos >= 0; pos--) {
      if (keys[pos] != free && keys[pos] != removed) {
	vector.addElement(elements[pos]);
      }
    }

    return (new HashtableEnumeration(vector));
  }
  
  public synchronized boolean contains(Object value) {
    for (int pos = elements.length-1; pos >= 0; pos--) {
      if (value.equals(elements[pos])) {
	  return (true);
      }
    }
    return false;
  }
  
  public synchronized boolean containsKey(Object key) {
    return (get(key) != null);
  }
  
  private int calculateBucket(Object key) {
    return ((key.hashCode() & Integer.MAX_VALUE) % keys.length);
  }

  public synchronized Object get(Object key)
  {
    int posn = calculateBucket(key);
    int limit = keys.length;
    for (int i = posn; i < limit; i++) {
      Object mkey = keys[i];
      if (key.equals(mkey)) {
	return (elements[i]);
      }
      if (mkey == free) {
	return (null);
      }
    }
    for (int i = 0; i < posn; i++) {
      Object mkey = keys[i];
      if (key.equals(mkey)) {
	return (elements[i]);
      }
      if (mkey == free) {
	return (null);
      }
    }
    return (null);
  }
  
  protected synchronized void rehash()
  {
    int newCapacity = keys.length * 2;
    Object oldKeys[] = keys;
    Object oldElements[] = elements;

    keys = new Object[newCapacity];
    elements = new Object[newCapacity];
    rehashLimit = (int)(loadFactor * (float)newCapacity);
    numberOfKeys = 0;

    /* Go through adding all the data to the new data */
    for (int pos = oldKeys.length-1; pos >= 0; pos--) {
      if (oldKeys[pos] != free && oldKeys[pos] != removed) {
	put(oldKeys[pos], oldElements[pos]);
      }
    }
  }
  
  public synchronized Object put(Object key, Object value) {
    if (numberOfKeys >= rehashLimit) {
      rehash();
    }

    int posn = calculateBucket(key);
    int limit = keys.length;
    int space = -1;
    for (int i = posn; i < limit; i++) {
      Object mkey = keys[i];
      if (key.equals(mkey)) {
	Object oldElement = elements[i];
	elements[i] = value;
	return (oldElement);
      }
      if (mkey == removed) {
	if (space == -1) {
	  space = i;
	}
      }
      else if (mkey == free) {
	if (space == -1) {
	  space = i;
	}
	keys[space] = key;
	elements[space] = value;
	numberOfKeys++;
	return (null);
      }
    }
    for (int i = 0; i < posn; i++) {
      Object mkey = keys[i];
      if (key.equals(mkey)) {
	Object oldElement = elements[i];
	elements[i] = value;
	return (oldElement);
      }
      if (mkey == removed) {
	if (space == -1) {
	  space = i;
	}
      }
      else if (mkey == free) {
	if (space == -1) {
	  space = i;
	}
	keys[space] = key;
	elements[space] = value;
	numberOfKeys++;
	return (null);
      }
    }
    if (space != -1) {
      keys[space] = key;
      elements[space] = value;
      numberOfKeys++;
      return (null);
    }
    // We shouldn't get here.
    throw new Error("Inconsistent Hashtable");
  }
  
  public synchronized Object remove(Object key) {

    int posn = calculateBucket(key);
    int limit = keys.length;
    for (int i = posn; i < limit; i++) {
      Object mkey = keys[i];
      if (key.equals(mkey)) {
	Object oldElement = elements[i];
	elements[i] = removed;
	keys[i] = removed;
	numberOfKeys--;
	return (oldElement);
      }
      if (mkey == free) {
	return (null);
      }
    }
    for (int i = 0; i < posn; i++) {
      Object mkey = keys[i];
      if (key.equals(mkey)) {
	Object oldElement = elements[i];
	elements[i] = removed;
	keys[i] = removed;
	numberOfKeys--;
	return (oldElement);
      }
      if (mkey == free) {
	return (null);
      }
    }
    return (null);
  }
  
  public synchronized void clear() {
    for (int pos = keys.length - 1; pos >= 0; pos--) {
      keys[pos] = free;
      elements[pos] = free;
    }
    numberOfKeys = 0;
  }
  
  /**
   * Creates a shallow copy of this hashtable.  
   * The keys and values themselves are not cloned. 
   * This is a relatively expensive operation. 
   *
   * @return a clone of the hashtable. 
   */
  public synchronized Object clone() {
    Hashtable result;
    try {
      /* Note that we must use super.clone() here instead of a
       * constructor or else subclasses such as java.util.Properties
       * will not be cloned properly.
       */
      result = (Hashtable)super.clone();
    }
    catch (CloneNotSupportedException _) {
      return (null);
    }
    result.numberOfKeys = 0;
    result.loadFactor = loadFactor;
    result.keys = new Object[keys.length];
    result.elements = new Object[elements.length];

    /* copy our entries in new hashtable */ 
    for (int pos = keys.length-1; pos >= 0; pos--) {
      if (keys[pos] != free && keys[pos] != removed) {
	result.put(keys[pos], elements[pos]);
      }
    }
    return ((Object)result);
  }
  
  /**
   * read this hashtable from a stream
   */
  private void readObject(ObjectInputStream stream) throws IOException, ClassNotFoundException {
    // read all default fields
    stream.defaultReadObject();

    // create buckets
    int capacity = stream.readInt();
    int size = stream.readInt();

    // We ignore the capacity
    capacity = (int)(rehashLimit / loadFactor);

    keys = new Object[capacity];
    elements = new Object[capacity];

    // clear table, but remember how many entries are in the stream
    numberOfKeys = 0;

    // read entries
    for (int i = 0; i < size; i++) {
      Object k = stream.readObject();
      Object o = stream.readObject();
      put(k, o);
    }
  }

  /**
   * write this hashtable into a stream
   */
  private void writeObject(java.io.ObjectOutputStream stream) throws IOException {
    // write all default fields
    stream.defaultWriteObject();

    // remember how many buckets there were
    stream.writeInt(keys.length);
    stream.writeInt(numberOfKeys);

    for (int pos = 0; pos < keys.length; pos++) {
      if (keys[pos] != free && keys[pos] != removed) {
	stream.writeObject(keys[pos]);
	stream.writeObject(elements[pos]);
      }
    }
  }

  public synchronized String toString() {
    boolean firstTime = true;
    StringBuffer result = new StringBuffer();

    result.append('{');
    int pos = 0;
    for (; pos < keys.length; pos++) {
      if (keys[pos] != free && keys[pos] != removed) {
	result.append(keys[pos]);
	result.append("=");
	result.append(elements[pos]);
      }
    }
    for (; pos < keys.length; pos++) {
      if (keys[pos] != free && keys[pos] != removed) {
	result.append(", ");
	result.append(keys[pos]);
	result.append("=");
	result.append(elements[pos]);
      }
    }
    result.append('}');

    return (result.toString());
  }

  public boolean containsValue(Object o) {
    return (contains(o));
  }

  public void putAll(Map m) {
    Object[] keys = m.keySet().toArray();
    for (int i = 0; i < keys.length; i++) {
      put(keys[i], m.get(keys[i]));
    }
  }

  public synchronized Set entrySet() {
    final Vector kvec = new Vector(numberOfKeys);
    final Vector vals = new Vector(numberOfKeys);

    // Snapshot key list and corresponding value list
    for (int pos = keys.length-1; pos >= 0; pos--) {
      if (keys[pos] != free && keys[pos] != removed) {
	kvec.addElement(keys[pos]);
	vals.addElement(get(keys[pos]));
      }
    }

    // Return unmodifiable Set view (XXX should be modifyable)
    return new AbstractSet() {
      public int size() {
	return kvec.size();
      }
      public Iterator iterator() {
	return new Iterator() {
	  private int index = 0;
	  public boolean hasNext() {
	    return index < kvec.size();
	  }
	  public Object next() {
	    final Object key = kvec.elementAt(index);
	    final Object val = vals.elementAt(index);
	    index++;
	    return new AbstractMapEntry(key, val) {
	      public void changeValue(Object newValue) {
		throw new UnsupportedOperationException();
	      }
	    };
	  }
	  public void remove() {
	    throw new UnsupportedOperationException();
	  }
	};
      }
    };
  }

  public Set keySet() {
    final Vector kvec = new Vector(numberOfKeys);

    // Snapshot key list
    for (int pos = keys.length-1; pos >= 0; pos--) {
      if (keys[pos] != free && keys[pos] != removed) {
	kvec.addElement(keys[pos]);
      }
    }

    // Return unmodifiable Set view (XXX should be modifyable)
    return new AbstractSet() {
      private int index;
      public int size() {
	return kvec.size();
      }
      public Iterator iterator() {
	return new Iterator() {
	  private int index = 0;
	  public boolean hasNext() {
	    return index < kvec.size();
	  }
	  public Object next() {
	    return kvec.elementAt(index++);
	  }
	  public void remove() {
	    throw new UnsupportedOperationException();
	  }
	};
      }
    };
  }

  public Collection values() {
    final Vector vals = new Vector(numberOfKeys);

    // Snapshot key list
    for (int pos = keys.length-1; pos >= 0; pos--) {
      if (keys[pos] != free && keys[pos] != removed) {
	vals.addElement(get(keys[pos]));
      }
    }

    // Return unmodifiable Collection view (XXX should be modifyable)
    return new AbstractCollection() {
      public int size() {
	return vals.size();
      }
      public Iterator iterator() {
	return new Iterator() {
	  private int index = 0;
	  public boolean hasNext() {
	    return index < vals.size();
	  }
	  public Object next() {
	    return vals.elementAt(index++);
	  }
	  public void remove() {
	    throw new UnsupportedOperationException();
	  }
	};
      }
    };
  }

  /*
   * JDK compatible serialization
   */
  class DefaultSerialization {
  
  private float loadFactor;
  private int threshold;
  
  private void readDefaultObject() {
          Hashtable.this.loadFactor = loadFactor;
          Hashtable.this.rehashLimit = threshold;
  }
  
  private void writeDefaultObject() {
          loadFactor = Hashtable.this.loadFactor;
          threshold = Hashtable.this.rehashLimit;
  }

  }

}

class HashtableEnumeration
  implements Enumeration
{
  private Vector vector;
  private int posn = 0;

  public HashtableEnumeration(Vector vector) {
    this.vector = vector;
  }

  public boolean hasMoreElements() {
    return (posn < vector.size());
  }

  public Object nextElement() {
    if (posn >= vector.size()) {
      throw new NoSuchElementException();
    }
    return (vector.elementAt(posn++));
  }
}
