import java.lang.*;

class TestIntLong {
	static boolean failed = false;

        static void test(String s, String r) {
		if (!s.equals(r)) {
			System.out.println("result "+s+" should be "+r);
			failed = true;
		}
        }

	static void test(long v, String r) {
                test(Long.toString(v, 10), r);
	}

	static void test(int v, String r) {
                test(Long.toString(v, 10), r);
	}

	public static void main(String args[]) {
		System.out.println("Some mathmatical tests");
		long l1 = 500000l;
		long l2 = 400000l;
		test(l1 * l2, "200000000000");
		test(l1 >> 4, "31250");
		test(l2 << 3, "3200000");
		int i1 = 5000;
		int i2 = 4000;
		test(i1 * i2, "20000000");
		test(i1 >> 4, "312");
		test(i2 << 3, "32000");
		String s = "Good";
		if (failed)
			s = "Bad";
		System.out.println("Results "+s);
	}
}

/* Expected Output:
Some mathmatical tests
Results Good
*/
