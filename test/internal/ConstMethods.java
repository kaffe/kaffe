
/**
 * Methods that return various constant values.  This verifies some really
 * basic functionality and should help get things going at first.
 *
 * XXX Might be a bit ppc specific.
 */
class ConstMethods
{
    public static final int test__int_method_void0 = 0x00000004;
    public static final int test__int_method_void1 = 0x0000ffff;
    public static final int test__int_method_void2 = 0x00007fff;
    public static final int test__int_method_void3 = 0xfffffff0;
    public static final int test__int_method_void4 = 0xcadef0f0;
    public static final int test__int_method_void5 = 0xcadee0f0;
    
    /**
     * @return a non-constpool immediate.  Should resolve to an ICONST_4
     * bytecode.
     */
    public static int int_method_void0()
    {
	return 0x00000004;
    }

    /**
     * @return an unsigned, at least, 16 bit constpool immediate.  This value
     * should not be sign extended when loaded as an immediate.
     */
    public static int int_method_void1()
    {
	return 0x0000ffff;
    }

    /**
     * @return a positive, at least, 16 bit constpool immediate.
     */
    public static int int_method_void2()
    {
	return 0x00007fff;
    }
    
    /**
     * @return a negative, at least, 16 bit constpool immediate.
     */
    public static int int_method_void3()
    {
	return 0xfffffff0;
    }

    /**
     * @return a signed 32 bit immediate.  The lower 16 bits is a negative
     * value so the sign extension should not damage the higher bits.
     */
    public static int int_method_void4()
    {
	return 0xcadef0f0;
    }

    /**
     * @return a signed 32 bit immediate.  Unlike the previous function, the
     * lower 16 bits are positive, so the lack of sign extension should not
     * damage the higher bits.
     */
    public static int int_method_void5()
    {
	return 0xcadee0f0;
    }

    public static final float test__float_method_void0 = 0.0F;
    public static final float test__float_method_void1 = 1.0F;
    public static final float test__float_method_void2 = 12345.6789F;

    public static float float_method_void0()
    {
	return 0.0F;
    }

    public static float float_method_void1()
    {
	return 1.0F;
    }

    public static float float_method_void2()
    {
	return 12345.6789F;
    }

    public static final double test__double_method_void0 = 0.0;
    public static final double test__double_method_void1 = 1.0;
    public static final double test__double_method_void2 = 12345.6789;

    public static double double_method_void0()
    {
	return 0.0;
    }

    public static double double_method_void1()
    {
	return 1.0;
    }

    public static double double_method_void2()
    {
	return 12345.6789;
    }

    public static final boolean test__boolean_method_void0 = false;
    public static final boolean test__boolean_method_void1 = true;

    public static boolean boolean_method_void0()
    {
	return false;
    }

    public static boolean boolean_method_void1()
    {
	return true;
    }

    public static final byte test__byte_method_void0 = 0;
    public static final byte test__byte_method_void1 = 1;
    public static final byte test__byte_method_void2 = 32;
    public static final byte test__byte_method_void3 = 127;
    public static final byte test__byte_method_void4 = -128;

    public static byte byte_method_void0()
    {
	return 0;
    }

    public static byte byte_method_void1()
    {
	return 1;
    }

    public static byte byte_method_void2()
    {
	return 32;
    }

    public static byte byte_method_void3()
    {
	return 127;
    }

    public static byte byte_method_void4()
    {
	return -128;
    }

    public static final char test__char_method_void0 = 0;
    public static final char test__char_method_void1 = 'a';
    public static final char test__char_method_void2 = 'A';
    public static final char test__char_method_void3 = 'z';
    public static final char test__char_method_void4 = 'Z';

    public static char char_method_void0()
    {
	return 0;
    }

    public static char char_method_void1()
    {
	return 'a';
    }

    public static char char_method_void2()
    {
	return 'A';
    }

    public static char char_method_void3()
    {
	return 'z';
    }

    public static char char_method_void4()
    {
	return 'Z';
    }

    public static final long test__long_method_void0 = 4L;
    public static final long test__long_method_void1 = 0x00000000FFFFFFFFL;
    public static final long test__long_method_void2 = 0xFFFFFFFF00000000L;
    public static final long test__long_method_void3 = 0x0000000FF0000000L;
    public static final long test__long_method_void4 = 0x0000000FFFFFFFF0L;
    public static final long test__long_method_void5 = 0x00000000EFFFFFFFL;
    public static final long test__long_method_void6 = 0xdeadbeefd0decadeL;

    public static long long_method_void0()
    {
	return 4L;
    }

    public static long long_method_void1()
    {
	return 0x00000000FFFFFFFFL;
    }

    public static long long_method_void2()
    {
	return 0xFFFFFFFF00000000L;
    }

    public static long long_method_void3()
    {
	return 0x0000000FF0000000L;
    }

    public static long long_method_void4()
    {
	return 0x0000000FFFFFFFF0L;
    }

    public static long long_method_void5()
    {
	return 0x00000000EFFFFFFFL;
    }

    public static long long_method_void6()
    {
	return 0xdeadbeefd0decadeL;
    }

    private ConstMethods()
    {
    }
}
