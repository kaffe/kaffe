// ReaderReadVoidTest.java
// Submitted by Dalibor Topic <dtopic@socs.uts.edu.au>

/* Tests whether Reader.read() has a race condition.
   Two threads read from the same reader, and if there
   is a race condition they end up with the same character,
   otherwise they don't.
*/

import java.io.*;

public class ReaderReadVoidTest {
	public static void main(String [] args) {
		PseudoThread.main(new String[0]);
	}
}

class PseudoThread extends Thread {
	Reader reader;
        public int chrRead = 0;

	public long timer = 0;

	public PseudoThread(Reader r, String name) {
		super(name);
		reader = r;
	}

	public void run() {
		try {			
			chrRead = reader.read();
		}
		catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void main(String[] args) {
		PseudoReader pr = new PseudoReader();
		int chr1 = 0;
		int chr2 = 0;

		PseudoThread thread1 = new PseudoThread(pr, "T1");
		PseudoThread thread2 = new PseudoThread(pr, "T2");

		thread1.start();
		thread2.start();

		try {
		        thread1.join();
			thread2.join();
		}
		catch (InterruptedException e) {
		        e.printStackTrace();
		}

		// with an unsinchronised read() implementation,
		// both threads will read the same value "2",
		// with a synchronised one they will read "1" and "2"
		// (like in the new Kaffe implementation, since they are
		// synchronised via a lock in read())

		if (thread1.chrRead != thread2.chrRead) {
		        System.out.println("SUCCESS");
		}
		else {
		        System.out.println("FAILURE");
		}
	}
}


class PseudoReader extends Reader {
	char chrRead = 0;

	public void close() throws IOException {
	}

	public int read(char buf[], int offset, int count) throws IOException {

		synchronized (lock) {
		        /*
		        System.out.println(Thread.currentThread().toString() 
				+ "locked!");
		        */
	
			chrRead++;

			for (int i = offset; i < offset + count; i ++) {
				buf[i] = chrRead;
			}

			PseudoThread pt = (PseudoThread)Thread.currentThread();

			if (pt.getName().equals("T1")) {
			    pt.timer = 1000;
			}
			else {
			    pt.timer = 1;
			}
		}
	
		/*
		System.out.println(Thread.currentThread().toString() 
			+ "unlocked!");
		*/

		synchronized (Thread.currentThread()) {
			try {
			        /*
				System.out.println(Thread.currentThread().toString() 
					+ "waiting!");
				*/

				Thread.currentThread().sleep(
					((PseudoThread)Thread.currentThread()).timer);
				/*
				System.out.println(Thread.currentThread().toString() 
					+ "free!");
				*/

			}
			catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

		return count;
	}
}

/* Expected Output:
SUCCESS
*/



