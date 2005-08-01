public class VirtualMethod
{
	public static final int test_0__base_fact = 1;
	public static final int test_1__base_fact = 1;
	public static final int test_2__base_fact = 2;
	public static final int test_3__base_fact = 6;
	public static final int test_4__base_fact = 24;
	public static final int test_5__base_fact = 120;

	public int fact(int i)
	{
		if (i == 1 || i == 0)
			return 1;

		return fact(i-1) * i;
	}

	public static int base_fact(int i)
	{
		VirtualMethod m = new VirtualMethod();

		return m.fact(i);
	}
}
