import java.io.*;
import java.util.*;

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
	    // jikes will incorrectly print a char[] as a String, but
	    // Sun's javac won't.
	    System.out.println ("read " + new String(buf));
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

	// More rigourous test of BufferedInputStream
	BufferedInputStream bis = new BufferedInputStream(
	  new InputStream() {
	    private int count = 0;
	    public int read() {
	      return count++ & 0xff;
	    }
	  });
	final int BUFSIZE = 64;			// must be power of 2!
	Random rand = new Random(12345678);
	int count = 0;
	for (int i = 0; i < 100; i++) {
	  byte[] tbuf = new byte[BUFSIZE];
	  int read1 = rand.nextInt() & (BUFSIZE - 1);
	  int read2 = rand.nextInt() & (BUFSIZE - 1);
	  int read3 = rand.nextInt() & (BUFSIZE - 1);
	  int mark = rand.nextInt() & (BUFSIZE - 1);
	  read1 = bis.read(tbuf, 0, read1);
	  bis.mark(mark);
	  if (read1 + read2 > tbuf.length)
	    read2 = tbuf.length - read1;
	  read2 = bis.read(tbuf, read1, read2);
	  if (read2 <= mark) {
	    bis.reset();
	    if (read1 + read3 > tbuf.length)
	      read3 = tbuf.length - read1;
	    read3 = bis.read(tbuf, read1, read3);
	  } else {
	    read3 = read2;
	  }
	  for (int j = 0; j < read1 + read3; j++) {
	    if ((int) (tbuf[j] & 0xff) != ((count + j) & 0xff)) {
	      throw new Error("brokenness");
	    }
	  }
	  count += read1 + read3;
	}
	System.out.println("Success.");
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
Success.
*/
