// BufferedReaderTest.java
// submitted by Dalibor Topic <robilad@yahoo.com>
import java.io.*;

public class BufferedReaderTest {

	static class PseudoReader extends Reader {
	    public void close() {
	    }

	    public int read(char buf[], int offset, int count) {
		System.out.println("count = " + count);
		return (count);
	    }
	}

	static class PseudoReader2 extends Reader {
	    public int closeCounter = 0;
  
	    public void close() {
		closeCounter++;
	    }
 
	    public int read(char buf[], int offset, int count) {
		return (-1);
	    }
	}

	static class PseudoReader3 extends Reader {
	    boolean called;
	    boolean calledAgain;

	    public void close () {
	    }

	    /* This version of read returns a string of characters
	       without EOL on the first call,
	       then 0 characters on the second.

	       If its caller is blocking, the caller will try again,
	       otherwise it will have enough.

	       If it tries again, and calls read for the 
	       third time, an IOException is thrown.
	    */

	    public int read(char[] buf2, int offset, int count) 
		throws IOException {

		if (calledAgain) {
		    throw (new IOException("you keep trying ..."));
		}
		if (called) {
		    calledAgain = true;
		    return (0);
		}
		else {
		    called = true;
  
		    String s = new String("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

		    s.getChars(0, count, buf2, offset);

		    return (count);
		}
	    }
	}

