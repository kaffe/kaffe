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
  private final float value;
  
  public static final float POSITIVE_INFINITY = 1.0f / 0.0f;
  public static final float NEGATIVE_INFINITY = -1.0f / 0.0f;
  public static final float NaN = 0.0f / 0.0f;
  public static final float MAX_VALUE = 3.40282346638528860e+38f;
  //public static final float MIN_VALUE = 1.40129846432481707e-45f;
  public static final float MIN_VALUE = 1.40129846432481707e-38f;
  public static final Class TYPE = Class.getPrimitiveClass("float");
  
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
  
  public static native int floatToIntBits(float value);
  public static native float intBitsToFloat(int bits);
  
  public static native String toString(float f);

  public String toString() {
    return toString(this.floatValue());
  }
  
  public static native Float valueOf(String s) throws NumberFormatException;
  
  public static boolean isNaN(float v) {
    /* A NaN is the only number which doesn't equal itself */
    return (v != v);
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
}
