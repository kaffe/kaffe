/**
 * ExceptionInInitializerTest.java
 *
 * This is a test for some intricacies during class loading and initializing.
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
/*
 * This class will throw an exception when its static initializer is
 * executed.  Hence, this class is never successfully loaded.
 */
class Throw {
    static {
	System.out.println("Running static initializer of Throw");
	// NullPointerException!!!
	System.out.println(((String)null).hashCode());
    }

    // we use this variable to trigger an active use of this class
    static boolean activeUse;
}

/*
 * main test class
 */
public class ExceptionInInitializerTest
{
    public static void main(String av[]) {
	// start watchdog.  If this thread times out, we've most 
	// likely deadlocked.
	new Thread(
	    new Runnable() {
		public void run()
		{
		    try { Thread.sleep(5000); } catch (Exception _) {}
		    System.out.println("Failure due to timeout, exiting");
		    System.exit(-1);
		}
	    }).start();

	System.out.println("Static initializer test");
	try {
	    // trigger the loading of class "Throw" by attempting to use it
	    Throw.activeUse = true;

	    // the resulting exception should be caught here
	} catch (ExceptionInInitializerError e) {

	    /* 
	     * Attempt to load and process java.math.BigDecimal from within
	     * a different thread.
	     * This checks whether the original thread still has the class 
	     * loading mechanism locked.  It mustn't, of course.
	     */
	    Thread th = new Thread(
		new Runnable() {
		    public void run()
		    {
			System.out.println("Now loading " + 
			    java.math.BigDecimal.class.getName());
		    }
		});
	    th.start();
	    try { th.join(); } catch (Exception ire) {
		System.out.println(ire);
	    }

	    // extract the exception that was thrown in the initializer
	    Throwable t = e.getException();
	    // it better be a NullPointerException
	    if (t instanceof NullPointerException) {
		System.out.println("Success 1.");

		/* Now let's try accessing Throw again.
		 * The correct error to throw here is NoClassDefFoundError
		 * Let's do this in another thread to ensure we don't have
		 * the centry still locked.
		 */
		Thread th2 = new Thread() {
		    public void run() {
			try {
			    System.out.println(Throw.activeUse);
			} catch (NoClassDefFoundError _) {
			    System.out.println("Success 2.");
			    System.exit(0);
			}
		    }
		};
		th2.start();
		try { th2.join(); } catch (Exception ire2) {
		    System.out.println(ire2);
		}

		System.out.println("Failed, class was loaded.");
	    }
	    System.out.println(e);
	}
	System.out.println("Failed, exception wasn't caught");
    }
}

/* Expected Output:
Static initializer test
Running static initializer of Throw
Now loading java.math.BigDecimal
Success 1.
Success 2.
*/
