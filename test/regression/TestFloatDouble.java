import java.lang.*;

class TestFloatDouble {
	static boolean failed = false;

        static void test(String s, String r) {
		if (!s.equals(r)) {
			System.out.println("result "+s+" should be "+r);
			failed = true;
		}
        }

	static void test(float v, String r) {
                test(Float.toString(v), r);
	}

	static void test(double v, String r) {
                test(Double.toString(v), r);
	}

	public static void main(String args[]) {
		System.out.println("Some mathmatical tests");
		float f1 = 500.005f;
		float f2 = 400.004f;
		test(f1 * f2, "200004");
		double d1 = 500.005;
		double d2 = 400.004;
		test(d1 * d2, "200004");
		String s = "Good";
		if (failed)
			s = "Bad";
		System.out.println("Results "+s);
	}
}

// Skip run
/* Expected Output:
Some mathmatical tests
Results Good
*/
