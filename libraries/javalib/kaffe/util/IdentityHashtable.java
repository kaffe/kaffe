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


/* IdentityHashtable is a simplified java.util.Hashtable for use by
   java.io.ObjectOutputStream. It uses System.identityHashCode and
   == instead of hashCode() and equals(). */

public class IdentityHashtable {
  transient private Object keys[];
  transient private Object elements[];
  transient private float loadFactor;
  private int numberOfKeys;
  transient private int rehashLimit;

  private static final int DEFAULTCAPACITY = 101;
  private static final float DEFAULTLOADFACTOR = (float)0.75;
  private static final Object removed = new Object();
  private static final Object free = null;

  public IdentityHashtable() {
    this(DEFAULTCAPACITY, DEFAULTLOADFACTOR);
  }
  
  public IdentityHashtable(int initialCapacity) {
    this(initialCapacity, DEFAULTLOADFACTOR);
  }
  
  public IdentityHashtable(int initialCapacity, float loadFactor)
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
    return ((System.identityHashCode(key) & Integer.MAX_VALUE) % keys.length);
  }

  public synchronized Object get(Object key)
  {
    int posn = calculateBucket(key);
    int limit = keys.length;
    for (int i = posn; i < limit; i++) {
      Object mkey = keys[i];
      if (key==mkey) {
       return (elements[i]);
      }
      if (mkey == free) {
       return (null);
      }
    }
    for (int i = 0; i < posn; i++) {
      Object mkey = keys[i];
      if (key==mkey) {
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
      if (key==mkey) {
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
      if (key==mkey) {
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
    throw new Error("Inconsistent IdentityHashtable");
  }
  
  public synchronized Object remove(Object key) {

    int posn = calculateBucket(key);
    int limit = keys.length;
    for (int i = posn; i < limit; i++) {
      Object mkey = keys[i];
      if (key==mkey) {
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
      if (key==mkey) {
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
}
