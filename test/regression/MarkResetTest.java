import java.io.*;

public class MarkResetTest {
    public static void main(String av[]) throws Exception {
        BufferedInputStream b =
	  new BufferedInputStream(new StringBufferInputStream( "ABCDEF"));
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
Success.
*/
