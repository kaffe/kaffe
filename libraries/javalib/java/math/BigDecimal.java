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

public class BigDecimal extends Number implements Comparable {

private static final long serialVersionUID = 6108874887143696463L;
public static final int ROUND_UP = 1;
public static final int ROUND_DOWN = 2;
public static final int ROUND_CEILING = 3;
public static final int ROUND_FLOOR = 4;
public static final int ROUND_HALF_UP = 5;
public static final int ROUND_HALF_DOWN = 6;
public static final int ROUND_HALF_EVEN = 7;
public static final int ROUND_UNNECESSARY = 8;

public BigDecimal(String val) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal(double val) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal(BigInteger val) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal(BigInteger val, int scale) {
	throw new kaffe.util.NotImplemented();
}

public static BigDecimal valueOf(long val, int scale) {
	throw new kaffe.util.NotImplemented();
}

public static BigDecimal valueOf(long val) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal add(BigDecimal val) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal subtract(BigDecimal val) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal multiply(BigDecimal val) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal divide(BigDecimal val, int scale, int roundingMode) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal divide(BigDecimal val, int roundingMode) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal abs() {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal negate() {
	throw new kaffe.util.NotImplemented();
}

public int signum() {
	throw new kaffe.util.NotImplemented();
}

public int scale() {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal setScale(int scale, int roundingMode) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal setScale(int scale) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal movePointLeft(int n) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal movePointRight(int n) {
	throw new kaffe.util.NotImplemented();
}

public int compareTo(Object obj) {
	return compareTo((BigDecimal)obj);
}

public int compareTo(BigDecimal val) {
	throw new kaffe.util.NotImplemented();
}

public boolean equals(Object x) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal min(BigDecimal val) {
	throw new kaffe.util.NotImplemented();
}

public BigDecimal max(BigDecimal val) {
	throw new kaffe.util.NotImplemented();
}

public int hashCode() {
	throw new kaffe.util.NotImplemented();
}

public String toString() {
	throw new kaffe.util.NotImplemented();
}

public BigInteger toBigInteger() {
	throw new kaffe.util.NotImplemented();
}

public int intValue() {
        throw new kaffe.util.NotImplemented();
}

public long longValue() {
        throw new kaffe.util.NotImplemented();
}

public float floatValue() {
        throw new kaffe.util.NotImplemented();
}

public double doubleValue() {
        throw new kaffe.util.NotImplemented();
}

}
