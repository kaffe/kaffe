import java.io.*;

public class MarkResetTest {
    public static void main(String av[]) throws Exception {

	// Test BufferedReader
	final int READLEN = 3;
	StringReader source = new StringReader ("0123456789ABC");
	BufferedReader br = new BufferedReader (source, 5);
	char buf[] = new char [READLEN];

	for (;;) {
	    br.mark(READLEN);
	    int charsRead = 0;
	    int len = 0;
	    while (charsRead < READLEN && 
		   (len = br.read (buf, charsRead, READLEN-charsRead)) >= 0)
		charsRead += len;
	    if (len < 0)
		break;
	    System.out.println ("read " + buf);
	    br.reset();
	    br.read();    // next char
	}

	// Test BufferedInputStream
        BufferedInputStream b =
	  new BufferedInputStream(new StringBufferInputStream("ABCDEF"));
        b.mark(3);
        b.read();
        b.read();
        b.read();
        b.reset();
        b.mark(-1);
        byte []bb = new byte[5];
        int r = b.read(bb, 0, 5);
        if (r == 5) {
            System.out.println("Success.");
        } else {
            System.out.println("Failure " + r);
        }
    }
}

/* Expected Output:
read 012
read 123
read 234
read 345
read 456
read 567
read 678
read 789
read 89A
read 9AB
read ABC
Success.
*/
