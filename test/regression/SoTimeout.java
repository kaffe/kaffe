/**
 * Test socket timeouts for accept and read for simple stream sockets 
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.net.*;
import java.io.*;

public class SoTimeout {
    public static void main(String av[]) throws Exception {
	final String foo = "foo";

	int tryport = 45054;
	ServerSocket server;
	for(;;++tryport) {
	    try {
		server = new ServerSocket(tryport);
		break;
	    } catch (IOException _) {}
	}
        final int port = tryport;

        Thread watchdog = new Thread() {
            public void run() {
                try {
                    Thread.sleep(10000);
                } catch (InterruptedException _) { }
                System.out.println("Failure:   Time out.");
                System.exit(-1);
            }
        };
        watchdog.start();

        Thread t = new Thread() {
            public void run() {
		try {
		    Socket s = new Socket(InetAddress.getByName(null), port);
		    try {
			Thread.sleep(3000);
		    } catch (InterruptedException e) {
			System.out.println("Failure " + e);
		    }
		    PrintWriter p = new PrintWriter(s.getOutputStream());
		    p.println(foo);
		    p.close();
		} catch (Exception e) {
		    System.out.println("Failure " + e);
		}
            }
        };
        server.setSoTimeout(1000);
        Socket rsocket = null;
	try {
	    rsocket = server.accept(); 
	} catch (InterruptedIOException e) {
	    // System.out.println(e);
	    System.out.println("Success 1.");
	}
        t.start();
	rsocket = server.accept(); 
	System.out.println("Success 2.");
        rsocket.setSoTimeout(2000);	// NB:	2 * 2000 > 3000
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
	}
	System.exit(0);
    }
}

/* Expected Output:
Success 1.
Success 2.
Success 3.
Success 4.
*/
