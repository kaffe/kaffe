class DoublePrint {

  static final double[] dcases = {
    0.0,
    -0.0,
    Double.longBitsToDouble(0x8000000000000000L),
    Double.NaN,
    Double.POSITIVE_INFINITY,
    Double.NEGATIVE_INFINITY,
    Double.MIN_VALUE,
    Double.MAX_VALUE,
    666666.5555555555555555,
    123.75,
    -0.9e7,
    1e-3,
    1.05e9,
  };

  static final float[] fcases = {
    0.0f,
    -0.0f,
    Float.intBitsToFloat(0x80000000),
    Float.NaN,
    Float.POSITIVE_INFINITY,
    Float.NEGATIVE_INFINITY,
    Float.MIN_VALUE,
    Float.MAX_VALUE,
    666666.5555555555555555f,
    123.75f,
    1e7f,
    -1e-3f,
  };

  public static void main(String[] args) {
    System.out.println("Double values:");
    System.out.println("  NaN -> " +
      Long.toHexString(
      Double.doubleToLongBits(
      Double.longBitsToDouble(0x7ff000abacab0000L))));
    for (int k = 0; k < dcases.length; k++) {
      System.out.println("  " + dcases[k]);
    }
    try {
	System.out.println("  0.0/0.0 = " + (dmeth() / dmeth()));
    }
    catch (ArithmeticException ae) {
	System.out.println("  0.0/0.0 = " + ae);
    }
    
    System.out.println();

    System.out.println("Float values:");
    System.out.println("  NaN -> " +
      Integer.toHexString(
      Float.floatToIntBits(
      Float.intBitsToFloat(0x7f80cb01))));
    for (int k = 0; k < fcases.length; k++) {
      System.out.println("  " + fcases[k]);
    }
    try {
	System.out.println("  0.0/0.0 = " + (fmeth() / fmeth()));
    }
    catch (ArithmeticException ae) {
	System.out.println("  0.0/0.0 = " + ae);
    }
  }

  public static double dmeth() {
    return 0.0;
  }

  public static float fmeth() {
    return 0.0f;
  }
}

/* Expected output:
Double values:
  NaN -> 7ff8000000000000
  0.0
  -0.0
  -0.0
  NaN
  Infinity
  -Infinity
  4.9E-324
  1.7976931348623157E308
  666666.5555555555
  123.75
  -9000000.0
  0.001
  1.05E9
  0.0/0.0 = NaN

Float values:
  NaN -> 7fc00000
  0.0
  -0.0
  -0.0
  NaN
  Infinity
  -Infinity
  1.4E-45
  3.4028235E38
  666666.56
  123.75
  1.0E7
  -0.001
  0.0/0.0 = NaN
*/

