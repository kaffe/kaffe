
import java.lang.Thread;
import java.lang.Exception;
import java.lang.System;

class ShutdownHookTest_Hook extends Thread {
	private int hook_num;
	private int hook_sleep;
	
	public ShutdownHookTest_Hook(int sleep, int num) {
		hook_num = num;
		hook_sleep = sleep;
	}
	
	public void run() {
		System.out.println("Hook " + hook_num + " started");
		try {
			sleep(hook_sleep);
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		try {
			Runtime.getRuntime().addShutdownHook(new ShutdownHookTest_Hook(0, -1));
			System.out.println("Able to add hook during shutdown?");
			Runtime.getRuntime().halt(0);
		} catch (IllegalStateException ie) {
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}

public class ShutdownHookTest {

	private static boolean a_running = false;
	
	static public void main(String args[]) throws Throwable {

		/* Start with argument checking by */
		Thread dummy = new Thread() {
				public void run() {
					System.out.println("This "
							   + "can't "
							   + "happen!");
				}
			};

		/* ... adding/removing hooks twice and */
		Runtime.getRuntime().addShutdownHook(dummy);
		try
		{
			Runtime.getRuntime().addShutdownHook(dummy);
			System.out.println("Able to add hook twice?");
		}
		catch(IllegalArgumentException e)
		{
		}
		Runtime.getRuntime().removeShutdownHook(dummy);
		if( Runtime.getRuntime().removeShutdownHook(dummy) )
		{
			System.out.println("Able to remove hook twice?");
		}

		/* ... adding a running/dead thread. */
		final Object syncer = new Object();
		Thread a = new Thread() {
				public void run() {
					synchronized( syncer ) {
						a_running = true;
						try { syncer.wait(); }
						catch(Exception e) { }
					}
				}
			};
		a.start();

		synchronized( syncer ) {
			while( !a_running ) {
				syncer.wait(1000);
			}
			try
			{
				Runtime.getRuntime().addShutdownHook(a);
				System.out.println("Able to add running hook?");
			}
			catch(IllegalArgumentException e)
			{
			}
			syncer.notifyAll();
		}
		a.join();
		try
		{
			Runtime.getRuntime().addShutdownHook(a);
			System.out.println("Able to add dead hook?");
		}
		catch(IllegalArgumentException e)
		{
		}

		/* Next, add some valid hooks and */
		Runtime.getRuntime().
			addShutdownHook(new ShutdownHookTest_Hook(1000, 0));
		Runtime.getRuntime().
			addShutdownHook(new ShutdownHookTest_Hook(500, 1));

		/* ... start a thread that will do the first exit and */
		new Thread() {
			public void run() {
				try {
					sleep(1000);
				} catch (Exception e) {
					e.printStackTrace();
				}
				System.out.println("Exiting");
				System.exit(0);
			}
		}.start();

		/*
		 * ... start another thread that will do another exit.  (Need
		 * to make sure the hooks aren't run twice on exit)
		 */
		new Thread() {
			public void run() {
				try {
					sleep(1500);
				} catch (Exception e) {
					e.printStackTrace();
				}
				System.out.println("Exiting again...");
				System.exit(0);
			}
		}.start();

		/*
		 * Finally, just wait to make sure that the exit actually
		 * happens and the main thread is killed.
		 */
		try {
			Thread.sleep(10000);
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("Normal exit");
	}
}

// Sort Output
/* Expected output:
Exiting
Exiting again...
Hook 0 started
Hook 1 started
*/
