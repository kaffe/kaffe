/**
 * Ensure that Double and Float compile time constants are correctly
 * compiled.
 *
 * @author: Edouard G. Parmelan <egp@free.fr> */
public class DoubleConst
{
    static void test (String title, float value, int expect)
    {
	int got = Float.floatToIntBits (value);

	if (got != expect) {
	    System.out.println (title
				+ " expect " + Integer.toString (expect, 16)
				+ " got " + Integer.toString (got, 16)
				);
	}
    }

    static void test (String title, double value, long expect)
    {
	long got = Double.doubleToLongBits (value);

	if (got != expect) {
	    System.out.println (title
				+ " expect " + Long.toString (expect, 16)
				+ " got " + Long.toString (got, 16)
				);
	}
    }

    public static void main (String args[])
    {
	System.out.println("Float tests:");
	test ("POSITIVE_INTINITY", Float.POSITIVE_INFINITY, 0x7f800000);
	test ("NEGATIVE_INTINITY", Float.NEGATIVE_INFINITY, 0xff800000);
	test ("NaN", Float.NaN, 0x7fc00000);
	test ("MIN_VALUE", Float.MIN_VALUE, 0x00000001);
	test ("MAX_VALUE", Float.MAX_VALUE, 0x7f7fffff);

	System.out.println("Double tests:");
	test ("POSITIVE_INTINITY", Double.POSITIVE_INFINITY, 0x7ff0000000000000L);
	test ("NEGATIVE_INTINITY", Double.NEGATIVE_INFINITY, 0xfff0000000000000L);
	test ("NaN", Double.NaN, 0x7ff8000000000000L);
	test ("MIN_VALUE", Double.MIN_VALUE, 0x0000000000000001L);
	test ("MAX_VALUE", Double.MAX_VALUE, 0x7fefffffffffffffL);

    }
}

/* Expected output:
Float tests:
Double tests:
*/
