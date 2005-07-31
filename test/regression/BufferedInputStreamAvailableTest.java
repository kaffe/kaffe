import java.io.*;

public class BufferedInputStreamAvailableTest {
    public static void main(String av[]) throws Exception {

	File file = new File("BufferedInputStreamAvailableTest.java");
	long flen = file.length();
	BufferedInputStream is = new BufferedInputStream(
	  new FileInputStream(file), (int)flen);
	int alen = is.available();
	System.out.println((int)flen == alen);
    }
}

/* Expected Output:
true
*/
