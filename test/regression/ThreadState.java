import java.util.*;
import java.io.*;

public class ThreadState extends Thread {
  public final static int DEFAULT_NUMTHREADS = 10;
  boolean childRunning;
  static PrintStream p;

  // Test thread states
  public static void main(String args[]) throws Exception {
    int numThreads;
    int index = 0;

    if (args.length > 0 && args[0].equals("-v")) {
      p = System.out;
      index = 1;
    }

    try {
      numThreads = Integer.parseInt(args[index]);
    } catch (Exception e) {
      numThreads = DEFAULT_NUMTHREADS;
    }

	new Thread() {
	    public void run() {
		try {
		    Thread.sleep(60 * 1000);
		} catch (Exception _) { }
		System.out.println("Time out.  Failure.");
		System.exit(-1);
	    }
	}.start();

    Thread[] threads = new Thread[numThreads];
    for (int i = 0; i < numThreads; i++) {
      threads[i] = new ThreadState();
      verbose("main starting " + threads[i].getName());
      threads[i].start();
    }
    for (int i = 0; i < numThreads; i++) {
      try {
	verbose("main joining " + threads[i].getName());
	threads[i].join();
      } catch (InterruptedException e) {
	check(false, "main " + e);
      }
    }
    System.exit(0);
  }

  public void run() {
    try {
      verbose(getName() + " running");

      // Create child thread
      Thread t = new Thread() {
        public synchronized void run() {
          try {
	    childRunning = true;
	    verbose(ThreadState.this.getName() + " child running [child]");
	    try {
	      this.wait(0);
	    } catch (InterruptedException e) {
	      check(false, "thread " + e);
	    }
	  verbose(ThreadState.this.getName() + " child thread exiting");
          } catch (Throwable t) {
            t.printStackTrace();
	    System.exit(-1);
	  }
        }
      };

      // Check state
      check(!t.isAlive(), "alive before start()");
      verbose(getName() + " starting child thread");
      t.start();
      check(t.isAlive(), "dead after start()");
  
      // Check setDaemon after start()
      try {
        t.setDaemon(false);
        check(false, "setDaemon() after start");
      } catch (IllegalThreadStateException e) {
      }

      // Check double start()
      try {
        t.start();
        check(false, "start() while alive");
      } catch (IllegalThreadStateException e) {
      }
  
      // Wait for thread to be running
      while (!childRunning) {
        Thread.yield();
      }
      verbose(getName() + " child thread running [parent]");

      // Notify child he can exit
      synchronized (t) {
        t.notify();
      }
  
      verbose(getName() + " joining my child thread");
      try {
        t.join();
      } catch (InterruptedException e) {
        check(false, "join: " + e);
      }
      check(!t.isAlive(), "alive after join()");
  
      // Check double start()
      try {
        t.start();
        check(false, "start() after dead");
      } catch (IllegalThreadStateException e) {
      }

      // OK
      synchronized (ThreadState.class) {
        System.out.println("Success.");
      }
      verbose(getName() + " exiting");
    } catch (Throwable t) {
      t.printStackTrace();
      System.exit(-1);
    }
  }

  public static void check(boolean that, String msg) {
    if (!that) {
      System.err.println("Failure: " + msg);
      System.exit(1);
    }
  }

  public static void verbose(String msg) {
    if (p != null) {
      p.println(msg);
      p.flush();
    }
  }
}

/* Expected Output:
Success.
Success.
Success.
Success.
Success.
Success.
Success.
Success.
Success.
Success.
*/
