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
  public static final double MAX_VALUE = 1.79769313486231570E+308;
  public static final double MIN_VALUE = 2.2250738585072014E-308;
  public static final Class TYPE = Class.getPrimitiveClass("double");

  private final double value;

  public static native String toString(double d);
  public static native Double valueOf(String s) throws NumberFormatException;
  public static native long doubleToLongBits(double value);
  public static native double longBitsToDouble(long bits);
  
  public Double(double value) {
    this.value = value;
  }

  public Double(String s) throws NumberFormatException {
    this.value = valueOf(s).value;
  }

  public double doubleValue() {
    return value;
  }

  public String toString() {
    return toString(value);
  }

  public boolean equals(Object obj) {
    if ( (obj!=null) && (obj instanceof Double)) {
      Double that=(Double )obj;
      if ((this.isNaN()==true) && (that.isNaN()==true)) return true;
    
      double left = this.value;
      double right = that.value;

      if ((left==+0.0) && (right==-0.0)) return false;
      if ((left==-0.0) && (right==+0.0)) return false;

      return (left==right);
    }
    else {
      return false;
    }
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
    /* A NaN is the only number which doesn't equal itself */
    return (v != v);
  }

  public long longValue() {
    return (long) value;
  }
}

