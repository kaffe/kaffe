/**
 * Test case for PR#601, PR#620 and PR#737.
 *
 * On ix86, FPU register is a stack.
 * When a FPU return value is ignored, JIT _must_ remove the result from the
 * FPU Stack.
 * When a FPU return value is store in a local variable, but JIT remove this
 * store, JIT _must_ remove the result from the FPU Stack.
 *
 * @author Edouard G. Parmelan <egp@free.fr>
 */
public class FPUStack {
    private int run = 0;
    private double total;

    public boolean can_stop() {
	return (++run % 5) == 0;
    }

    double stop() {
	total = (long)run * 1.0;
	return total;
    }

    double read() {
	return total;
    }

    float fstop() {
	return (float)stop();
    }

    float fread() {
	return (float)read();
    }

    public double integrate(int Num_samples) {
	return (long)run * 0.0001;
    }

    public float fintegrate(int Num_samples) {
	return (float)integrate(Num_samples);
    }

    private static void test_bad() {
	FPUStack Q = new FPUStack();

	System.out.println("test_bad()");
	while (true) {
	    // JIT _must_ remove ignored return value from stack.
	    Q.integrate(1);
	    Q.stop();
	    System.out.println ("[1] Q.read() " + Q.read());
	    System.out.println ("[2] Q.read() " + Q.read());
	    if (Q.can_stop()) break;
	}
    }

    private static void ftest_bad() {
	FPUStack Q = new FPUStack();

	System.out.println("ftest_bad()");
	while (true) {
	    // JIT _must_ remove ignored return value from stack.
	    Q.fintegrate(1);
	    Q.fstop();
	    System.out.println ("[1] Q.fread() " + Q.fread());
	    System.out.println ("[2] Q.fread() " + Q.fread());
	    if (Q.can_stop()) break;
	}
    }

    private static void test_good() {
	FPUStack Q = new FPUStack();

	System.out.println("test_good()");
	while (true) {
	    // FPU return values are store and store call is not elimitated
	    double integrate = Q.integrate(1);
	    double stop = Q.stop();
	    System.out.println ("[1] Q.read() " + Q.read());
	    System.out.println ("[2] Q.read() " + Q.read());
	    if (Q.can_stop()) break;
	}
    }

    private static void ftest_good() {
	FPUStack Q = new FPUStack();

	System.out.println("ftest_good()");
	while (true) {
	    // FPU return values are store and store call is not elimitated
	    float fintegrate = Q.fintegrate(1);
	    float fstop = Q.fstop();
	    System.out.println ("[1] Q.fread() " + Q.fread());
	    System.out.println ("[2] Q.fread() " + Q.fread());
	    if (Q.can_stop()) break;
	}
    }

    private static void test_bad_inline() {
	FPUStack Q = new FPUStack();

	System.out.println("test_bad_inline()");
	// JIT _must_ remove ignored return value from stack.
	Q.integrate(1);
	Q.stop();
	System.out.println ("[1] Q.read() " + Q.read());
	System.out.println ("[2] Q.read() " + Q.read());
    }

    private static void ftest_bad_inline() {
	FPUStack Q = new FPUStack();

	System.out.println("ftest_bad_inline()");
	// JIT _must_ remove ignored return value from stack.
	Q.fintegrate(1);
	Q.fstop();
	System.out.println ("[1] Q.fread() " + Q.fread());
	System.out.println ("[2] Q.fread() " + Q.fread());
    }

    private static void test_good_inline() {
	FPUStack Q = new FPUStack();

	System.out.println("test_good_inline()");
	// FPU return values are store but store call is elimitated,
	// JIT _must_ remove the result from the FPU Stack.
	double integrate = Q.integrate(1);
	double stop = Q.stop();
	System.out.println ("[1] Q.read() " + Q.read());
	System.out.println ("[2] Q.read() " + Q.read());
    }

    private static void ftest_good_inline() {
	FPUStack Q = new FPUStack();

	System.out.println("ftest_good_inline()");
	// FPU return values are store but store call is elimitated,
	// JIT _must_ remove the result from the FPU Stack.
	float fintegrate = Q.fintegrate(1);
	float fstop = Q.fstop();
	System.out.println ("[1] Q.fread() " + Q.fread());
	System.out.println ("[2] Q.fread() " + Q.fread());
    }

    
    public static double dstore(double a) {
	// Test case for PR#737
	// Author: Stas Sergeev <stas.orel@mailcity.com>
	double x0;
	double deltaX = 1.0;
	double deltaY = 1.0;
	double eps = 0.01;

        x0 = a - eps;
        deltaX = 2 * eps;
        return deltaX;
    }

