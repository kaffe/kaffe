/**
 * this class tests some aspects of the conversion 
 * between ints, shorts, and chars
 */
public class CharCvt
{
    static void print_as_int(char x)
    {
	System.out.println((int)x);
    }

    static void print_as_int(short x)
    {
	System.out.println((int)x);
    }

    static char make_a_char(int x)
    {
	return (char)x;
    }

    static short make_a_short(int x)
    {
	return (short)x;
    }

    public static void main(String av[])
    {
	print_as_int(make_a_char(-1));
	print_as_int(make_a_short(-1));
    }
}

/* Expected Output:
65535
-1
*/
