
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
import java.io.IOException;
import java.io.ObjectInputStream;

public class Hashtable extends Dictionary
		implements Map, Cloneable, Serializable {
	private static final long serialVersionUID = 1421746759512286392L;
	private static final int DEFAULT_CAPACITY = 11;
	private static final float DEFAULT_LOADFACTOR = 0.75f;
	private HashMap map;

	/* We need to keep these values for serialization */
	private float loadFactor;

	public Hashtable() {
		this(DEFAULT_CAPACITY, DEFAULT_LOADFACTOR);
	}

	public Hashtable(int initialCapacity) {
		this(initialCapacity, DEFAULT_LOADFACTOR);
	}

	public Hashtable(int initialCapacity, float loadFactor) {
		map = new HashMap(initialCapacity, loadFactor);
		this.loadFactor = loadFactor;
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

	class DefaultSerialization {

	private float loadFactor;
	private int threshold;

	private void readDefaultObject() {
		map = new HashMap((int)(threshold / loadFactor), loadFactor);
	}

	private void writeDefaultObject() {
		loadFactor = map.loadFactor;
		threshold = (int)(map.getTableLength() * loadFactor);
	}

	}

	/**
	* read this hashtable from a stream
	*/
	private void readObject(ObjectInputStream stream) throws IOException, ClassNotFoundException {
		// read all default fields
		stream.defaultReadObject();

		// create buckets
		stream.readInt();	// Capacity - ignore.
		int size = stream.readInt();

		// Read entries
		for (int i = 0; i < size; i++) {
			Object k = stream.readObject();
			Object o = stream.readObject();
			map.put(k, o);
		}
	}

	/**
	* write this hashtable into a stream
	*/
	private void writeObject(java.io.ObjectOutputStream stream) throws IOException {
		// write all default fields
		stream.defaultWriteObject();

		// remember how many buckets there were
		stream.writeInt(map.getTableLength());
		stream.writeInt(map.size());

		Iterator i = map.entrySet().iterator();
		while(i.hasNext()) {
			Map.Entry e = (Map.Entry)i.next();
			stream.writeObject(e.getKey());
			stream.writeObject(e.getValue());
		}
	}

}
