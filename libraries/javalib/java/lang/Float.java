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
  private float value;
  
  public static final float POSITIVE_INFINITY = (float)(1.0 / 0.0);
  public static final float NEGATIVE_INFINITY = (float)(-1.0 / 0.0);
  public static final float NaN = (float)(0.0 / 0.0);
  public static final float MAX_VALUE = (float)3.40282346638528860e+38;
  //public static final float MIN_VALUE = (float)1.40129846432481707e-45;
  public static final float MIN_VALUE = (float)1.40129846432481707e-38;
  public static final Class TYPE = Class.getPrimitiveClass("float");
  
  public Float(float value) {
    this.value=value;
  }
  
  public Float(double value) {
    this((float )value);
  }
  
  public Float(String s) throws NumberFormatException {
    this(Float.valueOf(s).floatValue());
  }
  
  public boolean equals(Object obj) {
    if ((obj!=null) && (obj instanceof Float)) {

      int leftBits=floatToIntBits(this.floatValue());
      int rightBits=floatToIntBits(((Float )obj).floatValue());
      int nanBits=floatToIntBits(NaN);
      int posBits=floatToIntBits(POSITIVE_INFINITY);
      int negBits=floatToIntBits(NEGATIVE_INFINITY);

      if ((this.isNaN()==true) && (((Float )obj).isNaN()==true)) {
        return true;
      }

      if (((leftBits==posBits) && (rightBits==negBits)) || ((leftBits==posBits) && (rightBits==negBits))) {
	return true;
      }
    
      return (leftBits==rightBits);
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
  
  public byte byteValue()
  {
    return ((byte)value);
  }
  
  public short shortValue()
  {
    return ((short)value);
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
    /* See class Double */
    return (v!=v);
  }
  
  public boolean isNaN() {
    return isNaN(this.floatValue());
  }
  
  public static boolean isInfinite(float v) {
    return (floatToIntBits(v)==floatToIntBits(POSITIVE_INFINITY)) || (floatToIntBits(v)==floatToIntBits(NEGATIVE_INFINITY));
  }
  
  public boolean isInfinite() {
    return isInfinite(this.floatValue());
  }
}
