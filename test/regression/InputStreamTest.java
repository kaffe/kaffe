import java.io.*;

public class InputStreamTest {
        static public void main (String args[]) {
                try {
                        StringBufferInputStream sbis =
                                new
StringBufferInputStream("foobar\r\nxbarfoo\rx");
                        DataInputStream dis = new DataInputStream(sbis);

                        System.out.println(dis.readLine());
                        System.out.println(sbis.read());
                        System.out.println(dis.readLine());
                        System.out.println(sbis.read());
			System.out.println(dis.read());

                } catch (IOException ioe) {
                }
        }
}

// skip run
/* Expected Output:
foobar
120
barfoo
-1
120
*/