	public static void main(String [] args) throws Exception {

		// This test shows that the default buffersize for
		// a BufferedReader is 8192 characters.

		final int defaultsize = 8 * 1024;

		BufferedReader br = new BufferedReader(new PseudoReader());
		try {

		    // Mark br, to make sure the data is copied into
		    // the buffer.
		    
		    br.mark(0);
		    
		    // This should call the read method of PseudoReader
		    // only once if the default buffersize of
		    // a buffered reader is 8192.
		    
		    br.read(new char[defaultsize], 0, defaultsize);

		    // And now call it to get one character, and
		    // it should refill its buffer

		    br.read(new char[1], 0, 1);

		}
		catch (IOException e) {
			e.printStackTrace();
		}


		// Let's try to set a mark with
		// a negative read ahead,
		// this should throw an IllegalArgumentException

		StringReader sr = new StringReader("sr");
		br = new BufferedReader(sr);

		try {
			br.mark(-1);
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}


		// This is a simple test of BufferedReader
		// functionality.

		sr = new StringReader("1234567");

		br = new BufferedReader(sr);

		try {
			char [] buf = new char[3];

			if (br.read(buf) == 3) {
				System.out.println(buf);
			}
			else {
				System.out.println("Couldn't read 3 characters");
			}

			br.mark(4);

			buf = new char[4];

			if (br.read(buf) == 4) {
				System.out.println(buf);
			}
			else {
				System.out.println("Couldn't read 4 characters");
			}

			br.reset();

			if (br.read(buf) == 4) {
				System.out.println(buf);
			}
			else {
				System.out.println("Couldn't re-read 4 characters");
			}		
		}
		catch (IOException e) {
			e.printStackTrace();
		}


		/* Trying to create a BufferedReader with 
		   input Reader = null should not result in 
		   an IllegalArgumentException being thrown,
		   instead a NullPointerException should be thrown
		   when the Reader is accessed.
		 */

		sr = new StringReader("1234567");
		br = new BufferedReader(sr);

		try {
			br = new BufferedReader(null);
		}
		catch (NullPointerException e) {
			System.out.println(e.toString());
		}

		try {
			br.read();
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}

		sr = new StringReader("1234567");

		/* Trying to create a BufferedReader with 
		   buffer size <= 0 should result in 
		   an IllegalArgumentException being thrown.
		 */

		try {
			br = new BufferedReader(sr, 0);
		}
		catch (IllegalArgumentException e) {
			System.out.println(e.toString());
		}

		/* A call to close a BufferedReader that has
		   already been closed should do nothing.
		   So the expected result should be "1"
		 */

		PseudoReader2 pr2 = new PseudoReader2();
		br = new BufferedReader(pr2);

		try {
			br.close();
			br.close();
		}
		catch (IOException e) {
			System.out.println(e.toString());
		}

		System.out.println(pr2.closeCounter);


		/* Trying to read from a BufferedReader that has
		   already been closed should generate an IOException.
		 */

		try {
			br.close();
		}
		catch (IOException e) {
			System.out.println(e.toString());
		}
		try {
			br.mark(1);
		}
		catch (IOException e) {
			System.out.println(e.toString());
		}
		try {
			br.read();
		}
		catch (IOException e) {
			System.out.println(e.toString());
		}
		try {
			br.read(new char[1]);
		}
		catch (IOException e) {
			System.out.println(e.toString());
		}
		try {
			br.read(new char[1], 0, 1);
		}
		catch (IOException e) {
			System.out.println(e.toString());
		}
		try {
			br.readLine();
		}
		catch (IOException e) {
			System.out.println(e.toString());
		}
		try {
			br.ready();
		}
		catch (IOException e) {
			System.out.println(e.toString());
		}
		try {
			br.reset();
		}
		catch (IOException e) {
			System.out.println(e.toString());
		}
		try {
			br.skip(1);
		}
		catch (IOException e) {
			System.out.println(e.toString());
		}

		int bufferSize = 1000000;

		sr = new StringReader("a");

		// Allocate a really huge buffer
		br = new BufferedReader(sr, bufferSize);

		/* A closed BufferReader should free its buffer */

		// close buffered reader
		try {
			long before = Runtime.getRuntime().freeMemory();
			br.close();

			// Call garbage collector and hope he does his work
			System.gc();

			long after = Runtime.getRuntime().freeMemory();

//			System.out.println(before + " " + after);

			// If garbage collector was successful, and close() did
			// let go the buffer, we expect to see some dramatic
			// change in the amount of free memory.

			if (after - before >= bufferSize) {
//				System.out.println("SUCCESS");
			}
			else {
//				System.out.println("FAILURE");
			}
		}
		catch (IOException e) {
			e.printStackTrace();
		}


		/* Test to see what happens when we try to
		   skip a negative number of characters
		*/

		sr = new StringReader("0123456789");
		br = new BufferedReader(sr,1);

		try {
			System.out.println(br.skip(5));
			System.out.println(br.skip(-5));
			System.out.println((char) br.read());
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}


		/* Testing the reset method */

		sr = new StringReader("ABCDEFGH");
		br = new BufferedReader(sr,1);

		/* Trying to reset an unmarked reader should result in
		   an Exception being thrown
		*/

		System.out.println("Reset unmarked");

		try {
			br.reset();
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}


		/* Now we try something slightly different.
		   We try to invalidate the mark by reading further
		   than the read ahead value passed as the parameter
		   when mark was called.
		   If we try to reset the buffer afterwards,
		   we should receive a nice Exception.
		*/

		System.out.println("Reset invalidated");

		try {
			br.mark(1);
			System.out.println(br.read());
			System.out.println(br.read());
			br.reset();
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}

		// let's see what happens when I skip over
		// marked read ahead buffer.

		System.out.println("Skipping over marked buffer");

		try {
			br.mark(1);
			br.skip(2);
			System.out.println(br.read());

			// According to JDK 1.1.8 behaviour,
			// this should invalidate the
			// mark, for this buffer size at least.

			br.reset();
			System.out.println(br.read());
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}

		/*
		   Actually, that seams to depend on the
		   buffer size. I've run tests with different
		   buffer sizes, and when the buffer is large
		   enough to serve the read ahead buffer and the
		   subsequent reads, then reset still works.
		*/

		sr = new StringReader("ABCDEFGH");
		br = new BufferedReader(sr);

		System.out.println("Reset invalidated");

		try {
			br.mark(1);
			System.out.println(br.read());
			System.out.println(br.read());
			br.reset();
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}

		// let's see what happens when I skip over
		// marked read ahead buffer, when
		// buffer size is large enough to accomodate 
		// the requests without the need for refilling.

		System.out.println("Skipping over marked buffer");

		try {
			br.mark(1);
			br.skip(2);
			System.out.println(br.read());

			// According to JDK 1.1.8 behaviour,
			// this should not invalidate the
			// mark. That is a strong hint that
			// mark validation/invalidation should
			// happen in the fillOutBuffer method.

			br.reset();
			System.out.println(br.read());
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}


		/* And now to something completely different:
		   BufferedReader.read (char [], int, int).

		   Here is a small test to check whether all
		   int arguments are checked against their
		   constraints (Java Class Lib. Vol. 1, 2nd Ed.)

		   This test should generate a lot of 
		   Exceptions.
		*/

		sr = new StringReader("sr");
		br = new BufferedReader(sr);
		char [] buf = new char[1];

		System.out.println("Null pointer");

		try {
			br.read(null, 0, 1);
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}

		System.out.println("offset >= buf.length");

		try {
			br.read(buf, 1, 1);
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}

		System.out.println("offset >= buf.length");

		try {

		    /* The funny thing is that this one *doesn't*
		       generate an exception in JDK1.1.8, although 
		       the offset parameter is out of bounds,
		       since there is no buf[1].
		    */
			br.read(buf, 1, 0);
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}

		System.out.println("offset >= buf.length");

		try {
			br.read(buf, 5, 1);
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}

		System.out.println("count > buf.length - offset");

		try {
			br.read(buf, 0, 5);
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}

		System.out.println("offset < 0");

		try {
			br.read(buf, -1, 1);
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}

		System.out.println("count < 0");

		try {
			br.read(buf, 0, -1);
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}

		/* Test whether readLine is blocking or not */

		PseudoReader3 pr3 = new PseudoReader3();
		br = new BufferedReader(pr3, 10);

		try {

		    /* Java Class Libraries Vol 1 2nd Edition
		       says that readLine is blocking.
		       JDK 1.1.8 behaves that way.
		    */
			System.out.println(br.readLine());
		}
		catch (Exception e) {
			System.out.println(e.toString());
		}
	}
}

/* Expected Output:
count = 8192
count = 8192
java.lang.IllegalArgumentException: Read-ahead limit < 0
123
4567
4567
java.lang.NullPointerException
java.lang.IllegalArgumentException: Buffer size <= 0
1
java.io.IOException: Stream closed
java.io.IOException: Stream closed
java.io.IOException: Stream closed
java.io.IOException: Stream closed
java.io.IOException: Stream closed
java.io.IOException: Stream closed
java.io.IOException: Stream closed
java.io.IOException: Stream closed
5
java.lang.IllegalArgumentException: skip value is negative
Reset unmarked
java.io.IOException: Stream not marked
Reset invalidated
65
66
java.io.IOException: Mark invalid
Skipping over marked buffer
69
java.io.IOException: Mark invalid
Reset invalidated
65
66
Skipping over marked buffer
67
65
Null pointer
java.lang.NullPointerException
offset >= buf.length
java.lang.IndexOutOfBoundsException
offset >= buf.length
offset >= buf.length
java.lang.IndexOutOfBoundsException
count > buf.length - offset
java.lang.IndexOutOfBoundsException
offset < 0
java.lang.IndexOutOfBoundsException
count < 0
java.lang.IndexOutOfBoundsException
java.io.IOException: you keep trying ...
*/
