import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;

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
	CharsetEncoder ce;
	CharBuffer cbuf;

	ce = Charset.forName("SHIFT_JIS").newEncoder();
	cbuf = CharBuffer.allocate(0);
	ce.encode(cbuf);

	/* Make sure ByteToCharIconv does the right thing. */
	CharsetDecoder cd;
	ByteBuffer bbuf;

	cd = Charset.forName("SHIFT_JIS").newDecoder();
	bbuf = ByteBuffer.allocate(0);
	cd.decode(bbuf);
    }
}

// unsafe

/* java args: CharCvt -Dgnu.classpath.nio.charset.provider.iconv=true
Expected Output:
65535
-1
*/
