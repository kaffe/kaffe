/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.util;

/**
 * A simple hashtable using +ints as keys.
 */
public class LongHashtable {
  private long keys[];
  private Object elements[];
  private int iloadFactor;
  private int numberOfKeys;
  private int rehashLimit;

  private static final int DEFAULTCAPACITY = 101;
  private static final int DEFAULTLOADFACTOR = 75;
  private static final long removed = -2;
  private static final long free = -1;

  public LongHashtable() {
    this(DEFAULTCAPACITY, DEFAULTLOADFACTOR);
  }
  
  public LongHashtable(int initialCapacity) {
    this(initialCapacity, DEFAULTLOADFACTOR);
  }
  
  public LongHashtable(int initialCapacity, float loadFactor) {
    this(initialCapacity, (int)(loadFactor * 100.0));
  }

  public LongHashtable(int initialCapacity, int iloadFactor) {
    if (initialCapacity <= 0) {
      throw new Error("Initial capacity is <= 0");
    }
    if (iloadFactor <= 0) {
      throw new Error("Load Factor is <= 0");
    }
    this.iloadFactor = iloadFactor;
    this.keys = new long[initialCapacity];
    for (int i = 0; i < initialCapacity; i++) {
      keys[i] = free;
    }
    this.elements = new Object[initialCapacity];
    this.numberOfKeys = 0;
    this.rehashLimit = (iloadFactor * initialCapacity) / 100;
  }
  
  public int size() {
    return (numberOfKeys);
  }
  
  public boolean isEmpty() {
    return (numberOfKeys == 0);
  }
  
  public synchronized boolean contains(Object value) {
    for (int pos = elements.length-1; pos >= 0; pos--) {
      if (value.equals(elements[pos])) {
	  return (true);
      }
    }
    return false;
  }
  
  public synchronized boolean containsKey(long key) {
    return (get(key) != null);
  }
  
  private int calculateBucket(long key) {
    return (((int)key % keys.length) & 0x7FFFFFFF);
  }

  public synchronized Object get(long key)
  {
    int posn = calculateBucket(key);
    int limit = keys.length;
    for (int i = posn; i < limit; i++) {
      long mkey = keys[i];
      if (key == mkey) {
	return (elements[i]);
      }
      if (mkey == free) {
	return (null);
      }
    }
    for (int i = 0; i < posn; i++) {
      long mkey = keys[i];
      if (key == mkey) {
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
    long oldKeys[] = keys;
    Object oldElements[] = elements;

    keys = new long[newCapacity];
    for (int i = 0; i < newCapacity; i++) {
      keys[i] = free;
    }
    elements = new Object[newCapacity];
    rehashLimit = (iloadFactor * newCapacity) / 100;
    numberOfKeys = 0;

    /* Go through adding all the data to the new data */
    for (int pos = oldKeys.length-1; pos >= 0; pos--) {
      if (oldKeys[pos] != free && oldKeys[pos] != removed) {
	put(oldKeys[pos], oldElements[pos]);
      }
    }
  }
  
  public synchronized Object put(long key, Object value) {
    if (numberOfKeys >= rehashLimit) {
      rehash();
    }

    int posn = calculateBucket(key);
    int limit = keys.length;
    int space = -1;
    for (int i = posn; i < limit; i++) {
      long mkey = keys[i];
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
      else if (key == mkey) {
	Object oldElement = elements[i];
	elements[i] = value;
	return (oldElement);
      }
    }
    for (int i = 0; i < posn; i++) {
      long mkey = keys[i];
      if (key == mkey) {
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
  
  public synchronized Object remove(long key) {

    int posn = calculateBucket(key);
    int limit = keys.length;
    for (int i = posn; i < limit; i++) {
      long mkey = keys[i];
      if (key == mkey) {
	Object oldElement = elements[i];
	elements[i] = null;
	keys[i] = removed;
	numberOfKeys--;
	return (oldElement);
      }
      if (mkey == free) {
	return (null);
      }
    }
    for (int i = 0; i < posn; i++) {
      long mkey = keys[i];
      if (key == mkey) {
	Object oldElement = elements[i];
	elements[i] = null;
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
      elements[pos] = null;
    }
    numberOfKeys = 0;
  }
  
  public synchronized String toString() {
    StringBuffer result = new StringBuffer();

    result.append('{');
    int pos = 0;
    for (; pos < keys.length; pos++) {
      if (keys[pos] != free && keys[pos] != removed) {
	result.append(keys[pos]);
	result.append("=");
	result.append(elements[pos]);
	break;
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
}
