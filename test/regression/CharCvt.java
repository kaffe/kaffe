
import kaffe.io.ByteToCharIconv;
import kaffe.io.CharToByteIconv;

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
	throws Throwable
    {
	print_as_int(make_a_char(-1));
	print_as_int(make_a_short(-1));

	/* Make sure CharToByteIconv does the right thing. */
	try
	{
	    CharToByteIconv ic;

	    ic = new CharToByteIconv("SHIFT_JIS");
	    ic.convert(new char[0], 0, 0, new byte[0], 0, 0);
	}
	catch(java.io.UnsupportedEncodingException th)
	{
	}
	/* Make sure ByteToCharIconv does the right thing. */
	try
	{
	    ByteToCharIconv ic;

	    ic = new ByteToCharIconv("SHIFT_JIS");
	    ic.convert(new byte[0], 0, 0, new char[0], 0, 0);
	}
	catch(java.io.UnsupportedEncodingException th)
	{
	}
    }
}

/* Expected Output:
65535
-1
*/
