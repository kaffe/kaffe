/**
 * a test to ensure we do upcalls to static initializers properly.
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
class Base {
    static {
	try {
	    Thread.sleep(5000);
	} catch (Exception e) {
	    System.out.println(e);
	}
    }
}

class Sub extends Base {
}

public class ProcessClassLockTest
{
    public static void main(String av[]) throws Exception {
	// a watchdog thread that kills us off after 2.5 sec
	new Thread() {
	    public void run() {
		try {
		    Thread.sleep(2500);
		    System.out.println("sorry, you timed out");
		    System.exit(-1);
		} catch (Exception e) {
		    System.out.println(e);
		}
	    }
	}.start();

	// a thread that will load Sub and Base
	new Thread() {
	    public void run() {
		try {
		    Class.forName("Sub");
		} catch (Throwable t) {
		    System.out.println(t);
		}
	    }
	}.start();

	// get thread 2 going
	Thread.sleep(1000);

	/* this thread should be unaffected by the fact that thread 2
	 * sleeps in the static initializer of Base
	 */
	new Thread() {
	    public void run() {
		try {
		    Class.forName("this_class_does_not_exist");
		} catch (Throwable t) {
		    System.out.println(t);
		}
		System.exit(0);
	    }
	}.start();
    }
}

/* Expected Output:
java.lang.ClassNotFoundException: this_class_does_not_exist
*/
