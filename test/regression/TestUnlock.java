/**
 * Test that locks taken in synchronized methods are properly unlocked
 * when an exception occurs.  Note that different mechanisms are used in
 * compiler & interpreter.
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
public class TestUnlock {
    synchronized void throwException() throws Exception {
	throw new Exception();
    }

    synchronized void success() {
	System.out.println("Success.");
    }

    public static void main(String av[]) throws Exception {
	final TestUnlock me = new TestUnlock();

	new Thread() {
	    public void run() {
		try {
		    Thread.sleep(9000);
		} catch (Exception _) { }
		System.out.println("Time out.  Failure.");
		System.exit(-1);
	    }
	}.start();

	Thread t = new Thread() {
	    public void run() {
		try {
		    me.throwException();
		} catch (Exception _) {
		}
	    }
	};
	t.start();
	t.join();

	Thread t2 = new Thread() {
	    public void run() {
		me.success();
	    }
	};
	t2.start();
	t2.join();
	System.exit(0);
    }
}

/* Expected Output:
Success.
*/
