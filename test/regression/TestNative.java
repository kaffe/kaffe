/**
 * This test requires native functions.
 * It tests that passing int, long, float, and double parameters works
 * properly.  See libraries/clib/native/TestNative.c
 *
 * This should be very helpful for porters to new architectures who want
 * to debug the sysdepCallMethod macro
 */
public class TestNative
{
    public static native int test16int(int a, int b, int c, int d,
				     int e, int f, int g, int h,
				     int i, int j, int k, int l,
				     int m, int n, int o, int p);

    public static native long test16long(long a, long b, long c, long d,
				     long e, long f, long g, long h,
				     long i, long j, long k, long l,
				     long m, long n, long o, long p);

    public static native long test16intlong(int a, long b, long c, long d,
				     long e, long f, long g, long h,
				     long i, int j, int k, long l,
				     long m, long n, long o, long p);

    public static native float test16float(float a, float b, float c, float d,
				     float e, float f, float g, float h,
				     float i, float j, float k, float l,
				     float m, float n, float o, float p);

    public static native double test16double(
				double a, double b, double c, double d,
				double e, double f, double g, double h,
				double i, double j, double k, double l,
				double m, double n, double o, double p);

    public static native double test16floatdouble(
				float a, double b, double c, double d,
				double e, double f, double g, double h,
				double i, float j, float k, double l,
				double m, double n, double o, double p);

    public static void main(String av[])
    {
if (true) {
	int i = test16int(0x11111111, 0x22222222, 0x33333333, 0x44444444,
			0x55555555, 0x66666666, 0x77777777, 0x88888888,
			0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC,
			0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF, 0x12345678
			);
	System.out.println("test16int returned " + i);

	long l = test16long(0x11111111, 0x22222222, 0x33333333, 0x44444444,
			0x55555555, 0x66666666, 0x77777777, 0x88888888,
			0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC,
			0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF, 0x12345678
			);
	System.out.println("test16long returned " + l);

	     l = test16intlong(0x11111111, 0x22222222, 0x33333333, 0x44444444,
			0x55555555, 0x66666666, 0x77777777, 0x88888888,
			0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC,
			0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF, 0x12345678
			);
	System.out.println("test16intlong returned " + l);
}
if (true) {
	float f = test16float(
	  -1.2f, 0.2f, 3.3f, 4.5f, 5.6f, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
			);
	System.out.println("test16float returned " + f);
}
	double d = test16double(
	  -1.2, 0.19, 3.3, 4.5, 5.6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
			);
	System.out.println("test16double returned " + d);

	d = test16floatdouble(
	  -1.2f, 0.2, 3.3, 4.5, 5.6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
			);
	System.out.println("test16floatdouble returned " + d);
    }
}
