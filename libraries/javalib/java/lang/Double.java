/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;

public final class Double extends Number {

  public static final double POSITIVE_INFINITY = 1.0 / 0.0;
  public static final double NEGATIVE_INFINITY = -1.0 / 0.0;
  public static final double NaN = 0.0 / 0.0;
  public static final double MIN_VALUE = longBitsToDouble(0x1L);
  public static final double MAX_VALUE = longBitsToDouble(0x7fefffffffffffffL);
  public static final Class TYPE = Class.getPrimitiveClass("double");

  private static final int NUM_MANTISSA_BITS = 52;
  private static final long EXPONENT_MASK = 0x7ff0000000000000L;
  private static final long MANTISSA_MASK = 0x000fffffffffffffL;

  // This table that tells us how many decimal digits are needed to uniquely
  // specify N binary bits, i.e.: bitsToDecimal[N-1] = Ceiling(N ln 2 / ln 10).
  static final int bitsToDecimal[] = new int[] {
     1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,
     6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10,
    10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15,
    15, 16, 16, 16
  };

  /* This is what Sun's JDK1.1 "serialver java.lang.Double" spits out */
  private static final long serialVersionUID = -9172774392245257468L;

  private final double value;

  public static native long doubleToLongBits(double value);
  public static native double longBitsToDouble(long bits);

  static native String toStringWithPrecision(double value, int precision);
  static native double valueOf0(String s) throws NumberFormatException;
  
  public Double(double value) {
    this.value = value;
  }

  public Double(String s) throws NumberFormatException {
    this.value = valueOf(s).value;
  }

  public double doubleValue() {
    return value;
  }

  public static String toString(double value) {
    int precision, bitIndex;

    // Handle exceptional values
    if (isNaN(value))
      return "NaN";
    if (value == POSITIVE_INFINITY)
      return "Infinity";
    if (value == NEGATIVE_INFINITY)
      return "-Infinity";

    // Determine number of digits of decimal precision to display
    long bits = doubleToLongBits(value);
    if ((bits & EXPONENT_MASK) == 0) {			// denormalized value
	    for (bitIndex = NUM_MANTISSA_BITS - 1;
		bitIndex > 0 && ((1L << bitIndex) & bits) == 0;
		bitIndex--);
	    precision = bitsToDecimal[bitIndex];
    } else {						// normalized value
	    precision = bitsToDecimal[NUM_MANTISSA_BITS - 1];
    }

    // Add an extra digit to handle rounding
    precision++;

    // Display value
    return toStringWithPrecision(value, precision);
  }

  public String toString() {
    return toString(value);
  }

  public boolean equals(Object that) {
    return (that instanceof Double
      && doubleToLongBits(this.value)
	== doubleToLongBits(((Double)that).value));
  }

  public float floatValue()
  {
    return ((float)value);
  }

  public int hashCode()
  {
    return ((int)value);
  }
  
  public int intValue()
  {
    return ((int)value);
  }
  
  public boolean isInfinite() {
    return ((value == POSITIVE_INFINITY) || (value == NEGATIVE_INFINITY));
  }
  
  public static boolean isInfinite(double v) {
    return ((v == POSITIVE_INFINITY) || (v == NEGATIVE_INFINITY));
  }
  
  public boolean isNaN() {
    return isNaN(value);
  }
  
  public static boolean isNaN(double v) {
    long bits = doubleToLongBits(v);

    return ((bits & EXPONENT_MASK) == EXPONENT_MASK
	&& (bits & MANTISSA_MASK) != 0);
  }
 
  public long longValue() {
    return (long) value;
  }

  public static Double valueOf(String s) throws NumberFormatException {
    if (s == null) {
      throw new NullPointerException();
    }
    return (new Double(valueOf0(s)));
  }

  public static double parseDouble(String s) throws NumberFormatException {
    if (s == null) {
      throw new NullPointerException();
    }
    return valueOf0(s);
  }

}
