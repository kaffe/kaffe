
/**
 * Tests for static method calls.  Previously, native code had been calling the
 * functions, so this will be the first test of jitted code calling other
 * methods.  Note:  This will also be the first test of the trampolines.
 */
class StaticMethodCall
{
    public static int int_method_void()
    {
	return 0xDEADBEEF;
    }

    public static final int test__int_call_void = 0xDEADBEEF;

    public static int int_call_void()
    {
	return int_method_void();
    }
    
    public static int int_method_int(int a)
    {
	return a;
    }

    public static final int test_1__int_call_int = 1;
    
    public static int int_call_int(int a)
    {
	return int_method_int(a);
    }
    
    public static int int_method_int_int(int a, int b)
    {
	return a + b;
    }

    public static final int test_1_2__int_call_int_int = 3;
    
    public static int int_call_int_int(int a, int b)
    {
	return int_method_int_int(a, b);
    }
    
    public static int int_method_int_int2(int a, int b)
    {
	return b;
    }

    public static final int test__int_call_int_int2 = 2;
    
    public static int int_call_int_int2()
    {
	return int_method_int_int2(1, 2);
    }
    
    public static float float_method_int_float(int a, float b)
    {
	return b;
    }

    public static final float test_1_2__float_call_int_float = 2.0F;
    public static final float test_2_3__float_call_int_float = 3.0F;
    
    public static float float_call_int_float(int a, float b)
    {
	return float_method_int_float(a, b);
    }
    
    public static double double_method_int_float_double_int(int a,
							    float b,
							    double c,
							    int d)
    {
	return c + (double)d;
    }

    public static final double test_4__double_call_int_float_double_int =
	7.0;
    
    public static double double_call_int_float_double_int(int d)
    {
	return double_method_int_float_double_int(1, 2.0F, 3.0, d);
    }
    
    private StaticMethodCall()
    {
    }
}
