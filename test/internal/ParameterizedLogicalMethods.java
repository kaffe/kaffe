
class ParameterizedLogicalMethods
{
    /*
     * These boolean operations come out as branches in bytecode, so they
     * should probably be moved to ControlFlow, but it would be nice to
     * optimize them away.
     */
    
    public static final boolean test_true__bool_not_bool = false;
    public static final boolean test_false__bool_not_bool = true;
    
    public static boolean bool_not_bool(boolean a)
    {
	return !a;
    }
    
    public static final boolean test_true_true__bool_and_bool_bool = true;
    public static final boolean test_true_false__bool_and_bool_bool = false;
    public static final boolean test_false_false__bool_and_bool_bool = false;
    public static final boolean test_false_true__bool_and_bool_bool = false;
    
    public static boolean bool_and_bool_bool(boolean a, boolean b)
    {
	return a && b;
    }
    
    public static final boolean test_true_true__bool_or_bool_bool = true;
    public static final boolean test_true_false__bool_or_bool_bool = true;
    public static final boolean test_false_false__bool_or_bool_bool = false;
    public static final boolean test_false_true__bool_or_bool_bool = true;
    
    public static boolean bool_or_bool_bool(boolean a, boolean b)
    {
	return a || b;
    }
    
    public static final boolean test_1__bool_lt_int = true;
    public static final boolean test_2__bool_lt_int = false;
    
    public static boolean bool_lt_int(int a)
    {
	return a < 2;
    }
    
    public static final boolean test_1_2__bool_lt_int_int = true;
    public static final boolean test_2_1__bool_lt_int_int = false;
    
    public static boolean bool_lt_int_int(int a, int b)
    {
	return a < b;
    }
    
    public static final boolean test_0__bool_le_int = true;
    public static final boolean test_2__bool_le_int = false;
    public static final boolean test_1__bool_le_int = true;
    
    public static boolean bool_le_int(int a)
    {
	return a <= 1;
    }
    
    public static final boolean test_1_2__bool_le_int_int = true;
    public static final boolean test_2_1__bool_le_int_int = false;
    public static final boolean test_1_1__bool_le_int_int = true;
    
    public static boolean bool_le_int_int(int a, int b)
    {
	return a <= b;
    }
    
    public static final boolean test_2__bool_gt_int = true;
    public static final boolean test_1__bool_gt_int = false;
    
    public static boolean bool_gt_int(int a)
    {
	return a > 1;
    }
    
    public static final boolean test_2_1__bool_gt_int_int = true;
    public static final boolean test_1_2__bool_gt_int_int = false;
    
    public static boolean bool_gt_int_int(int a, int b)
    {
	return a > b;
    }
    
    public static final boolean test_2__bool_ge_int = true;
    public static final boolean test_1__bool_ge_int = false;
    public static final boolean test_3__bool_ge_int = true;
    
    public static boolean bool_ge_int(int a)
    {
	return a >= 2;
    }
    
    public static final boolean test_2_1__bool_ge_int_int = true;
    public static final boolean test_1_2__bool_ge_int_int = false;
    public static final boolean test_1_1__bool_ge_int_int = true;
    
    public static boolean bool_ge_int_int(int a, int b)
    {
	return a >= b;
    }
    
    public static final boolean test_1__bool_eq_int = false;
    public static final boolean test_2__bool_eq_int = true;
    
    public static boolean bool_eq_int(int a)
    {
	return a == 2;
    }
    
    public static final boolean test_2_1__bool_eq_int_int = false;
    public static final boolean test_2_2__bool_eq_int_int = true;
    
    public static boolean bool_eq_int_int(int a, int b)
    {
	return a == b;
    }
    
    public static final boolean test_1__bool_neq_int = true;
    public static final boolean test_2__bool_neq_int = false;
    
    public static boolean bool_neq_int(int a)
    {
	return a != 2;
    }
    
    public static final boolean test_2_1__bool_neq_int_int = true;
    public static final boolean test_2_2__bool_neq_int_int = false;
    
    public static boolean bool_neq_int_int(int a, int b)
    {
	return a != b;
    }
    
    public static final int test_2__int_amp_int = 3;
    public static final int test_3__int_amp_int = 4;
    public static final int test_4__int_amp_int = 5;
    
    public static int int_amp_int(int a)
    {
	int retval;

	if( (a == 2) & (a++ == 4) )
	{
	    retval = 1;
	}
	else
	{
	    retval = a;
	}
	return retval;
    }
    
    public static final int test_2__int_bar_int = 3;
    public static final int test_3__int_bar_int = 4;
    public static final int test_4__int_bar_int = -1;
    
    public static int int_bar_int(int a)
    {
	int retval;

	if( (a == 2) | (a++ == 3) )
	{
	    retval = a;
	}
	else
	{
	    retval = -1;
	}
	return retval;
    }
    
    public static final int test_2__int_carat_int = -1;
    public static final int test_3__int_carat_int = -1;
    public static final int test_4__int_carat_int = 5;
    
    public static int int_carat_int(int a)
    {
	int retval;

	if( (a == 2) ^ ((a++ & 0x1) == 0) )
	{
	    retval = a;
	}
	else
	{
	    retval = -1;
	}
	return retval;
    }
    
    private ParameterizedLogicalMethods()
    {
    }
}
