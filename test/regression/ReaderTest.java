// ReaderMarkTest.java
// submitted by Dalibor Topic <dtopic@socs.uts.edu.au>

import java.io.*;

public class ReaderTest {
	public static void main(String [] args) {
		Reader pr = new Reader() {
			private int counter = 0;
			public void close() throws IOException {
			}
			public int read(char buf[], int offset, int count)
					throws IOException {
				if ((counter += count) >= 10000)
					throw new IOException("enough");
				return count;
			}
		};
		// test mark()
		try {
			pr.mark(0);
		} catch (IOException e) {
			System.out.println(e.toString());
		}
		// test reset()
		try {
			pr.reset();
		}
		catch (IOException e) {
			System.out.println(e.toString());
		}
		// test ready()
		try {
			System.out.println(pr.ready());
		} catch (IOException e) {
			System.out.println(e.toString());
		}
		// test skip()
		try {
			System.out.println(pr.skip(10000));
		} catch (IOException e) {
			System.out.println(e.toString());
		}
	}
}

/* Expected Output:
java.io.IOException: mark() not supported
java.io.IOException: reset() not supported
false
java.io.IOException: enough
*/
