/**
 * ???
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.util.Vector;

class Base {
    static boolean useme;

    static {
	Base b = new Base();
	ProcessClassInst.v.addElement(b);
	useme = true;

	try {
	    Thread.sleep(2000);
	} catch (InterruptedException e) { }
	((Object)null).toString();
    }
    public String toString() { return "a base"; }
}

public class ProcessClassInst
{
    static Vector v = new Vector();

    public static void main(String av[]) throws Exception {
	// a watchdog thread that kills us off after 9 sec
	Thread wd = new Thread() {
	    public void run() {
		try {
		    Thread.sleep(9000);
		    System.out.println("sorry, you timed out");
		    System.exit(-1);
		} catch (Exception e) {
		    System.out.println(e);
		}
	    }
	};
	wd.setDaemon(true);
	wd.start();

	// a thread that will load Base
	Thread t = new Thread() {
	    public void run() {
		try {
		    Class.forName("Base");
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
		    System.out.println("tan " + new Base());
		} catch (Throwable tt) {
		    System.out.println("tan " + tt);
		}
	    }
	};
	t0.start();
	t0.join();

	Class cc = v.elementAt(0).getClass();
	try {
		cc.newInstance();
	}
	catch (NoClassDefFoundError e) {
		System.out.println(e);
	}
    }
}

/* Expected Output:
tan java.lang.NoClassDefFoundError: Base
java.lang.NoClassDefFoundError: Base
*/
