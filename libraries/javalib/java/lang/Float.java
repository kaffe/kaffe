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

public final class Float extends Number {

  public static final float POSITIVE_INFINITY = 1.0f / 0.0f;
  public static final float NEGATIVE_INFINITY = -1.0f / 0.0f;
  public static final float NaN = 0.0f / 0.0f;
  public static final float MIN_VALUE = intBitsToFloat(0x1);
  public static final float MAX_VALUE = intBitsToFloat(0x7f7fffff);
  public static final Class TYPE = Class.getPrimitiveClass("float");

  private static final int DECIMAL_PRECISION = 7; // ceiling (23 log 2)

  private final float value;

  /* This is what Sun's JDK1.1 "serialver java.lang.Float" spits out */  
  private static final long serialVersionUID = -2671257302660747028L;

  public static native int floatToIntBits(float value);
  public static native float intBitsToFloat(int bits);

  public Float(float value) {
    this.value = value;
  }
  
  public Float(double value) {
    this.value = (float) value;
  }
  
  public Float(String s) throws NumberFormatException {
    this.value = valueOf(s).value;
  }
  
  public boolean equals(Object obj) {
    if ((obj!=null) && (obj instanceof Float)) {

      final int leftBits=floatToIntBits(this.value);
      final int rightBits=floatToIntBits(((Float )obj).value);
      final int posBits=floatToIntBits(POSITIVE_INFINITY);
      final int negBits=floatToIntBits(NEGATIVE_INFINITY);

      if (this.isNaN() && ((Float )obj).isNaN()) {
        return true;
      }

      if (((leftBits == posBits) && (rightBits == posBits))
	  || ((leftBits == negBits) && (rightBits == negBits))) {
	return true;
      }
    
      return (leftBits == rightBits);
    }
    else {
      return false;
    }
  }
  
  public int hashCode() {
    return this.intValue();
  }
  
  public double doubleValue() {
    return (double )value;
  }
  
  public float floatValue() {
    return value;
  }
  
  public long longValue() {
    return (long )value;
  }
  
  public int intValue() {
    return (int )value;
  }

  public static String toString(float value) {
    if (isNaN(value))
      return "NaN";
    if (value == POSITIVE_INFINITY)
      return "Infinity";
    if (value == NEGATIVE_INFINITY)
      return "-Infinity";
    return Double.normalToString((double) value, DECIMAL_PRECISION);
  }

  public String toString() {
    return toString(this.floatValue());
  }
  
  public static boolean isNaN(float v) {
    /* A NaN is the only number which doesn't equal itself. Unfortunately,
       this test doesn't seem to work on all platforms. */
    /* return (v != v); */

    final int expMask = 0x7f800000;	// exponent mask
    final int manMask = 0x007fffff;	// mantissa mask
    final int bits = floatToIntBits(v);

    return ((bits & expMask) == expMask && (bits & manMask) != 0);
  }
  
  public boolean isNaN() {
    return isNaN(value);
  }
  
  public static boolean isInfinite(float v) {
    return (floatToIntBits(v)==floatToIntBits(POSITIVE_INFINITY))
	|| (floatToIntBits(v)==floatToIntBits(NEGATIVE_INFINITY));
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
}
