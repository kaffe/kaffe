
/**
 * Test math operations with constants, in reality, _something_ should do
 * constant folding and everything should fall away.  This is just to make
 * absolutely sure it does happen.
 */
class ConstMathMethods
{
    public static final int test__int_add_void0 = 2;
    public static final int test__int_add_void1 = 0xDEADFEEB;
    
    public static int int_add_void0()
    {
	return 1 + 1;
    }

    public static int int_add_void1()
    {
	return 0xdead0000 + 0xfeeb;
    }

    public static final int test__int_sub_void0 = 0;
    public static final int test__int_sub_void1 = 0xDEAD0000;

    public static int int_sub_void0()
    {
	return 1 - 1;
    }
    
    public static int int_sub_void1()
    {
	return 0xdeadFFFF - 0x0000FFFF;
    }

    public static final int test__int_mul_void0 = 4;
    public static final int test__int_mul_void1 = 0;
    
    public static int int_mul_void0()
    {
	return 2 * 2;
    }
    
    public static int int_mul_void1()
    {
	return 2 * 0;
    }

    public static final int test__int_div_void0 = 2;
    public static final int test__int_div_void1 = 4;
    
    public static int int_div_void0()
    {
	return 4 / 2;
    }

    public static int int_div_void1()
    {
	return 4 / 1;
    }

    public static final boolean test__boolean_lt_void0 = false;
    public static final boolean test__boolean_lt_void1 = true;

    public static boolean boolean_lt_void0()
    {
	return 4 < 1;
    }

    public static boolean boolean_lt_void1()
    {
	return 1 < 4;
    }

    public static final boolean test__boolean_le_void0 = false;
    public static final boolean test__boolean_le_void1 = true;
    public static final boolean test__boolean_le_void2 = true;

    public static boolean boolean_le_void0()
    {
	return 4 <= 1;
    }

    public static boolean boolean_le_void1()
    {
	return 1 <= 4;
    }

    public static boolean boolean_le_void2()
    {
	return 1 <= 1;
    }

    public static final boolean test__boolean_gt_void0 = false;
    public static final boolean test__boolean_gt_void1 = true;

    public static boolean boolean_gt_void0()
    {
	return 1 > 4;
    }

    public static boolean boolean_gt_void1()
    {
	return 4 > 1;
    }

    public static final boolean test__boolean_ge_void0 = false;
    public static final boolean test__boolean_ge_void1 = true;
    public static final boolean test__boolean_ge_void2 = true;

    public static boolean boolean_ge_void0()
    {
	return 1 >= 4;
    }

    public static boolean boolean_ge_void1()
    {
	return 4 >= 1;
    }

    public static boolean boolean_ge_void2()
    {
	return 1 >= 1;
    }

    public static final boolean test__boolean_eq_void0 = false;
    public static final boolean test__boolean_eq_void1 = true;

    public static boolean boolean_eq_void0()
    {
	return 1 == 4;
    }

    public static boolean boolean_eq_void1()
    {
	return 1 == 1;
    }

    public static final boolean test__boolean_neq_void0 = false;
    public static final boolean test__boolean_neq_void1 = true;

    public static boolean boolean_neq_void0()
    {
	return 1 != 1;
    }

    public static boolean boolean_neq_void1()
    {
	return 1 != 4;
    }

    public static final boolean test__boolean_not_void0 = false;
    public static final boolean test__boolean_not_void1 = true;

    public static boolean boolean_not_void0()
    {
	return !true;
    }

    public static boolean boolean_not_void1()
    {
	return !false;
    }

    private ConstMathMethods()
    {
    }
}
