public class Preempt implements Runnable
{
    public final static int TIMEOUT = 5000;

    public static void main(String args[])
    {
	new Thread() {
	    public void run() {
		try {
		    Thread.sleep(TIMEOUT);
		} catch (InterruptedException _) {
		    ;
		}
		System.exit(0);
	    }
	}.start();

	for (int i = 0; i < 5; i++)
	    new Thread(new Preempt(), "thread " + i).start();

	synchronized(Preempt.class) {
	    System.out.println("started 5 threads");
	}
    }

    public void run()
    {
	// this shouldn't be necessary---System.out.println should be
	// synchronized, but it apparently is (9/30/99)
	synchronized(Preempt.class) {
	    System.out.println(Thread.currentThread().getName());
	}
	while (true) {
		// Apparently, Solaris is needing this to reschedule sub-thread.
		Thread.yield();
		continue;
	}
    }
}

// Sort output
/* Expected Output:
started 5 threads
thread 0
thread 1
thread 2
thread 3
thread 4
*/
