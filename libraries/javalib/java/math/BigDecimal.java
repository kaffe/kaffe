/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 1999, 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * 
 * @author Edouard G. Parmelan <egp@free.fr>
 */

package java.math;


public class BigDecimal extends Number implements Comparable {

    private static final long serialVersionUID = 6108874887143696463L;

    public static final int ROUND_UP = 0;
    public static final int ROUND_DOWN = 1;
    public static final int ROUND_CEILING = 2;
    public static final int ROUND_FLOOR = 3;
    public static final int ROUND_HALF_UP = 4;
    public static final int ROUND_HALF_DOWN = 5;
    public static final int ROUND_HALF_EVEN = 6;
    public static final int ROUND_UNNECESSARY = 7;

    private BigInteger num;
    private int scale;

    public BigDecimal(String val) {
	int dot = val.indexOf('.');
	if (dot == -1) {
	    num = new BigInteger(val, 10);
	}
	else {
	    num = new BigInteger(val.substring(0, dot) + val.substring(dot + 1),
				 10);
	    scale = val.length() - dot - 1;
	}
    }

    public BigDecimal(double val) {
	if (Double.isInfinite(val) || Double.isNaN(val)) {
	    throw new NumberFormatException("Infinite or NaN");
	}
	
	// JLS 20.10.22 describle IEEE decomposition of a double:
	// val == s * m * 2^(e-1075) where
	long bits = Double.doubleToLongBits(val);
	int s = ((bits >> 63) == 0) ? 1 : -1;
	int e = (int)((bits >> 52) & 0x7ffL);
	long m = (e == 0)
	    ? (bits & 0xfffffffffffffL) << 1
	    : (bits & 0xfffffffffffffL) | 0x10000000000000L;
	e -= 1075;
	// now, val == s * m * 2^e

	if (m == 0) {
	    num = BigInteger.ZERO;
	    return;
	}

	// normalize mantissa and exponent
	while ((m & 1) == 0) {
	    m >>= 1;
	    e++;
	}

	num = BigInteger.valueOf(s * m);
	if (e > 0) {
	    // 2^e > 1
	    num = num.multiply(BigInteger.valueOf(2).pow(e));
	}
	else if (e < 0) {
	    // 2^e < 1, find smallest integer multiplier P and scale Q
	    // where 2^e == P / 10^Q
	    // 2^e == 1/2^-e == 10^-e/(2^-e * 10^-e)
	    //     == (10/2)^-e * 1/(10^-e)
	    num = num.multiply(BigInteger.valueOf(5).pow(-e));
	    scale = -e;
	}
    }

    public BigDecimal(BigInteger unscaledVal) {
	num = unscaledVal;
    }

    public BigDecimal(BigInteger unscaledVal, int scale) {
	if (scale < 0) {
	    throw new NumberFormatException("Negative scale: " + scale);
	}
	num = unscaledVal;
	this.scale = scale;
    }

    public static BigDecimal valueOf(long unscaledVal, int scale) {
	return new BigDecimal(BigInteger.valueOf(unscaledVal), scale);
    }

    public static BigDecimal valueOf(long val) {
	return valueOf(val, 0);
    }

    public BigDecimal add(BigDecimal val) {
	// promote scale to max(scale, val.scale)
	BigInteger a, b;
	int newScale;
	int n = scale - val.scale;
	if (n == 0) {
	    newScale = scale;
	    a = num;
	    b = val.num;
	}
	else if (n > 0) {
	    // this.scale > val.scale
	    newScale = scale;
	    a = num;
	    b = mul10n(val.num, n);
	}
	else {
	    // this.scale < val.scale
	    newScale = val.scale;
	    a = mul10n(num, -n);
	    b = val.num;
	}
	return new BigDecimal(a.add(b), newScale);
    }

    public BigDecimal subtract(BigDecimal val) {
	// promote scale to max(scale, val.scale)
	BigInteger a, b;
	int newScale;
	int n = scale - val.scale;
	if (n == 0) {
	    newScale = scale;
	    a = num;
	    b = val.num;
	}
	else if (n > 0) {
	    // this.scale > val.scale
	    newScale = scale;
	    a = num;
	    b = mul10n(val.num, n);
	}
	else {
	    // this.scale < val.scale
	    newScale = val.scale;
	    a = mul10n(num, -n);
	    b = val.num;
	}
	return new BigDecimal(a.subtract(b), newScale);
    }

    public BigDecimal multiply(BigDecimal val) {
	return new BigDecimal(num.multiply(val.num), scale + val.scale);
    }

