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

public final class Double extends Number implements Comparable {

  private static final int NUM_MANTISSA_BITS	= 52;
  private static final long EXPONENT_MASK	= 0x7ff0000000000000L;
  private static final long MANTISSA_MASK	= 0x000fffffffffffffL;
  private static final long NAN_BITS		= 0x7ff8000000000000L;

  // Jacks requires that theses constants are _compile time_ constants.
  // test-case DoubleConst will check that are correctly compiled.
  public static final double POSITIVE_INFINITY	= 1.0d / 0.0d;
  public static final double NEGATIVE_INFINITY	= -1.0d / 0.0d;
  public static final double NaN		= 0.0d / 0.0d;
  public static final double MIN_VALUE		= 4.9406564584124654418e-324;
  public static final double MAX_VALUE		= 1.7976931348623157081e+308;

  public static final Class TYPE = VMClassLoader.getPrimitiveClass('D');

  private static final int MAX_DIGITS = 19;

  private final double value;

  /* This is what Sun's JDK1.1 "serialver java.lang.Double" spits out */
  private static final long serialVersionUID = -9172774392245257468L;

  public static native long doubleToRawLongBits(double value);
  public static native double longBitsToDouble(long bits);

  static native String toStringWithPrecision(double value, int max_precision);
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

  public int compareTo(Double that) {
    return compare(this.value, that.value);
  }

  public static int compare(double d1, double d2) {
    final long bits1 = doubleToLongBits(d1);
    final long bits2 = doubleToLongBits(d2);

    if ((bits1>>>63)==1 && (bits2>>>63)==1) {
      return (bits1 < bits2) ?  1 : (bits1 == bits2) ? 0 : -1;
    } else {
      return (bits1 < bits2) ? -1 : (bits1 == bits2) ? 0 :  1;
    }
  }

  public int compareTo(Object that) {
    return compareTo((Double) that);
  }

  public static String toString(double value) {
    // Handle exceptional values
    if (isNaN(value))
      return "NaN";
    if (value == POSITIVE_INFINITY)
      return "Infinity";
    if (value == NEGATIVE_INFINITY)
      return "-Infinity";

    return toStringWithPrecision(value, MAX_DIGITS);
  }

  public String toString() {
    return toString(value);
  }

  public boolean equals(Object that) {
    return (that instanceof Double
      && doubleToLongBits(value) == doubleToLongBits(((Double)that).value));
  }

  public float floatValue()
  {
    return ((float)value);
  }

  public int hashCode()
  {
    long bits = doubleToLongBits(value);

    return (int)bits ^ (int)(bits >> 32);
  }

  public int intValue()
  {
    return ((int)value);
  }

  public static long doubleToLongBits(double value) {
    long bits = doubleToRawLongBits(value);

    return isNaN(bits) ? NAN_BITS : bits;
  }

  public boolean isInfinite() {
    return isInfinite(value);
  }

  public static boolean isInfinite(double v) {
    return ((v == POSITIVE_INFINITY) || (v == NEGATIVE_INFINITY));
  }

  public boolean isNaN() {
    return isNaN(doubleToRawLongBits(value));
  }

  public static boolean isNaN(double v) {
    return isNaN(doubleToRawLongBits(v));
  }

  private static boolean isNaN(long b) {
    return ((b & EXPONENT_MASK) == EXPONENT_MASK && (b & MANTISSA_MASK) != 0);
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
