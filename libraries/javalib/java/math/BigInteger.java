/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.math;

import java.util.Random;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.IOException;
import kaffe.util.Ptr;

public class BigInteger extends Number {

private static final long serialVersionUID = -8287574255936472291L;
private Ptr number;

static {
	System.loadLibrary("math");
	initialize0();
}

public BigInteger(byte val[]) throws NumberFormatException {
	this(1, val);
}

public BigInteger(int signum, byte magnitude[]) throws NumberFormatException {
	this();
	if (magnitude.length != 0) {
		if (signum == 0) {
			throw new NumberFormatException();
		}
		assignBytes0(signum, magnitude);
	}
}

public BigInteger(String val, int radix) throws NumberFormatException {
	this();
	assignString0(val, radix);
}

public BigInteger(String val) throws NumberFormatException {
	this(val, 10);
}

public BigInteger(int numBits, Random rndSrc) throws IllegalArgumentException {
	throw new kaffe.util.NotImplemented();
}

public BigInteger(int bitLength, int certainty, Random rnd) {
	throw new kaffe.util.NotImplemented();
}

private BigInteger() {
	init0();
}

public static BigInteger valueOf(long val) {
	// Not the fastest way to do this ...
	return (new BigInteger(Long.toString(val)));
}

public BigInteger add(BigInteger val) throws ArithmeticException {
	BigInteger r = new BigInteger();
	r.add0(this, val);
	return (r);
}

public BigInteger subtract(BigInteger val) {
	BigInteger r = new BigInteger();
	r.sub0(this, val);
	return (r);
}

public BigInteger multiply(BigInteger val) {
	BigInteger r = new BigInteger();
	r.mul0(this, val);
	return (r);
}

public BigInteger divide(BigInteger val) throws ArithmeticException {
	BigInteger r = new BigInteger();
	r.div0(this, val);
	return (r);
}

public BigInteger remainder(BigInteger val) throws ArithmeticException {
	BigInteger r = new BigInteger();
	r.rem0(this, val);
	return (r);
}

public BigInteger[] divideAndRemainder(BigInteger val) throws ArithmeticException {
	BigInteger q = new BigInteger();
	BigInteger r = new BigInteger();
	divrem0(q, r, this, val);
	return (new BigInteger[]{ q, r });
}

public BigInteger pow(int exponent) throws ArithmeticException {
	BigInteger r = new BigInteger();
	r.pow0(this, exponent);
	return (r);
}

public BigInteger gcd(BigInteger val) {
	BigInteger r = new BigInteger();
	r.gcd0(this, val);
	return (r);
}

public BigInteger abs() {
	BigInteger r = new BigInteger();
	r.abs0(this);
	return (r);
}

public BigInteger negate() {
	BigInteger r = new BigInteger();
	r.neg0(this);
	return (r);
}

public int signum() {
	BigInteger zero = new BigInteger();
	return (compareTo(zero));
}

public BigInteger mod(BigInteger mod) {
	BigInteger r = new BigInteger();
	r.mod0(this, mod);
	return (r);
}

public BigInteger modPow(BigInteger exponent, BigInteger mod) {
	BigInteger r = new BigInteger();
	r.modpow0(this, exponent, mod);
	return (r);
}

public BigInteger modInverse(BigInteger m) throws ArithmeticException {
	throw new kaffe.util.NotImplemented();
}

public BigInteger shiftLeft(int n) {
	BigInteger s = new BigInteger();
	s.setbit0(s, n);
	s.mul0(this, s);
	return (s);
}

public BigInteger shiftRight(int n) {
	BigInteger s = new BigInteger();
	s.setbit0(s, n);
	s.div0(this, s);
	return (s);
}

public BigInteger and(BigInteger val) {
	BigInteger r = new BigInteger();
	r.and0(this, val);
	return (r);
}

public BigInteger or(BigInteger val) {
	BigInteger r = new BigInteger();
	r.or0(this, val);
	return (r);
}

public BigInteger xor(BigInteger val) {
	BigInteger r = new BigInteger();
	r.xor0(this, val);
	return (r);
}

public BigInteger not() {
	BigInteger r = new BigInteger();
	r.not0(this);
	return (r);
}

public BigInteger andNot(BigInteger val) {
	BigInteger r = new BigInteger();
	r.and0(this, val);
	r.not0(r);
	return (r);
}

public boolean testBit(int n) throws ArithmeticException {
	BigInteger b = new BigInteger();
	b.setbit0(this, n);
	if (cmp0(b, this) == 0) {
		return (true);
	}
	else {
		return (false);
	}
}

public BigInteger setBit(int n) throws ArithmeticException {
	BigInteger r = new BigInteger();
	r.setbit0(this, n);
	return (r);
}

public BigInteger clearBit(int n) throws ArithmeticException {
	BigInteger r = new BigInteger();
	r.clrbit0(this, n);
	return (r);
}

public BigInteger flipBit(int n) throws ArithmeticException {
	BigInteger r = new BigInteger();
	r.setbit0(r, n);
	r.xor0(r, this);
	return (r);
}

public int getLowestSetBit() {
	return (scansetbit0());
}

public int bitLength() {
	throw new kaffe.util.NotImplemented();
}

public int bitCount() {
	throw new kaffe.util.NotImplemented();
}

public boolean isProbablePrime(int certainty) {
	if (probablyPrime0(certainty) == 0) {
		return (false);
	}
	return (true);
}

public int compareTo(BigInteger val) {
	return (cmp0(this, val));
}

public boolean equals(Object o) {
	if (!(o instanceof BigInteger)) {
		return (false);
	}
	if (compareTo((BigInteger)o) == 0) {
		return (true);
	}
	return (false);
}

public BigInteger min(BigInteger val) {
	int r = compareTo(val);
	if (r > 0) {
		return (val);
	}
	else {
		return (this);
	}
}

public BigInteger max(BigInteger val) {
	int r = compareTo(val);
	if (r < 0) {
		return (val);
	}
	else {
		return (this);
	}
}

public int hashCode() {
	// It probably isn't this but I don't know what it's suppose to be.
	return (super.hashCode());
}

public String toString(int radix) {
	return (toString0(radix));
}

public String toString() {
	return (toString(10));
}

public byte[] toByteArray() {
	throw new kaffe.util.NotImplemented();
}

public int intValue() {
	return (toInt0());
}

public long longValue() {
	// Not very efficient ...
	return (Long.valueOf(toString()).longValue());
}

public float floatValue() {
	return ((float)doubleValue());
}

public double doubleValue() {
	return (toDouble0());
}

protected void finalize() throws Throwable {
	finalize0();
	super.finalize();
}

private void writeObject(ObjectOutputStream s) throws IOException {
	s.writeInt(-1);
	s.writeInt(-1);
	s.writeInt(-2);
	s.writeInt(getLowestSetBit());
	s.writeInt(signum());
	s.writeObject(toByteArray());	/* not implemented right now */
}

/**
 * deserialize this object
 */
private void readObject(ObjectInputStream s)
                 throws IOException, ClassNotFoundException {
    	/* serialized form is
	 * int bitCount
	 *
	 *   The bitCount of this BigInteger, as returned by 
	 *   bitCount(), or -1 (either value is acceptable).
	 *
	 * int bitLength
	 *
	 *   The bitLength of this BigInteger, as returned by bitLength(), 
	 *   or -1 (either value is acceptable).
	 *
	 * int firstNonzeroByteNum
	 *
	 *   The byte-number of the lowest-order nonzero byte in the 
	 *   magnitude of this BigInteger, or -2 (either value is acceptable). 
	 *   The least significant byte has byte-number 0, the next byte in 
	 *   order of increasing significance has byte-number 1, and so forth.
	 *
	 * int lowestSetBit
	 *
	 *   The lowest set bit of this BigInteger, as returned by 
	 *   getLowestSetBit(), or -2 (either value is acceptable).
	 *
	 * byte[] magnitude
	 *
	 *   The magnitude of this BigInteger, in big-endian byte-order: 
	 *   the zeroth element of this array is the most-significant byte 
	 *   of the magnitude. The magnitude must be "minimal" in that the 
	 *   most-significant byte (magnitude[0]) must be non-zero.  This is 
	 *   necessary to ensure that there is exactly one representation for 
	 *   each BigInteger value. Note that this implies that the BigInteger 
	 *   zero has a zero-length magnitude array.
	 *
	 * int signum
	 *
         *   The signum of this BigInteger: -1 for negative, 0 for zero, 
	 *   or 1 for positive.  Note that the BigInteger zero must have a 
	 *   signum of 0. This is necessary to ensures that there is exactly 
	 *   one representation for each BigInteger value.
	 *
	 * NB: the order has magnitude last cause it's an object 
	 */

	/* 
	 * I don't know whether that's the right way to do it 
	 */
	System.out.println("jmb: readObject called, this is experimental");
	int bitCount = s.readInt();	// ignored, not implemented
	int bitLength = s.readInt();	// ignored, not implemented
	int firstNonzeroByteNum = s.readInt();	// ignored, can be recomputed?
	int lowestSetBit = s.readInt();	// ignored, can be recomputed?
	int signum = s.readInt();

	// magnitude is an object and hence last
	byte[] magnitude = (byte[])s.readObject();

	init0();	/* I think this is needed because the serialization 
			 * won't invoke the constructor
			 */
	assignBytes0(signum, magnitude);
	System.out.println("jmb: " + toString());
}

private native void init0();
private native void finalize0();

private native void assignBytes0(int s, byte[] m);
private native void assignString0(String v, int i);
private native void assignLong0(long v);

private native void add0(BigInteger s1, BigInteger s2);
private native void sub0(BigInteger s1, BigInteger s2);
private native void mul0(BigInteger s1, BigInteger s2);
private native void div0(BigInteger s1, BigInteger s2);
private native void rem0(BigInteger s1, BigInteger s2);
private native void abs0(BigInteger s);
private native void neg0(BigInteger s);
private native void pow0(BigInteger s, int p);
private native void gcd0(BigInteger s1, BigInteger s2);
private native void mod0(BigInteger s1, BigInteger s2);
private native void modpow0(BigInteger s1, BigInteger s2, BigInteger s3);

private native void and0(BigInteger s1, BigInteger s2);
private native void or0(BigInteger s1, BigInteger s2);
private native void xor0(BigInteger s1, BigInteger s2);
private native void not0(BigInteger s);

private native void clrbit0(BigInteger s, int n);
private native void setbit0(BigInteger s, int n);
private native int scansetbit0();
private native int probablyPrime0(int cert);

private native static int cmp0(BigInteger s1, BigInteger s2);

private native static void initialize0();
private native static void divrem0(BigInteger r, BigInteger q, BigInteger s1, BigInteger s2);
private native String toString0(int base);
private native double toDouble0();
private native int toInt0();

}
