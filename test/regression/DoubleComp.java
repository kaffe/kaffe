
public class DoubleComp {

  private static class TestCase {
    String arg1, arg2, expect;
    double val1, val2;
    TestCase(String arg1, String arg2, String expect) {
      this.arg1 = arg1;
      this.arg2 = arg2;
      this.expect = expect;
      val1 = cvt(arg1);
      val2 = cvt(arg2);
    }

    private static double cvt(String arg) {
      if (arg.equals("1.0"))
	return 1.0;
      else if (arg.equals("-1.0"))
	return -1.0;
      else if (arg.equals("0.0"))
	return pzero();
      else if (arg.equals("-0.0"))
	return nzero();
      else if (arg.equals("NaN"))
	return nan();
      else if (arg.equals("Inf"))
	return pinf();
      else if (arg.equals("-Inf"))
	return ninf();
      else
	throw new Error("unknown float " + arg);
    }
  }

  private static TestCase[] genCases() {
    return new TestCase[] {

      new TestCase("1.0", "NaN", "NaN"),
      new TestCase("NaN", "1.0", "NaN"),
      new TestCase("NaN", "0.0", "NaN"),

      new TestCase("Inf", "Inf", "NaN"),
      new TestCase("-Inf", "Inf", "NaN"),
      new TestCase("Inf", "-Inf", "NaN"),
      new TestCase("-Inf", "-Inf", "NaN"),

      new TestCase("-Inf", "1.0", "-Infinity"),
      new TestCase("Inf", "-0.0", "-Infinity"),
      new TestCase("-Inf", "0.0", "-Infinity"),
      new TestCase("-Inf", "-0.0", "Infinity"),

      new TestCase("1.0", "Inf", "0.0"),
      new TestCase("1.0", "-Inf", "-0.0"),
      new TestCase("-0.0", "Inf", "-0.0"),
      new TestCase("0.0", "-Inf", "-0.0"),
      new TestCase("-0.0", "-Inf", "0.0"),

      new TestCase("0.0", "0.0", "NaN"),
      new TestCase("0.0", "-0.0", "NaN"),
      new TestCase("-0.0", "-0.0", "NaN"),
      new TestCase("0.0", "-1.0", "-0.0"),
      new TestCase("-0.0", "1.0", "-0.0"),

      new TestCase("1.0", "0.0", "Infinity"),
      new TestCase("1.0", "-0.0", "-Infinity"),
      new TestCase("-1.0", "0.0", "-Infinity"),
      new TestCase("-1.0", "-0.0", "Infinity"),

    };
  }

  public static void main(String[] args) {
    TestCase[] tests = genCases();

    System.out.println("Double tests:");
    for (int k = 0; k < tests.length; k++) {
      String exp = tests[k].expect;
      String got;
      try { got = Double.toString(tests[k].val1 / tests[k].val2); }
      catch (Throwable t) { got = t.toString(); }

      if (!exp.equals(got))
	System.out.println("  FAIL"
	  + ": " + tests[k].arg1 + " / " + tests[k].arg2
	  + " got " + got + " but should be " + exp);
    }

    System.out.println("Float tests:");
    for (int k = 0; k < tests.length; k++) {
      String exp = tests[k].expect;
      String got;
      try { got = Float.toString((float) tests[k].val1
				 / (float) tests[k].val2); }
      catch (Throwable t) { got = t.toString(); }

      if (!exp.equals(got))
	System.out.println("  FAIL"
	  + ": " + tests[k].arg1 + " / " + tests[k].arg2
	  + " got " + got + " but should be " + exp);
    }
  }

  public static double pzero() {
    return Double.longBitsToDouble(0x0000000000000000L);
  }

  public static double nzero() {
    return Double.longBitsToDouble(0x8000000000000000L);
  }

  public static double pinf() {
    return Double.longBitsToDouble(0x7ff0000000000000L);
  }

  public static double ninf() {
    return Double.longBitsToDouble(0xfff0000000000000L);
  }

  public static double nan() {
    return Double.longBitsToDouble(0x7ff8000000000000L);
  }
}

/* Expected output:
Double tests:
Float tests:
*/
