/* test that we don't get tripped up by x86 problem */

public class divtest {

    public static int divfunc(int x, int y) {
      return x/y;
    }

    public static int modfunc(int x, int y) {
      return x%y;
    }

    public static void test(String s, int x, int y) {
	int r;

	try {
	    r = divfunc(x, y);
	    System.out.println (s + " / " + Integer.toHexString(r));
	}
	catch (Throwable t) {
	    System.out.println (s + " / " + t.toString());
	}

	try {
	    r = modfunc(x, y);
	    System.out.println (s + " % " + Integer.toHexString(r));
	}
	catch (Throwable t) {
	    System.out.println (s + " % " + t.toString());
	}
    }

    public static void main(String args[]) {
      test ("40000000 1", 0x40000000, 1);
      test ("80000000 1", 0x80000000, 1);

      test ("40000000 -1", 0x40000000, -1);
      test ("80000000 -1", 0x80000000, -1);	// x86 causes trap for no good reason

      test ("40000000 2", 0x40000000, 2);
      test ("80000000 2", 0x80000000, 2);

      test ("40000000 -2", 0x40000000, -2);
      test ("80000000 -2", 0x80000000, -2);
    }
}
/* Expected Output:
40000000 1 / 40000000
40000000 1 % 0
80000000 1 / 80000000
80000000 1 % 0
40000000 -1 / c0000000
40000000 -1 % 0
80000000 -1 / 80000000
80000000 -1 % 0
40000000 2 / 20000000
40000000 2 % 0
80000000 2 / c0000000
80000000 2 % 0
40000000 -2 / e0000000
40000000 -2 % 0
80000000 -2 / 40000000
80000000 -2 % 0
*/
