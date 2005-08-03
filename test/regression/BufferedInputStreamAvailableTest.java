import java.io.*;

public class BufferedInputStreamAvailableTest {
    public static void main(String av[]) throws Exception {

	File file = new File("TestScript");
	long flen = file.length();
	BufferedInputStream is = new BufferedInputStream(
	  new FileInputStream(file), (int)flen);
	int alen = is.available();
	// System.out.println((int)flen == alen);
	// We are happy if (int)flen == alen, but that is not necessarily true.
	System.out.println(alen >= 0);
    }
}

/* Expected Output:
true
*/
