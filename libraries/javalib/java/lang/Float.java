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

public final class Float extends Number implements Comparable {

  private static final int NUM_MANTISSA_BITS	= 23;
  private static final int EXPONENT_MASK	= 0x7f800000;
  private static final int MANTISSA_MASK	= 0x007fffff;
  private static final int NAN_BITS		= 0x7fc00000;

  // Jacks requires that theses constants are _compile time_ constants.
  // test-case DoubleConst will check that are correctly compiled.
  public static final float POSITIVE_INFINITY	= 1.0f / 0.0f;
  public static final float NEGATIVE_INFINITY	= -1.0f / 0.0f;
  public static final float NaN			= 0.0f / 0.0f;
  public static final float MIN_VALUE		= 1.4012984643248170709e-45f;
  public static final float MAX_VALUE		= 3.4028234663852885981e+38f;

  public static final Class TYPE = VMClassLoader.getPrimitiveClass('F');
  private static final int MAX_DIGITS = 10;

  private final float value;

  /* This is what Sun's JDK1.1 "serialver java.lang.Float" spits out */
  private static final long serialVersionUID = -2671257302660747028L;

  public static native int floatToRawIntBits(float value);
  public static native float intBitsToFloat(int bits);
  private static native String toStringWithPrecision(float value, int max_precision);

  public Float(float value) {
    this.value = value;
  }

  public Float(double value) {
    this.value = (float) value;
  }

  public Float(String s) throws NumberFormatException {
    this.value = valueOf(s).value;
  }

  public boolean equals(Object that) {
    return (that instanceof Float
      && floatToIntBits(value) == floatToIntBits(((Float)that).value));
  }

  public int compareTo(Float that) {
    return compare (this.value, that.value);
  }

  public static int compare(float f1, float f2) {
    final int bits1 = floatToIntBits(f1);
    final int bits2 = floatToIntBits(f2);

    if ((bits1>>>31)==1 && (bits2>>>31)==1) {
      return (bits1 < bits2) ?  1 : (bits1 == bits2) ? 0 : -1;
    } else {
      return (bits1 < bits2) ? -1 : (bits1 == bits2) ? 0 :  1;
    }
  }
 
  public int compareTo(Object that) {
    return compareTo((Float) that);
  }

  public int hashCode() {
    return floatToIntBits(value);
  }

  public double doubleValue() {
    return (double)value;
  }

  public float floatValue() {
    return value;
  }

  public long longValue() {
    return (long)value;
  }

  public int intValue() {
    return (int)value;
  }

  public static String toString(float value) {
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

  public static int floatToIntBits(float value) {
    int bits = floatToRawIntBits(value);

    return isNaN(bits) ? NAN_BITS : bits;
  }

  public static boolean isNaN(float v) {
    return isNaN(floatToRawIntBits(v));
  }

  public boolean isNaN() {
    return isNaN(floatToRawIntBits(value));
  }

  private static boolean isNaN(int b) {
    return ((b & EXPONENT_MASK) == EXPONENT_MASK && (b & MANTISSA_MASK) != 0);
  }

  public static boolean isInfinite(float v) {
    return ((v == POSITIVE_INFINITY) || (v == NEGATIVE_INFINITY));
  }

  public boolean isInfinite() {
    return isInfinite(value);
  }

  public static Float valueOf(String s) throws NumberFormatException {
    if (s == null) {
      throw new NullPointerException();
    }
    return (new Float(Double.valueOf0(s)));
  }

  public static float parseFloat(String s) throws NumberFormatException {
    if (s == null) {
      throw new NullPointerException();
    }
    return (float) Double.valueOf0(s);
  }
}
