import java.io.*;

public class InputStreamTest {
        static public void main (String args[]) {
                try {
			byte [] chars = "foobar\r\nxbarfoo\rx".getBytes("US-ASCII");
                        ByteArrayInputStream bais =
                                new ByteArrayInputStream(chars);
                        DataInputStream dis = new DataInputStream(bais);

                        System.out.println(dis.readLine());
                        System.out.println(bais.read());
                        System.out.println(dis.readLine());
                        System.out.println(bais.read());
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

