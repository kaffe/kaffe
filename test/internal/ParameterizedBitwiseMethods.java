
/**
 * Tests for the bitwise operators, ~, &, |, ^, <<, >>, and >>>.
 */
class ParameterizedBitwiseMethods
{
    public static final int test_0xFFFF0000__int_not_int = 0x0000FFFF;
    public static final int test_0xF0F0F0F0__int_not_int = 0x0F0F0F0F;
    
    public static int int_not_int(int a)
    {
	return ~a;
    }
    
    public static final int test_0xDEADBEEF__int_and_int =
	0xDEAD0000;
    public static final int test_0xBEEFD0DE__int_and_int =
	0xBEEF0000;
    
    public static int int_and_int(int a)
    {
	return a & 0xFFFF0000;
    }
    
    public static final int test_0xDEADBEEF_0xFFFFFFFF__int_and_int_int =
	0xDEADBEEF;
    public static final int test_0xDEADBEEF_0x00000000__int_and_int_int =
	0x00000000;
    
    public static int int_and_int_int(int a, int b)
    {
	return a & b;
    }
    
    public static final int test_0xDEADBEEF__int_or_int = 0xFEFDFEFF;
    public static final int test_0x00000000__int_or_int = 0xF0F0F0F0;
    
    public static int int_or_int(int a)
    {
	return a | 0xF0F0F0F0;
    }
    
    public static final int test_0xDEADBEEF_0xFFFFFFFF__int_or_int_int =
	0xFFFFFFFF;
    public static final int test_0xDEADBEEF_0x00000000__int_or_int_int =
	0xDEADBEEF;
    
    public static int int_or_int_int(int a, int b)
    {
	return a | b;
    }
    
    public static final int test_0xDEADBEEF__int_xor_int =
	0xDEADBEEF ^ 0xFFFFFFFF;
    public static final int test_0x00000000__int_xor_int =
	0x00000000 ^ 0xFFFFFFFF;
    
    public static int int_xor_int(int a)
    {
	return a ^ 0xFFFFFFFF;
    }
    
    public static final int test_0xDEADBEEF_0xFFFFFFFF__int_xor_int_int =
	0xDEADBEEF ^ 0xFFFFFFFF;
    public static final int test_0xDEADBEEF_0x00000000__int_xor_int_int =
	0xDEADBEEF;
    
    public static int int_xor_int_int(int a, int b)
    {
	return a ^ b;
    }
    
    public static final int test_2__int_lshl_int = 2 << 2;
    public static final int test_128__int_lshl_int = 128 << 2;
    public static final int test_0x80000000__int_lshl_int =
    0x80000000 << 2;
    
    public static int int_lshl_int(int a)
    {
	return a << 2;
    }
    
    public static final int test_2_2__int_lshl_int_int = 2 << 2;
    public static final int test_128_28__int_lshl_int_int = 128 << 28;
    public static final int test_0x80000000_1__int_lshl_int_int =
    0x80000000 << 1;
    
    public static int int_lshl_int_int(int a, int b)
    {
	return a << b;
    }
    
    public static final int test_2__int_lshr_int = 2 >> 2;
    public static final int test_128__int_lshr_int = 128 >> 2;
    public static final int test_0x80000000__int_lshr_int =
	0x80000000 >> 2;
    
    public static int int_lshr_int(int a)
    {
	return a >> 2;
    }
    
    public static final int test_2_2__int_lshr_int_int = 2 >> 2;
    public static final int test_128_3__int_lshr_int_int = 128 >> 3;
    public static final int test_0x80000000_1__int_lshr_int_int =
	0x80000000 >> 1;
    public static final int test_0x40000000_1__int_lshr_int_int =
	0x40000000 >> 1;
    
    public static int int_lshr_int_int(int a, int b)
    {
	return a >> b;
    }
    
    public static final int test_2__int_ulshr_int = 2 >>> 2;
    public static final int test_128__int_ulshr_int = 128 >>> 2;
    public static final int test_0x80000000__int_ulshr_int =
	0x80000000 >>> 2;
    public static final int test_0x40000000__int_ulshr_int =
	0x40000000 >>> 2;
    
    public static int int_ulshr_int(int a)
    {
	return a >>> 2;
    }
    
    public static final int test_2_2__int_ulshr_int_int = 2 >>> 2;
    public static final int test_128_28__int_ulshr_int_int = 128 >>> 28;
    public static final int test_0x80000000_1__int_ulshr_int_int =
	0x80000000 >>> 1;
    public static final int test_0x40000000_1__int_ulshr_int_int =
	0x40000000 >>> 1;
    
    public static int int_ulshr_int_int(int a, int b)
    {
	return a >>> b;
    }
    
    private ParameterizedBitwiseMethods()
    {
    }
}
