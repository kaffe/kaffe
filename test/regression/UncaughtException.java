/**
 *  This test checks that we indeed invoke the UncaughtException method
 *  for dying threads and that we ignore exceptions in it.
 *
 *  @author Godmar Back <gback@cs.utah.edu>
 */
public class UncaughtException {
    public static void main(String av[]) {
	ThreadGroup g = new ThreadGroup("testgroup") {
	    int i;
	    public void uncaughtException(Thread t, Throwable e) {
		synchronized (this) {
		    i++;
		    if (i == 2) {
			((Object)null).hashCode();
		    }
		    System.out.println("Success " + i + ".");
		}
	    }
	};

	for (int i = 0; i < 3; i++) {
	    new Thread(g, new Integer(i).toString()) {
		public void run() {
		    throw new RuntimeException();
		}
	    }.start();
	}
    }
}

/* Expected Output:
Success 1.
Success 3.
*/
