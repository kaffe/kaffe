
public class NativeMethodCall
{
	public static int int_nmethod_void()
	{
		int a1[] = new int[] { 1, 2, 3, 4 };
		int a2[] = new int[a1.length];

		System.arraycopy(a1, 0, a2, 0, a1.length);
		
		return a2[2];
	}

	public static final int test__int_nmethod_void = 3;
}