    public BigDecimal divide(BigDecimal val, int newScale, int roundingMode) {
	if (newScale < 0) {
	    throw new NumberFormatException("Negative scale: " + newScale);
	}
	if (roundingMode < ROUND_UP || roundingMode > ROUND_UNNECESSARY) {
	    throw new IllegalArgumentException("Illegal rounding mode: " +
					       roundingMode);
	}

	// Quotient should have scale newScale, so promote scale of numerator
	// or denominator to have correct quotient scale.
	// num / 10^scale == (Q val.num) / 10^(newScale + val.scale) + R
	// promote scale to max(scale, newScale + val.scale)
	BigInteger a, b;
	int n = scale - newScale - val.scale;
	if (n == 0) {
	    a = num;
	    b = val.num;
	}
	else if (n > 0) {
	    // this.scale > newScale + val.scale
	    a = num;
	    b = mul10n(val.num, n);
	}
	else {
	    // this.scale < newScale + val.scale
	    a = mul10n(num, -n);
	    b = val.num;
	}
	
	// this should throw ArithmeticException("Divide by zero")
	BigInteger[] qr = a.divideAndRemainder(b);
	BigInteger q = qr[0], r = qr[1];
	if (r.signum() == 0) {
	    return new BigDecimal(q, newScale);
	}

	int sign = a.signum() * b.signum();
	// half rounding study (r / b) <=> .5 aka r * 2 <=> b
	int half = r.abs().multiply(BigInteger.valueOf(2)).compareTo(b.abs());
	switch (roundingMode) {
	case ROUND_CEILING:
	    roundingMode = (sign > 0) ? ROUND_UP : ROUND_DOWN;
	    break;
	case ROUND_FLOOR:
	    roundingMode = (sign > 0) ? ROUND_DOWN : ROUND_UP;
	    break;
	case ROUND_HALF_UP:
	    roundingMode = (half >= 0) ? ROUND_UP : ROUND_DOWN;
	    break;
	case ROUND_HALF_DOWN:
	    roundingMode = (half > 0) ? ROUND_UP : ROUND_DOWN;
	    break;
	case ROUND_HALF_EVEN:
	    if (half == 0) {
		roundingMode = q.testBit(0) ? ROUND_UP : ROUND_DOWN;
	    }
	    else {
		roundingMode = (half > 0) ? ROUND_UP : ROUND_DOWN;
	    }
	    break;
	case ROUND_UNNECESSARY:
	    throw new ArithmeticException("Rounding necessary");
	}

	if (roundingMode == ROUND_UP)
	  q = q.abs().add(BigInteger.ONE);
	else
	  q = q.abs();
    
	if (sign > 0)
	  return new BigDecimal(q, newScale);
	else
	  return new BigDecimal(q.negate(), newScale);
    }

    public BigDecimal divide(BigDecimal val, int roundingMode) {
	return divide(val, scale, roundingMode);
    }

    public BigDecimal abs() {
	return (num.signum() < 0) ? negate() : this;
    }

    public BigDecimal negate() {
	return new BigDecimal(num.negate(), scale);
    }

    public int signum() {
	return num.signum();
    }

    public int scale() {
	return scale;
    }

    public BigInteger unscaledValue() {
	return num;
    }

    public BigDecimal setScale(int newScale, int roundingMode) {
	if (newScale < 0) {
	    throw new ArithmeticException("Negative scale: " + newScale);
	}
	if (roundingMode < ROUND_UP || roundingMode > ROUND_UNNECESSARY) {
	    throw new IllegalArgumentException("Illegal rounding mode: "
					       + roundingMode);
	}
	if (scale == newScale) {
	    return this;
	}
	else if (scale < newScale) {
	    return new BigDecimal(mul10n(num, newScale - scale), newScale);
	}
	else {
	    return divide(valueOf(1), newScale, roundingMode);
	}
    }

    public BigDecimal setScale(int newScale) {
	return setScale(newScale, ROUND_UNNECESSARY);
    }

    public BigDecimal movePointLeft(int n) {
	return (n < 0) ? movePointRight(-n) : new BigDecimal(num, scale + n);
    }

    public BigDecimal movePointRight(int n) {
	// When n < 0, we should return movePointLeft(-n).  In this case,
	// it's new BigDecimal(num, scale + (-n)).  Optimize it with
	// case scale >= n as scale >= 0.
	return (scale >= n)
	    ? new BigDecimal(num, scale - n)
	    : new BigDecimal(mul10n(num, n - scale), 0);
    }

    public int compareTo(BigDecimal that) {
	// promote scale to max(scale, val.scale)
	BigInteger a, b;
	int n = scale - that.scale;
	if (n == 0) {
	    a = num;
	    b = that.num;
	}
	else if (n > 0) {
	    // this.scale > that.scale
	    a = num;
	    b = mul10n(that.num, n);
	}
	else {
	    // this.scale < that.scale
	    a = mul10n(num, -n);
	    b = that.num;
	}
	return a.compareTo(b);
    }

    public int compareTo(Object obj) {
	return compareTo((BigDecimal)obj);
    }

    public boolean equals(Object obj) {
	if (!(obj instanceof BigDecimal)) {
	    return false;
	}
	BigDecimal that = (BigDecimal)obj;
	// Note: JDK say this is not (compareTo(that) == 0)
	return (scale == that.scale) && num.equals(that.num);
    }

    public BigDecimal min(BigDecimal that) {
	return (compareTo(that) < 0) ? this : that;
    }

    public BigDecimal max(BigDecimal that) {
	return (compareTo(that) < 0) ? that : this;
    }

    public int hashCode() {
	// This is not describe in JDK 1.1, JDK 1.2 nor JDK 1.3
	return num.hashCode() ^ scale;
    }

    public String toString() {
	if (scale == 0) {
	    return num.toString();
	}

	String intStr = num.abs().toString();
	int dot = intStr.length() - scale;
	StringBuffer sb = new StringBuffer(intStr.length() +
					   ((dot <= 0) ? 3 - dot : 2));
	while (dot <= 0) {
	    sb.append('0');
	    dot++;
	}
	sb.append(intStr);
	sb.insert(dot, '.');
	if(num.signum() < 0) {
	    sb.insert(0, '-');
	}
	return sb.toString();
    }

    public BigInteger toBigInteger() {
	return (scale == 0) ? num : num.divide(pow10(scale));
    }

    public int intValue() {
	return toBigInteger().intValue();
    }

    public long longValue() {
	return toBigInteger().longValue();
    }

    public float floatValue() {
	return Float.valueOf(toString()).floatValue();
    }

    public double doubleValue() {
	return Double.valueOf(toString()).doubleValue();
    }


    // Helper function, return 10^n.
    static private BigInteger pow10(int n) {
	return BigInteger.valueOf(10).pow(n);
    }

    // Helper function, return a * 10^n.
    static private BigInteger mul10n(BigInteger a, int n) {
	return a.multiply(BigInteger.valueOf(10).pow(n));
    }
}
