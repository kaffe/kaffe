
/**
 * Tests for simple parameter passing.
 */
class ParameterizedMethods
{

    /* Basic in and out parameter passing. */
    
    public static final int test_0x00000000__int_method_int = 0;
    public static final int test_0x00000001__int_method_int = 1;
    public static final int test_0xdeadbeef__int_method_int = 0xdeadbeef;
    
    public static int int_method_int(int a)
    {
	return a;
    }
    
    public static final short test_0x0000__short_method_short = 0;
    public static final short test_0x0001__short_method_short = 1;
    public static final short test_0x7eef__short_method_short = 0x7eef;
    
    public static short short_method_short(short a)
    {
	return a;
    }

    public static final long test_0x0000000000000000__long_method_long = 0L;
    public static final long test_0x0000000000000001__long_method_long = 1L;
    public static final long test_0xdeadbeefd0decade__long_method_long =
    0xdeadbeefd0decadeL;
    
    public static long long_method_long(long a)
    {
	return a;
    }
    
    public static final float test_0__float_method_float = 0;
    public static final float test_1__float_method_float = 1;
    public static final float test_100000__float_method_float = 100000;
    public static final float test_123d456__float_method_float = 123.456F;

    public static float float_method_float(float a)
    {
	return a;
    }
    
    public static final double test_0__double_method_double = 0;
    public static final double test_1__double_method_double = 1;
    public static final double test_100000__double_method_double = 100000;
    public static final double test_123d456__double_method_double = 123.456D;

    public static double double_method_double(double a)
    {
	return a;
    }

    /*
     * Mixed arguments, mostly a challenge for register rich CPUs.  Errors here
     * may also be due to a broken sysdepCallMethod.
     */
    
    public static final int test_1_2_3___int_method_int_float_int = 3;

    public static int int_method_int_float_int(int a, float b, int c)
    {
	return c;
    }

    public static final int test_1_2_3___int_method_int_double_int = 3;

    public static int int_method_int_double_int(int a, double b, int c)
    {
	return c;
    }

    public static final int test_1_2_3_4__int_method_int_float_double_int = 4;

    public static int int_method_int_float_double_int(int a,
						      float b,
						      double c,
						      int d)
    {
	return d;
    }

    public static final float
	test_1_2_3_4__float_method_int_float_double_int = 2.0F;

    public static float float_method_int_float_double_int(int a,
							  float b,
							  double c,
							  int d)
    {
	return b;
    }

    public static final double
	test_1_2_3_4__double_method_int_float_double_int = 3.0;

    public static double double_method_int_float_double_int(int a,
							    float b,
							    double c,
							    int d)
    {
	return c;
    }


    /* Test long parameter lists */
    
    public static final int test_0_1_2_3__int_method3 = 3;
    
    public static int int_method3(int a, int b, int c, int d)
    {
	return d;
    }
    
    public static final int test_0_1_2_3_4__int_method4 = 4;
    
    public static int int_method4(int a, int b, int c, int d, int e)
    {
	return e;
    }
    
    public static final int test_0_1_2_3_4_5__int_method5 = 5;
    
    public static int int_method5(int a, int b, int c, int d, int e, int f)
    {
	return f;
    }
    
    public static final int test_0_1_2_3_4_5_6__int_method6 = 6;
    
    public static int int_method6(int a, int b, int c, int d, int e, int f,
				  int g)
    {
	return g;
    }
    
    public static final int test_0_1_2_3_4_5_6_7__int_method7 = 7;
    
    public static int int_method7(int a, int b, int c, int d, int e, int f,
				  int g, int h)
    {
	return h;
    }

    /*
    public static final int test_0_1_2_3_4_5_6_7_8__int_method8 = 8;
    
    public static int int_method8(int a, int b, int c, int d, int e, int f,
				  int g, int h, int i)
    {
	return i;
    }
    
    public static final int test_0_1_2_3_4_5_6_7_8_9__int_method9 = 9;
    
    public static int int_method9(int a, int b, int c, int d, int e, int f,
				  int g, int h, int i, int j)
    {
	return j;
    }
    
    public static final int test_0_1_2_3_4_5_6_7_8_9_10__int_method10 = 10;
    
    public static int int_method10(int a, int b, int c, int d, int e, int f,
				   int g, int h, int i, int j, int k)
    {
	return k;
    }
    
    public static final int test_0_1_2_3_4_5_6_7_8_9_10_11__int_method11 = 11;
    
    public static int int_method11(int a, int b, int c, int d, int e, int f,
				   int g, int h, int i, int j, int k, int l)
    {
	return l;
    }
    */
    
    private ParameterizedMethods()
    {
    }
}
