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

import java.io.Serializable;

public class BitSet
  implements Cloneable, Serializable
{
	int[] bits;
	int size;
	static int[] mask = {
	0x00000001,
	0x00000002,
	0x00000004,
	0x00000008,
	0x00000010,
	0x00000020,
	0x00000040,
	0x00000080,
	0x00000100,
	0x00000200,
	0x00000400,
	0x00000800,
	0x00001000,
	0x00002000,
	0x00004000,
	0x00008000,
	0x00010000,
	0x00020000,
	0x00040000,
	0x00080000,
	0x00100000,
	0x00200000,
	0x00400000,
	0x00800000,
	0x01000000,
	0x02000000,
	0x04000000,
	0x08000000,
	0x10000000,
	0x20000000,
	0x40000000,
	0x80000000
};
	static int[] invMask = {
	0xfffffffe,
	0xfffffffd,
	0xfffffffb,
	0xfffffff7,
	0xffffffef,
	0xffffffdf,
	0xffffffbf,
	0xffffff7f,
	0xfffffeff,
	0xfffffdff,
	0xfffffbff,
	0xfffff7ff,
	0xffffefff,
	0xffffdfff,
	0xffffbfff,
	0xffff7fff,
	0xfffeffff,
	0xfffdffff,
	0xfffbffff,
	0xfff7ffff,
	0xffefffff,
	0xffdfffff,
	0xffbfffff,
	0xff7fffff,
	0xfeffffff,
	0xfdffffff,
	0xfbffffff,
	0xf7ffffff,
	0xefffffff,
	0xdfffffff,
	0xbfffffff,
	0x7fffffff
};

public BitSet () {
	this( 32);
}

public BitSet ( int size ) {
	if ( size <= 32 ) {
		bits = new int[1];
		this.size = 32;
	}
	else {
		int n = (size+31) >> 5;
		bits = new int[n];
		this.size = n << 5;
	}
}

public void and ( BitSet other ) {
	int[]  ob = other.bits;
	int    i, n = ob.length;

	if ( n > bits.length )
		grow( n << 5);
		
	for ( i=0; i<n; i++ )
		bits[i] &= ob[i];
}

public void clear ( int bit ) {
	if ( bit < 32 ) {
		bits[0] &= invMask[bit];
	}
	else {
		if ( bit >= size )
			grow( bit+1);

		int n = bit >> 5;
		int i = bit - (n << 5);
	
		bits[n] &= invMask[i];
	}
}

public Object clone () {
	BitSet other = new BitSet( size);
	System.arraycopy( bits, 0, other.bits, 0, bits.length);
	return other;
}

public boolean equals ( Object obj ) {
	if ( obj instanceof BitSet ) {
		BitSet other = (BitSet) obj;

		int[]  ob = other.bits;
		int    i, n = ob.length;

		if ( n != bits.length )
			return false;
		
		if ( n == 1 ){
			return (bits[0] == ob[0]);
		}
		else {
			for ( i=0; i<n; i++ ){
				if ( bits[i] != ob[i] ) return false;
			}	
			return true;
		}
	}
	
	return false;
}

public boolean get ( int bit ) {
	if ( bit < 32 ) {
		return ((bits[0] & mask[bit]) != 0);
	}
	else {
		if ( bit >= size )
			return false;

		int n = bit >> 5;
		int i = bit - (n << 5);

		return ((bits[n] & mask[i]) != 0);
	}
}

void grow ( int newSize ) {
	int n = (newSize+31) >> 5;
	int[] newBits;
	
	// no need to play the stop and go game if this is continously incremented
	if ( n < (bits.length<<1) )
		n = bits.length<<1;
	
	newBits = new int[n];
	
	System.arraycopy( bits, 0, newBits, 0, bits.length);
	bits = newBits;
	size = n << 5;
}

public int hashCode () {
	// Hmm, is this specified ? We use a very simple scheme here
	int i, h, b, n = bits.length;
	
	if ( n == 1 ){
		return bits[0];
	}
	else {
		h = (bits[0] == 0) ? 19 : bits[0];
	
		for ( i=1; i<n; i++ ) {
			if ( (b = bits[i]) == 0 )
				b = i;
			h ^= (h * 65599) + b;
		}
		return h;
	}
}

public void or ( BitSet other ) {
	int[]  ob = other.bits;
	int    i, n = ob.length;

	if ( n > bits.length )
		grow( n << 5);
		
	for ( i=0; i<n; i++ )
		bits[i] |= ob[i];
}

public void set ( int bit ) {
	if ( bit < 32 ) {
		bits[0] |= mask[bit];
	}
	else {
		if ( bit >= size )
			grow( bit+1);

		int n = bit >> 5;
		int i = bit - (n << 5);

		bits[n] |= mask[i];
	}
}

public int size () {
	return size;
}

public String toString () {
	StringBuffer s = new StringBuffer( size); // rough guess
	int b, i, j, k, n = bits.length;
	
	s.append( '{');
	for ( i=0; i<n; i++ ) {
		if ( (b = bits[i]) != 0 ) {
			k = i << 5;
			for ( j=0; j<32; j++ ){
				if ( (b & mask[j]) != 0 ){
					s.append( k + j);
					s.append( ',');
				}
			}
		}
	}
	if ( (i = s.length()) > 1 )
		s.setCharAt( i - 1, '}');
	else
		s.append( '}');
	
	return s.toString();
}

public void xor ( BitSet other ) {
	int[]  ob = other.bits;
	int    i, n = ob.length;

	if ( n > bits.length )
		grow( n << 5);
		
	for ( i=0; i<n; i++ )
		bits[i] ^= ob[i];
}
}
