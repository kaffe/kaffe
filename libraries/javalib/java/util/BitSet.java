package java.util;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
final public class BitSet
  implements Cloneable
{
	Vector bits;
	final static int roundBits = 64;

public BitSet() {
	this(roundBits);
}

public BitSet(int nbits) {
	bits = new Vector();
	nbits = (nbits + roundBits - 1) & -roundBits;
	ensureSize(nbits);
}

public void and(BitSet bitset) {
	int limit = Math.min(size(), bitset.size());
	for (int bit = 0; bit < limit; bit++) {
		if (get(bit) == true && bitset.get(bit) == false) {
			clear(bit);
		}
	}
}

public void clear(int bit) {
	ensureSize(bit+1);
	bits.setElementAt(new Boolean(false), bit);
}

public Object clone() {
	BitSet bitset=new BitSet(size());

	int limit = bitset.size();
	for (int pos=0; pos<limit; pos++) {
		if (get(pos)) bitset.set(pos);
	}

	return bitset;
}

private void ensureSize(int size) {
	int bit = bits.size();
	if (size > bit) {
		bits.setSize(size);
		for (; bit < size; bit++) {
			bits.setElementAt(new Boolean(false), bit);
		}
	}
}

public boolean equals(Object obj) {
	if (obj instanceof BitSet) {
		BitSet bitset=(BitSet )obj;

		if (size()!=bitset.size()) return false;

		int limit = size();
		for (int pos=0; pos<limit; pos++) {
			if (get(pos) != bitset.get(pos)) return false;
		}
	}
	return true;
}

public boolean get(int bit) {
	ensureSize(bit+1);
	return ((Boolean)bits.elementAt(bit)).booleanValue();
}

public int hashCode() {
	int value=0;

	for (int pos=0; pos<Math.min(32, size()); pos++) {
		value=value*2;
		if (get(pos)) value++;
	}

	return value;
}

public void or(BitSet bitset) {
	int limit = Math.min(size(), bitset.size());
	for (int bit = 0; bit < limit; bit++) {
		if (get(bit) == false && bitset.get(bit) == true) {
			set(bit);
		}
	}
}

public void set(int bit) {
	ensureSize(bit+1);
	bits.setElementAt(new Boolean(true), bit);
}

public int size() {
	return bits.size();
}

public String toString() {
	StringBuffer buffer=new StringBuffer();
	boolean firstTime=true;

	buffer.append('{');
	int limit = size();
	for (int pos=0; pos<limit; pos++) {
		if (get(pos)) {
			if (!firstTime) buffer.append(", ");
			buffer.append(pos);
			firstTime=false;
		}
	}
	buffer.append('}');

	return buffer.toString();
}

public void xor(BitSet bitset) {
	int limit = Math.min(size(), bitset.size());
	for (int bit = 0; bit < limit; bit++) {
		if (get(bit) == false && bitset.get(bit) == true) {
			set(bit);
		}
		else if (get(bit) == true && bitset.get(bit) == true) {
			clear(bit);
		}
	}
}
}
