/*
 * ThreadStop.java
 *
 * Test stopping various threads.
 * 
 * Courtesy Pat Tullmann (tullmann@cs.utah.edu)
 */

class ThreadStop_BlockThread
	extends Thread
{
	String blocker = "blocker";

	public void run()
	{
		System.out.println("Target (BlockThread) running...");
		try {
			synchronized(blocker)
			{
				System.out.println(" Locked ...");
				try
				{
					blocker.wait();
				}
				catch (InterruptedException ie)
				{
					System.out.println("INTERRUPTED.  TRY AGAIN.  " +ie);
				}
			}
		} catch (Error o) {
			System.out.println(" Handling my own: " + o);
			System.out.print(" Am I alive? Answer: ");
			System.out.println(Thread.currentThread().isAlive());
			// rethrow exception
			throw o;
		}
	}

	public ThreadStop_BlockThread()
	{
		super("BlockThread");
	}
}

class ThreadStop_RunThread
	extends Thread
{
	static int ct_ = 0;
	public void run()
	{
		System.out.println("Target (RunThread) running...");
		while(true)
		{
			ct_++;
		}
	}

	public ThreadStop_RunThread()
	{
		super("RunThread");
	}
}

class ThreadStop_MonitorThread
	extends Thread
{
	static Object obj_ = new Object();

	public void run()
	{
		System.out.println("Target (MonitorThread) running...");
		synchronized(obj_)
		{
			try
			{
				obj_.wait();
			}
			catch (InterruptedException ie)
			{
				System.out.println("INTERRUPTED.  TRY AGAIN. " +ie);
			}
		}
	}

	public ThreadStop_MonitorThread()
	{
		super("MonitorThread");
	}
}


class ThreadStop_DoneThread
	extends Thread
{
	public void run()
	{
		// just exit immediately
	}

	public ThreadStop_DoneThread()
	{
		super("DoneThread");
	}
}

class ThreadStop_SelfStop
	extends Thread
{
	public void run()
	{
		this.stop();

		System.out.println("SelfStop thread returned from stop()");
	}
	
	public ThreadStop_SelfStop()
	{
		super("SelfStopThread");
	}
}


public class ThreadStop
{
	static int ct_ = 0;
	static Object obj_ = new Object();
	
	static public void sleep(int time)
	{
		try 
		{
			Thread.sleep(time);
		}
		catch (InterruptedException ie)
		{
			msg("Dammit!  Thread.sleep() was interrupted.  Results are INVALID. " +ie);
		}
	}

	static public void killIt(Thread target)
	{
		// Sleep for a bit to let it get going.
		sleep(500);
		
		// Kill the target
		target.stop();
		
		// Sleep for a bit and then check if its dead
		sleep(500);
		if (target.isAlive())
			msg(" Failure! Target is alive.");
		else
			msg(" Success. Target is dead.");
	}

	static public void main(String[] args)
	{
		Thread target;
		
		msg("Test 1: Stop a thread that's blocked on itself");
		target = new ThreadStop_BlockThread();
		msg(" Is thread alive before starting? " + target.isAlive());
		target.start();

		sleep(500);
		synchronized(((ThreadStop_BlockThread)target).blocker) {
		    killIt(target);
		    // sleep while holding on to target
		    msg(" Sleeping while still holding on to target");
		    sleep(500);
		    msg(" Releasing target lock");
		}
		// give this thread time to catch its death exception
		sleep(500);

		msg("Test 2: Stop a thread that's blocked in a monitor");
		target = new ThreadStop_MonitorThread();
		target.start();

		killIt(target);


		msg("Test 3: Stop a thread that's running");
		target = new ThreadStop_RunThread();
		// make sure we get to run again if non-preemptive
		Thread.currentThread().setPriority(Thread.MAX_PRIORITY);
		target.start();
		
		killIt(target);

		msg("Test 4: Stop a thread that's done");
		target = new ThreadStop_DoneThread();
		target.start();
		
		killIt(target);

	    // killing unstarted threads doesn't work yet
	    if (false) {
		msg("Test 5: Stop a thread that's never been run");
		target = new ThreadStop_RunThread();
		
		killIt(target);

		target.start();
		
		sleep(100);
		if (target.isAlive())
			msg(" Failure (#5)! Target is still alive!");
		
	    }
		msg("Test 6: Have a thread stop itself");
		target = new ThreadStop_SelfStop();
		target.start();
		
		sleep(2000);
		
		if (target.isAlive())
			msg(" Failure (#6)! Target is still alive!");
		else
			msg(" Success. Target is dead.");

		msg("All tests completed");
	}

	public void run()
	{
		while (true)
		{
			try
			{
				synchronized(obj_)
				{
					msg("Running; blocking (ct=" +ct_+ ")");
					obj_.notify();
					ct_++;
					obj_.wait();
					msg("Woken (_ct=" +ct_+ ")");
				}
			}
			catch (java.lang.InterruptedException ie)
			{
				msg("Dammit.  Test is invalid, try again.  " +ie);
			}
		}
	}

	public static void msg(String arg)
	{
		System.out.println(arg);
	}
}


// Skip run
// javac flags: -nowarn
/* Expected Output:
Test 1: Stop a thread that's blocked on itself
 Is thread alive before starting? false
Target (BlockThread) running...
 Locked ...
 Success. Target is dead.
 Sleeping while still holding on to target
 Releasing target lock
 Handling my own: java.lang.ThreadDeath
 Am I alive? Answer: false
Test 2: Stop a thread that's blocked in a monitor
Target (MonitorThread) running...
 Success. Target is dead.
Test 3: Stop a thread that's running
Target (RunThread) running...
 Success. Target is dead.
Test 4: Stop a thread that's done
 Success. Target is dead.
Test 6: Have a thread stop itself
 Success. Target is dead.
All tests completed
*/
