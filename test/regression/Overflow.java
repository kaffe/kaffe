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
	Overflow o = new Overflow();
	o.run();	// check for main thread

	o.start();	// check for other thread
	o.join();
    }
}
