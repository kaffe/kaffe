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

import java.io.Serializable;
import java.io.ObjectStreamField;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

public class Hashtable extends Dictionary
		implements Map, Cloneable, Serializable {

	private static final long serialVersionUID = 1421746759512286392L;
	private static final ObjectStreamField[] serialPersistentFields = 
	{
		new ObjectStreamField("threshold", int.class),
		new ObjectStreamField("loadFactor", float.class),
	};

	private static final int DEFAULT_CAPACITY = 11;
	private static final float DEFAULT_LOADFACTOR = 0.75f;
	private transient HashMap map;

	public Hashtable() {
		this(DEFAULT_CAPACITY, DEFAULT_LOADFACTOR);
	}

	public Hashtable(int initialCapacity) {
		this(initialCapacity, DEFAULT_LOADFACTOR);
	}

	public Hashtable(int initialCapacity, float loadFactor) {
		map = new HashMap(initialCapacity, loadFactor);
	}

	public Hashtable(Map t) {
		map = new HashMap(t);
	}

	private Hashtable(HashMap map) {
		this.map = map;
	}

	public synchronized int size() {
		return map.size();
	}

	public synchronized boolean isEmpty() {
		return map.isEmpty();
	}

	public synchronized Enumeration keys() {
		return new Vector(map.keySet()).elements();
	}

	public synchronized Enumeration elements() {
		return new Vector(map.values()).elements();
	}

	public synchronized boolean contains(Object val) {
		if (val == null) {
			throw new NullPointerException();
		}
		return map.containsValue(val);
	}

	public synchronized boolean containsKey(Object key) {
		if (key == null) {
			throw new NullPointerException();
		}
		return map.containsKey(key);
	}

	public synchronized boolean containsValue(Object val) {
		if (val == null) {
			throw new NullPointerException();
		}
		return map.containsValue(val);
	}

	public synchronized Object get(Object key) {
		if (key == null) {
			throw new NullPointerException();
		}
		return map.get(key);
	}

	protected synchronized void rehash() {
		map.rehash();
	}

	public synchronized Object put(Object key, Object val) {
		if (key == null || val == null) {
			throw new NullPointerException();
		}
		return map.put(key, val);
	}

	public synchronized Object remove(Object key) {
		return map.remove(key);
	}

	public synchronized void putAll(Map t) {
		map.putAll(t);
	}

	public synchronized void clear() {
		map.clear();
	}

	public synchronized Object clone() {
		try {
			Hashtable h = (Hashtable)super.clone();
			h.map = (HashMap)map.clone();
			return (h);
		}
		catch (CloneNotSupportedException _) {
			return (null);
		}
	}

	public synchronized String toString() {
		return map.toString();
	}

	public synchronized Set keySet() {
		return map.keySet();
	}

	public synchronized Set entrySet() {
		return map.entrySet();
	}

	public synchronized Collection values() {
		return map.values();
	}

	public synchronized boolean equals(Object o) {
		return AbstractMap.equals(this, o);
	}

	public synchronized int hashCode() {
		return map.hashCode();
	}

	/* Serialization ---------------------------------------- */

	/**
	 * See ObjectInputStream doc.  (Used when serialized stream
	 * contains a class that doesn't indicate (in stream) that
	 * this class is a superclass, but in the VM this class is a
	 * superclass of that class.
	 */
	private void readObjectNoData() 
	{
		this.map = new HashMap();
	}

	/**
	 * Read this hashtable from a stream.  Tries to be compatible
	 * with Sun's serialized hashtables.
	 *
	 * Sun's hashtable are serialized as:
	 *    int threshhold
	 *    float loadfactor
	 *    int capacity
	 *    int element count
	 *    <key>, <object> elements.
	 *
	 * We don't really match that format naturally, so some fudging
	 * goes on.
	 */
	private void readObject(ObjectInputStream stream) 
		throws IOException, ClassNotFoundException 
	{
		// Get the "fields" out of the stream.  Should be a "loadFactor" field and a "threshold" field.
		ObjectInputStream.GetField fieldMucker = stream.readFields();
		float loadFactor = fieldMucker.get("loadFactor", DEFAULT_LOADFACTOR);
		int threshold = fieldMucker.get("threshold", (int)0);

		// "capacity" and "size" are stored in the blockdata:

		// No need to synchronized(this), as nothing has a handle on this yet

		int capacity = stream.readInt();
		this.map = new HashMap(capacity, loadFactor);

		// Read entries
		int size = stream.readInt();
		while (size-- > 0)
		{
			Object k = stream.readObject();
			Object o = stream.readObject();
			map.put(k, o);
		}
	}

	/**
	 * Write this hashtable into a stream.  See readObject() doc
	 * for format info.
	 */
	private void writeObject(ObjectOutputStream stream) 
		throws IOException 
	{
		// Fake the "threshold" and "loadFactor" fields:
		ObjectOutputStream.PutField fieldMucker = stream.putFields();
		fieldMucker.put("loadFactor", (float)(this.map.loadFactor));
		fieldMucker.put("threshold", (int)(this.map.size() * map.loadFactor));
		stream.writeFields();

		// "capacity" and "size" are stored in the blockdata:
		synchronized(this)
		{
			stream.writeInt(map.getTableLength()); // current capacity of table
			stream.writeInt(map.size()); // number of elements
			
			Iterator i = map.entrySet().iterator();
			while(i.hasNext()) {
				Map.Entry e = (Map.Entry)i.next();
				stream.writeObject(e.getKey());
				stream.writeObject(e.getValue());
			}
		}
	}

}