    public static void dstore_test() {
	System.out.println("dstore_test()");
	for (int i = 0; i < 5; i++) {
	    System.out.println(dstore(0));
	}
    }


    public static float fstore(float a) {
	// Test case for PR#737
	// Author: Stas Sergeev <stas.orel@mailcity.com>
	float x0;
	float deltaX = 1.0f;
	float deltaY = 1.0f;
	float eps = 0.01f;

        x0 = a - eps;
        deltaX = 2 * eps;
        return deltaX;
    }

    public static void fstore_test() {
	System.out.println("fstore_test()");
	for (int i = 0; i < 5; i++) {
	    System.out.println(fstore(0));
	}
    }


    public static void main (String[] args) {
	test_good();
	test_bad();

	test_good_inline();
	test_good_inline();
	test_good_inline();
	test_good_inline();
	test_good_inline();
	test_bad_inline();
	test_bad_inline();
	test_bad_inline();
	test_bad_inline();
	test_bad_inline();

	ftest_good();
	ftest_bad();

	ftest_good_inline();
	ftest_good_inline();
	ftest_good_inline();
	ftest_good_inline();
	ftest_good_inline();
	ftest_bad_inline();
	ftest_bad_inline();
	ftest_bad_inline();
	ftest_bad_inline();
	ftest_bad_inline();

	dstore_test();
	fstore_test();
    }
}

/* Expected Output:
test_good()
[1] Q.read() 0.0
[2] Q.read() 0.0
[1] Q.read() 1.0
[2] Q.read() 1.0
[1] Q.read() 2.0
[2] Q.read() 2.0
[1] Q.read() 3.0
[2] Q.read() 3.0
[1] Q.read() 4.0
[2] Q.read() 4.0
test_bad()
[1] Q.read() 0.0
[2] Q.read() 0.0
[1] Q.read() 1.0
[2] Q.read() 1.0
[1] Q.read() 2.0
[2] Q.read() 2.0
[1] Q.read() 3.0
[2] Q.read() 3.0
[1] Q.read() 4.0
[2] Q.read() 4.0
test_good_inline()
[1] Q.read() 0.0
[2] Q.read() 0.0
test_good_inline()
[1] Q.read() 0.0
[2] Q.read() 0.0
test_good_inline()
[1] Q.read() 0.0
[2] Q.read() 0.0
test_good_inline()
[1] Q.read() 0.0
[2] Q.read() 0.0
test_good_inline()
[1] Q.read() 0.0
[2] Q.read() 0.0
test_bad_inline()
[1] Q.read() 0.0
[2] Q.read() 0.0
test_bad_inline()
[1] Q.read() 0.0
[2] Q.read() 0.0
test_bad_inline()
[1] Q.read() 0.0
[2] Q.read() 0.0
test_bad_inline()
[1] Q.read() 0.0
[2] Q.read() 0.0
test_bad_inline()
[1] Q.read() 0.0
[2] Q.read() 0.0
ftest_good()
[1] Q.fread() 0.0
[2] Q.fread() 0.0
[1] Q.fread() 1.0
[2] Q.fread() 1.0
[1] Q.fread() 2.0
[2] Q.fread() 2.0
[1] Q.fread() 3.0
[2] Q.fread() 3.0
[1] Q.fread() 4.0
[2] Q.fread() 4.0
ftest_bad()
[1] Q.fread() 0.0
[2] Q.fread() 0.0
[1] Q.fread() 1.0
[2] Q.fread() 1.0
[1] Q.fread() 2.0
[2] Q.fread() 2.0
[1] Q.fread() 3.0
[2] Q.fread() 3.0
[1] Q.fread() 4.0
[2] Q.fread() 4.0
ftest_good_inline()
[1] Q.fread() 0.0
[2] Q.fread() 0.0
ftest_good_inline()
[1] Q.fread() 0.0
[2] Q.fread() 0.0
ftest_good_inline()
[1] Q.fread() 0.0
[2] Q.fread() 0.0
ftest_good_inline()
[1] Q.fread() 0.0
[2] Q.fread() 0.0
ftest_good_inline()
[1] Q.fread() 0.0
[2] Q.fread() 0.0
ftest_bad_inline()
[1] Q.fread() 0.0
[2] Q.fread() 0.0
ftest_bad_inline()
[1] Q.fread() 0.0
[2] Q.fread() 0.0
ftest_bad_inline()
[1] Q.fread() 0.0
[2] Q.fread() 0.0
ftest_bad_inline()
[1] Q.fread() 0.0
[2] Q.fread() 0.0
ftest_bad_inline()
[1] Q.fread() 0.0
[2] Q.fread() 0.0
dstore_test()
0.02
0.02
0.02
0.02
0.02
fstore_test()
0.02
0.02
0.02
0.02
0.02
*/
