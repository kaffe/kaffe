/**
 * Tests for Thread.interrupt()
 * This tests only basic functionality, no corner cases.
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.io.*;
import java.net.ServerSocket;

public class ThreadInterrupt {
    public static void main(String av[]) throws Exception {
	Thread t;
	t = new Thread() {
	    public void run() {
		synchronized(this) {
		    try {
			wait();
			System.out.println("FAIL: returned from wait"); 
		    } catch (InterruptedException e) {
			if (isInterrupted() || interrupted()) {
			    System.out.println(
				"FAIL: interrupted flag has been turned on."); 
			} else {
			    System.out.println("Success 0a.");
			}
		    }
		}
	    }
	};
	ssij(t);
	t = new Thread() {
	    public void run() {
		try {
		    Thread.sleep(5000);
		    System.out.println("FAIL: returned from sleep"); 
		} catch (InterruptedException e) {
		    if (isInterrupted() || interrupted()) {
			System.out.println(
			    "FAIL: interrupted flag has been turned on."); 
		    } else {
			System.out.println("Success 0b.");
		    }
		}
	    }
	};
	ssij(t);
	t = new Thread() {
	    public void run() {
		synchronized (this) {
		    try {
			wait();
		    } catch (InterruptedException e) {
			if (isInterrupted() || interrupted()) {
			    System.out.println(
				"FAIL: interrupted flag has been turned on."); 
			} 
		    }
		    if (isInterrupted() || interrupted()) {
			System.out.println(
			    "FAIL: interrupted flag has been turned on."); 
		    } 
		    try {
			Thread.sleep(1000);
		    } catch (InterruptedException e) {
			System.out.println("FAIL: " + e);
		    }
		}
	    }
	};
	t.start();
	Thread.sleep(1000);
	synchronized(t) {
	    t.interrupt();
	    if (t.isInterrupted()) {
		System.out.println("Success 1.");
	    } else {
		System.out.println("Failure 1.");
	    }
	    // make sure isInterrupted doesn't clear the flag.
	    if (t.isInterrupted()) {
		System.out.println("Success 2.");
	    } else {
		System.out.println("Failure 2.");
	    }
	}
	Thread.sleep(1000);		// let thread finish
	if (t.isInterrupted()) {
	    System.out.println("Failure 3.");
	} else {
	    System.out.println("Success 3.");
	}
	t.join();
	Thread watchdog = new Thread() {
	    public void run() {
		try {
		    Thread.sleep(3000);
		} catch (InterruptedException _) { }
		System.out.println("Failure 4/5.:   Time out.");
		System.exit(-1);
	    }
	};
	watchdog.start();

	Thread me = Thread.currentThread();
	me.interrupt();
	synchronized(me) {
	    try {
		me.wait(4000);
		System.out.println("Failure 4.");
	    } catch (InterruptedException e) {
		System.out.println("Success 4.");
	    }

	    me.interrupt();
	    try {
		Thread.sleep(4000);
		System.out.println("Failure 5.");
	    } catch (InterruptedException e) {
		System.out.println("Success 5.");
	    }
	}
	System.exit(0);
    }

    static void ssij(Thread t) {
	t.start();
	try {
	    Thread.sleep(1000);
	    t.interrupt();
	    t.join();
	} catch (InterruptedException e) { 
	    System.out.println("caught " + e);
	}
    }
}


/* Expected Output:
Success 0a.
Success 0b.
Success 1.
Success 2.
Success 3.
Success 4.
Success 5.
*/
