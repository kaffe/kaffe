/**
 * Test interruptable I/O.
 *
 * Note that this test will not run under Sun's JDK1.2 on Solaris, see
 * http://developer.java.sun.com/developer/bugParade/bugs/4178050.html
 *
 * It's unlikely to work on Windows either, see
 * http://developer.java.sun.com/developer/bugParade/bugs/4154947.html
 *
 * It it supported by Kaffe's jthreads, though it may not work under other
 * threading systems either.  XXX
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.net.*;
import java.io.*;

public class SoInterrupt {
    public static void main(String av[]) throws Exception {
        final int port = 45054;
	final String foo = "foo";
	final Thread main = Thread.currentThread();

        ServerSocket server = new ServerSocket(port); 
        Thread t = new Thread() {
            public void run() {
		try {
		    // let main thread wait a while
		    try {
			Thread.sleep(1000);
		    } catch (InterruptedException e) {
			System.out.println("Failure " + e);
		    }
		    // System.out.println("interrupting " + main);
		    // interrupt it
		    main.interrupt();
		    // give it some more time
		    try {
			Thread.sleep(1000);
		    } catch (InterruptedException e) {
			System.out.println("Failure " + e);
		    }

		    // now connect
		    // System.out.println("connecting...");
		    Socket s = new Socket(InetAddress.getLocalHost(), port);
		    // wait some
		    try {
			Thread.sleep(1000);
		    } catch (InterruptedException e) {
			System.out.println("Failure " + e);
		    }

		    // System.out.println("interrupting again " + main);
		    // interrupt it again
		    main.interrupt();
		    // wait some more
		    try {
			Thread.sleep(1000);
		    } catch (InterruptedException e) {
			System.out.println("Failure " + e);
		    }

		    // now finish up
		    PrintWriter p = new PrintWriter(s.getOutputStream());
		    p.println(foo);
		    p.close();
		} catch (Exception e) {
		    System.out.println("Failure " + e);
		}
            }
        };
        t.start();
        Socket rsocket = null;
	try {
	    // System.out.println("waiting for client...");
	    rsocket = server.accept(); 
	    // System.out.println("accepted..." + rsocket);
	} catch (InterruptedIOException e) {
	    // System.out.println(e);
	    System.out.println("Success 1.");
	}
	// System.out.println("waiting for client again...");
	rsocket = server.accept(); 
	System.out.println("Success 2.");
	InputStream is = rsocket.getInputStream();
	LineNumberReader r = new LineNumberReader(new InputStreamReader(is));
        byte []b = null;
        try {
	    r.readLine();
	} catch (InterruptedIOException e) {
	    // System.out.println(e);
	    System.out.println("Success 3.");
	}
	String s = r.readLine();
	if (s.equals(foo)) {
	    System.out.println("Success 4.");
	} else {
	    System.out.println("Failure: `" + s + "'");
	}
    }
}

/* Expected Output:
Success 1.
Success 2.
Success 3.
Success 4.
*/
