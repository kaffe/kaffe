/**
 * Test socket timeouts for accept and read for simple stream sockets 
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.net.*;
import java.io.*;

public class SoTimeout {
    public static void main(String av[]) throws Exception {
        final int port = 45054;
	final String foo = "foo";

        ServerSocket server = new ServerSocket(port); 
        Thread t = new Thread() {
            public void run() {
		try {
		    Socket s = new Socket(InetAddress.getLocalHost(), port);
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
    }
}
