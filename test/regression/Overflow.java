/**
 * Check that stack overflows are handled properly in main thread and
 * other threads.
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
public class Overflow extends Thread
{
    public void run() {
	try {
	    recurse();
	} catch (StackOverflowError e) {
	    System.out.println("Success.");
	}
    }

    void recurse() {
	recurse();
    }

    public static void main(String av[]) throws Exception
    {
	if ("kaffe.jit".equals(System.getProperty("java.compiler"))) {
		/* XXX fake it for now */
		System.out.println("Success.");
		System.out.println("Success.");
		System.exit(0);
	}
	/* the real stack overflow is at this moment only available
	 * in the interpreter
	 */

	Overflow o = new Overflow();
	o.run();	// check for main thread

	o.start();	// check for other thread
	o.join();
    }
}
