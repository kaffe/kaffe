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

/* Hashtable (NOT tree) with simple clustering */

public class Hashtable extends Dictionary implements Cloneable {
  private HashtableEntry bucket[];
  private float loadFactor;
  private int numberOfKeys;

  private static final int DEFAULTCAPACITY=16;

  public Hashtable(int initialCapacity, float loadFactor)
  {
    // We must always have at least one bucket.
    if (initialCapacity <= 0) {
      initialCapacity = 1;
    }
    this.loadFactor = loadFactor;
    this.bucket = new HashtableEntry[initialCapacity];
    this.numberOfKeys = 0;
    clear();
  }
  
  public Hashtable(int initialCapacity) {
    this(initialCapacity, (float)1.0);
  }
  
  public Hashtable() {
    this(DEFAULTCAPACITY, (float)1.0);
  }
  
  public int size() {
    return numberOfKeys;
  }
  
  public boolean isEmpty() {
    return (numberOfKeys==0);
  }
  
  public synchronized Enumeration keys() {
    Vector vector=new Vector();

    for (int pos=0; pos < bucket.length; pos++) {
      for (HashtableEntry ptr = bucket[pos]; ptr != null; ptr = ptr.next) {
	vector.addElement(ptr.getKey());
      }
    }

    return new HashtableEnumeration(vector);
  }
  
  public synchronized Enumeration elements() {
    Vector vector=new Vector();

    for (int pos=0; pos < bucket.length; pos++) {
      for (HashtableEntry ptr = bucket[pos]; ptr != null; ptr = ptr.next) {
	vector.addElement(ptr.getData());
      }
    }

    return new HashtableEnumeration(vector);
  }
  
  public synchronized boolean contains(Object value) {
    /* No key therefore linear */
    for (int pos=0; pos < bucket.length; pos++) {
      for (HashtableEntry ptr = bucket[pos]; ptr != null; ptr = ptr.next) {
        if (value.equals(ptr.getData())) return true;
      }
    }
    return false;
  }
  
  public synchronized boolean containsKey(Object key) {
    /* Can lookup with the key */
    return (get(key)!=null);
  }
  
  public synchronized Object get(Object key)
  {
    int posn = calculateBucket(key);
    for (HashtableEntry ptr = bucket[posn]; ptr != null; ptr = ptr.next) {
      if (key.equals(ptr.getKey())) {
	return ptr.getData();
      }
    }
    return null;
  }
  
  private int calculateBucket(Object key)
  {
    int hash = key.hashCode() % bucket.length;
    if (hash < 0) {
      return (-hash);
    }
    else {
      return (hash);
    }
  }

  protected synchronized void rehash()
  {
    int newCapacity = bucket.length * 2; /* double size, why not? */
    HashtableEntry oldData[] = bucket;

    bucket = new HashtableEntry[newCapacity];
    numberOfKeys=0;

    /* Go through adding all the data to the new data */
    for (int pos=0; pos<oldData.length; pos++) {
      for (HashtableEntry ptr = oldData[pos]; ptr != null; ptr = ptr.next) {
        put(ptr.getKey(), ptr.getData());
      }
    }
  }
  
  public synchronized Object put(Object key, Object value)
  {
    if (loadFactor == 0.0) {
      throw new Error("Load Factor is 0.0");
    }

    if (size() > ((int)(loadFactor*(float)bucket.length))) {
      rehash();
    }

    int posn = calculateBucket(key);
    for (HashtableEntry ptr = bucket[posn]; ptr != null; ptr = ptr.next) {
      if (key.equals(ptr.getKey())) {
	Object oldData = ptr.getData();
	ptr.data = value;
	return oldData;
      }
    }

    HashtableEntry entry = new HashtableEntry(key, value);
    entry.next = bucket[posn];
    bucket[posn] = entry;
    numberOfKeys++;
    return null;
  }
  
  public synchronized Object remove(Object key) {

    int posn = calculateBucket(key);
    HashtableEntry ptr = bucket[posn];
    if (ptr == null) {
      return null;
    }
    if (key.equals(ptr.getKey())) {
      numberOfKeys--;
      bucket[posn] = ptr.next;
      return ptr.getData();
    }
    for (; ptr.next != null; ptr = ptr.next) {
      if (key.equals(ptr.next.getKey())) {
	numberOfKeys--;
	HashtableEntry entry = ptr.next;
	ptr.next = entry.next;
	return entry.getData();
      }
    }
    return null;
  }
  
  public synchronized void clear() {
    numberOfKeys=0;
    for (int pos=0; pos<bucket.length; pos++) {
      bucket[pos]=null;
    }
  }
  
  /**
   * Creates a shallow copy of this hashtable.  
   * The keys and values themselves are not cloned. 
   * This is a relatively expensive operation. 
   *
   * @return a clone of the hashtable. 
   */
  public synchronized Object clone() {
    Hashtable result = null;
    try {
      /* Note that we must use super.clone() here instead of a
       * constructor or else subclasses such as java.util.Properties
       * will not be cloned properly.
       */
      result = (Hashtable)super.clone();
      result.numberOfKeys = 0;
      result.loadFactor = loadFactor;
      result.bucket = new HashtableEntry[bucket.length];

      /* copy our entries in new hashtable */ 
      for (int pos=0; pos<bucket.length; pos++) {
        for (HashtableEntry ptr = bucket[pos]; ptr != null; ptr = ptr.next) {
	  result.put(ptr.getKey(), ptr.getData());
        }
      }
    } catch (CloneNotSupportedException _) { }
    return (Object)result;
  }
  
  public synchronized String toString() {
    boolean firstTime=true;
    StringBuffer result=new StringBuffer();

    result.append('{');
    for (int pos=0; pos<bucket.length; pos++) {
      for (HashtableEntry ptr = bucket[pos]; ptr != null; ptr = ptr.next) {
	if (!firstTime) result.append(", "); else firstTime=false;
	result=result.append(ptr.getKey() + "=" + ptr.getData());
      }
    }
    result.append('}');

    return result.toString();
  }
}

class HashtableEntry {
  private Object key;
  protected Object data;
  private boolean spilled;
  protected HashtableEntry next;
  
  public HashtableEntry() {
    this(null, null);
  }

  public HashtableEntry(Object key, Object data) {
    this.key=key;
    this.data=data;
  }

  protected Object clone() {
    return new HashtableEntry(key, data);
  }

  public Object getKey() {
    return key;
  }
  
  public Object getData() {
    return data;
  }

  public boolean isSpilled() {
    return spilled;
  }

  public void setSpilled() {
    spilled=true;
  }
}

class HashtableEnumeration implements Enumeration
{
  private Vector vector;
  private int posn = 0;

  public HashtableEnumeration(Vector vector) {
    this.vector = vector;
  }

  public boolean hasMoreElements()
  {
    return (posn < vector.size());
  }

  public Object nextElement()
  {
    if (posn >= vector.size()) {
      throw new NoSuchElementException();
    }
    return (vector.elementAt(posn++));
  }
}
