public class CatchDeath implements Runnable
{
    public void run()
    {
	try {
	    while (true)
		Thread.yield();
	} catch(Error o) {
	    System.out.println("Caught " + o);
	    throw o;
	}
    }

    public static void main(String av[]) throws Exception
    {
	Thread t[] = new Thread[3];
	t[0] = new Thread(new CatchDeath());
	t[1] = new Thread(new CatchSyncDeath());
	t[2] = new Thread(new CatchSyncWaitDeath());

	for (int i = 0; i < t.length; i++) {
	    t[i].start();
	    Thread.sleep(500);

	    synchronized (t[i]) {
		t[i].stop();
	    }
	    Thread.sleep(500);
	}
    }
}

class CatchSyncDeath implements Runnable
{
    public void run()
    {
	try {
	    synchronized(this) {
		while (true)
		    Thread.yield();
	    }
	} catch(Error o) {
	    System.out.println("Caught " + o);
	    throw o;
	}
    }
}

class CatchSyncWaitDeath implements Runnable
{
    public void run()
    {
	try {
	    synchronized(this) {
		try {
		    wait();
		} catch (InterruptedException e) {
		    System.out.println("Interrupted " + e);
		}
	    }
	    System.out.println("CatchSyncWaitDeath should not be here");
	} catch(Error o) {
	    System.out.println("Caught " + o);
	    throw o;
	}
	System.out.println("Ditto, CatchSyncWaitDeath should not be here");
    }
}

