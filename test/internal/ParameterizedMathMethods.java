
/**
 * Tests for simple math operations.
 */
class ParameterizedMathMethods
{
    public static final int test_2__int_postinc_int = 2;
    public static final int test_3__int_postinc_int = 3;
    
    public static int int_postinc_int(int a)
    {
	return a++;
    }
    
    public static final int test_2__int_postdec_int = 2;
    public static final int test_3__int_postdec_int = 3;
    
    public static int int_postdec_int(int a)
    {
	return a--;
    }
    
    public static final int test_2__int_preinc_int = 3;
    public static final int test_3__int_preinc_int = 4;
    
    public static int int_preinc_int(int a)
    {
	return ++a;
    }
    
    public static final int test_2__int_predec_int = 1;
    public static final int test_3__int_predec_int = 2;
    
    public static int int_predec_int(int a)
    {
	return --a;
    }
    
    public static final int test_2__int_neg_int = -2;
    public static final int test_0xFFFFFFFE__int_neg_int = 2;
    
    public static int int_neg_int(int a)
    {
	return -a;
    }
    
    public static final int test_2__int_add_int0 = 4;
    public static final int test_128__int_add_int0 = 130;
    
    public static final int test_2__int_add_int1 = 2 + 255;
    public static final int test_128__int_add_int1 = 128 + 255;
    
    public static final int test_128__int_add_int2 = 128 + 32768;
    
    public static int int_add_int0(int a)
    {
	return a + 2;
    }
    
    public static int int_add_int1(int a)
    {
	return a + 255;
    }
    
    public static int int_add_int2(int a)
    {
	return a + 32768;
    }
    
    public static final int test_2__int_sub_int0 = 0;
    public static final int test_128__int_sub_int0 = 126;
    
    public static final int test_2__int_sub_int1 = 2 - 255;
    public static final int test_128__int_sub_int1 = 128 - 255;
    
    public static final int test_128__int_sub_int2 = 128 - 32768;
    
    public static int int_sub_int0(int a)
    {
	return a - 2;
    }
    
    public static int int_sub_int1(int a)
    {
	return a - 255;
    }
    
    public static int int_sub_int2(int a)
    {
	return a - 32768;
    }
    
    public static final int test_2_2__int_add_int_int = 4;
    public static final int test_128_64__int_add_int_int = 128 + 64;
    
    public static int int_add_int_int(int a, int b)
    {
	return a + b;
    }
    
    public static final int test_2_2__int_sub_int_int = 0;
    public static final int test_128_64__int_sub_int_int = 128 - 64;
    
    public static int int_sub_int_int(int a, int b)
    {
	return a - b;
    }
    
    public static final int test_2_2__int_div_int_int = 1;
    public static final int test_128_64__int_div_int_int = 2;
    
    public static int int_div_int_int(int a, int b)
    {
	return a / b;
    }
    
    public static final int test_2_2__int_mul_int_int = 4;
    public static final int test_128_64__int_mul_int_int = 128 * 64;
    
    public static int int_mul_int_int(int a, int b)
    {
	return a * b;
    }
    
    public static final int test_2_2__int_mod_int_int = 0;
    public static final int test_128_63__int_mod_int_int = 128 % 63;
    
    public static int int_mod_int_int(int a, int b)
    {
	return a % b;
    }
    
    public static final float test_2__float_neg_float = -2;
    
    public static float float_neg_float(float a)
    {
	return -a;
    }
    
    public static final float test_2__float_add_float0 = 4;
    public static final float test_128__float_add_float0 = 130;
    
    public static final float test_2__float_add_float1 = 2 + 255;
    public static final float test_128__float_add_float1 = 128 + 255;
    
    public static final float test_128__float_add_float2 = 128 + 32768;
    
    public static float float_add_float0(float a)
    {
	return a + 2;
    }
    
    public static float float_add_float1(float a)
    {
	return a + 255;
    }
    
    public static float float_add_float2(float a)
    {
	return a + 32768;
    }
    
    public static final float test_2__float_sub_float0 = 0;
    public static final float test_128__float_sub_float0 = 126;
    
    public static final float test_2__float_sub_float1 = 2 - 255;
    public static final float test_128__float_sub_float1 = 128 - 255;
    
    public static final float test_128__float_sub_float2 = 128 - 32768;
    
    public static float float_sub_float0(float a)
    {
	return a - 2;
    }
    
    public static float float_sub_float1(float a)
    {
	return a - 255;
    }
    
    public static float float_sub_float2(float a)
    {
	return a - 32768;
    }

    public static final float test_2_2__float_sub_float_float = 0;
    public static final float test_6_2__float_sub_float_float = 4;
    public static final float test_128_40__float_sub_float_float = 88;

    public static float float_sub_float_float(float a, float b)
    {
	return a - b;
    }
    
    public static final float test_2_2__float_div_float_float = 1;
    public static final float test_128_64__float_div_float_float = 2;
    
    public static float float_div_float_float(float a, float b)
    {
	return a / b;
    }
    
    public static final float test_2_2__float_mul_float_float = 4;
    public static final float test_128_64__float_mul_float_float = 128 * 64;
    
    public static float float_mul_float_float(float a, float b)
    {
	return a * b;
    }

    /*
    public static final float test_2_2__float_mod_float_float = 0;
    public static final float test_128_63__float_mod_float_float = 128 % 63;
    
    public static float float_mod_float_float(float a, float b)
    {
	return a % b;
    }
    */

    public static final double test_2__double_neg_double = -2;
    
    public static double double_neg_double(double a)
    {
	return -a;
    }
        
    public static final double test_2__double_add_double0 = 4;
    public static final double test_128__double_add_double0 = 130;
    
    public static final double test_2__double_add_double1 = 2 + 255;
    public static final double test_128__double_add_double1 = 128 + 255;
    
    public static final double test_128__double_add_double2 = 128 + 32768;
    
    public static double double_add_double0(double a)
    {
	return a + 2;
    }
    
    public static double double_add_double1(double a)
    {
	return a + 255;
    }
    
    public static double double_add_double2(double a)
    {
	return a + 32768;
    }
    
    public static final double test_2__double_sub_double0 = 0;
    public static final double test_128__double_sub_double0 = 126;
    
    public static final double test_2__double_sub_double1 = 2 - 255;
    public static final double test_128__double_sub_double1 = 128 - 255;
    
    public static final double test_128__double_sub_double2 = 128 - 32768;
    
    public static double double_sub_double0(double a)
    {
	return a - 2;
    }
    
    public static double double_sub_double1(double a)
    {
	return a - 255;
    }
    
    public static double double_sub_double2(double a)
    {
	return a - 32768;
    }
    
    public static final double test_2_2__double_sub_double_double = 0;
    public static final double test_6_2__double_sub_double_double = 4;
    public static final double test_128_40__double_sub_double_double = 88;

    public static double double_sub_double_double(double a, double b)
    {
	return a - b;
    }
    
    public static final double test_2_2__double_div_double_double = 1;
    public static final double test_128_64__double_div_double_double = 2;
    
    public static double double_div_double_double(double a, double b)
    {
	return a / b;
    }
    
    public static final double test_2_2__double_mul_double_double = 4;
    public static final double test_128_64__double_mul_double_double = 128 * 64;
    
    public static double double_mul_double_double(double a, double b)
    {
	return a * b;
    }

    /*
    public static final double test_2_2__double_mod_double_double = 0;
    public static final double test_128_63__double_mod_double_double = 128 % 63;
    
    public static double double_mod_double_double(double a, double b)
    {
	return a % b;
    }
    */
    
    private ParameterizedMathMethods()
    {
    }
}
