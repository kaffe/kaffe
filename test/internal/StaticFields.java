
class StaticFields
{
    /* Force a <clinit> method. */
    public static int int_field0 = 0;
    public static int int_field1 = 0;

    public static long long_field0;
    public static long long_field1;

    public static float float_field0;
    public static float float_field1;

    public static double double_field0;
    public static double double_field1;

    public static final int test_1__exchange_int0 = 0;
    public static final int test_2__exchange_int0 = 1;
    
    public static int exchange_int0(int a)
    {
	int retval;

	retval = int_field0;
	int_field0 = a;
	return retval;
    }
    
    public static final long test_1__exchange_long0 = 0L;
    public static final long test_2__exchange_long0 = 1L;
    
    public static long exchange_long0(long a)
    {
	long retval;

	retval = long_field0;
	long_field0 = a;
	return retval;
    }
    
    public static final float test_1__exchange_float0 = 0.0F;
    public static final float test_2__exchange_float0 = 1.0F;
    
    public static float exchange_float0(float a)
    {
	float retval;

	retval = float_field0;
	float_field0 = a;
	return retval;
    }
    
    public static final double test_1__exchange_double0 = 0.0;
    public static final double test_2__exchange_double0 = 1.0;
    
    public static double exchange_double0(double a)
    {
	double retval;

	retval = double_field0;
	double_field0 = a;
	return retval;
    }

    private StaticFields()
    {
    }
}
