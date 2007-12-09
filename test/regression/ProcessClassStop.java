/**
 * Test that class processing is unaffected by thread stopping in
 * static initializers.
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.util.Vector;
import java.lang.reflect.*;

class StopBase {
    static boolean useme;

    private static void killme()
    {
	throw new ThreadDeath();
    }

    static {
	StopBase b = new StopBase();
	ProcessClassStop.v.addElement(b);
	useme = true;
	killme();
    }
    public String toString() { return "a base"; }
}

public class ProcessClassStop
{
    static Vector v = new Vector();

    public static void main(String av[]) throws Exception {
	// a watchdog thread that kills us off after 3 sec
	Thread wd = new Thread() {
	    public void run() {
		try {
		    Thread.sleep(10000);
		    System.out.println("sorry, you timed out");
		    System.exit(-1);
		} catch (Exception e) {
		    System.out.println(e);
		}
	    }
	};
	wd.setDaemon(true);
	wd.start();

	// a thread that will load StopBase
	Thread t = new Thread() {
	    public void run() {
		try {
		    Class.forName("StopBase");
		} catch (Throwable tx) {
		    // System.out.println(tx);
		}
	    }
	};
	t.start();
	Thread.sleep(1000);

	// nobody knows yet whether that should succeed...
	Thread t0 = new Thread() {
	    public void run() {
		try {
		    System.out.println("tan " + new StopBase());
		} catch (Throwable tt) {
		    System.out.println("tan " + tt);
		}
	    }
	};
	t0.start();
	t0.join();

	Class cc = v.elementAt(0).getClass();
	try {
		final Object ni = cc.newInstance();
	} catch (NoClassDefFoundError e) {
		System.out.println(e);
	}
    }
}

/* Expected Output:
tan java.lang.NoClassDefFoundError: StopBase
java.lang.NoClassDefFoundError: StopBase
*/
