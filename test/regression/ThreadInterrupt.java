/**
 * Tests for Thread.interrupt()
 * This tests only basic functionality, no corner cases.
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.io.*;
import java.net.ServerSocket;

public class ThreadInterrupt {
   
    /*
     * See if interrupt flag is cleared when a thread dies.
     */
    static class ThreadWithField extends Thread {
	int field = 0;
	
	public void run() {
			infiniteLoop:
	    for(;;)
	    {
		synchronized(this) 
		{
		    if (field != 0)
			break infiniteLoop;
		}
		Thread.yield();
	    }
	    
	    // just exit.
	}
    };
    
    public static void main(String av[]) throws Exception {
	Thread watchdog = new Thread() {
	    public void run() {
		try {
		    Thread.sleep(180 * 1000); // 3 minutes should be sufficient
		} catch (InterruptedException _) { }
		System.out.println("Failure! Watchdog timed out.");
		System.exit(-1);
	    }
	};
	watchdog.start();

	Thread t;

	/*
	 * Create a thread that will wait().  Test that
	 * throwing InterruptedException clears interrupted bit.
	 */
	t = new Thread("Interrupt-in-wait") {
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

	
	/*
	 * Create a thread that will sleep().  Test that interrupted exception
	 * will clear interrupted bit.
	 */
	t = new Thread("Interrupt-in-sleep") {
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

	/*
	 * XXX JanosVM Change: we don't think its worth supporting the interrupt
	 * semantics for dead threads (that their interrupt flag is cleared).
	 * Not part of the spec, that we can see.  Not a good practice
	 * either (relying on the interrupt state of some other thread).
	 */
	if (false)
	{
		/*
		 * Try interrupting a dead thread.
		 */
		t = new Thread() {
			public void run() {
				// just exit.
			}
		};
		t.start();
		while(t.isAlive())
			Thread.yield(); // let t get going and die
		t.interrupt();
		if (t.isInterrupted())
			System.out.println("FAIL: dead thread has interrupt flag set.");
		else
			System.out.println("Success: dead thread interrupt.");
		
		ThreadWithField twf = new ThreadWithField();
		twf.start();
		Thread.sleep(500); // let t get going
		twf.interrupt();
		twf.field = 1;
		while(twf.isAlive())
			Thread.yield();
		if (twf.isInterrupted())
			System.out.println("FAIL: dead thread has interrupt flag set.");
		else
			System.out.println("Success: dead thread interrupt flag clear.");
	}
	
	/*
	 * Create a thread that will wait().  Test that interrupted
	 * exception will clear interrupted bit.  Test that subsequent
	 * sleeps do not get interrupted.
	 */
	t = new Thread("Test-interrupt-post catch") {
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

	/*
	 * Test interrupt on current thread by current thread.
	 * Test both sleep() and wait()
	 */
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

    /*
     * utility funtion to start the given thread, sleep for a bit,
     * then interrupt and join the given thread.
     */
    static void ssij(Thread t) {
	t.start();
	try {
	    Thread.sleep(1000);
	    t.interrupt();
	    t.join();
	} catch (InterruptedException e) { 
	    System.out.println("caught " + e);
	    e.printStackTrace();
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
