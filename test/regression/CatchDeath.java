/*
 * See whether we can catch a ThreadDeath exception.
 *
 * Turns out we can't do it reliably, so we need to use a work-around.
 * This test demonstrates and tests the work-around.
 *
 * The work-around is the have the try/catch clause in an outer context
 * that is not executing when the stop request arrives.
 * See try { loop(); } catch ()...
 */

abstract class CatchDeath_iface implements Runnable
{
	public boolean throwDeath;

	public void checkAndThrow()
	{
		if (throwDeath)
			throw new ThreadDeath();
	}
}

public class CatchDeath extends CatchDeath_iface
{
    boolean alwaysTrue()	{ return (true); }

    void loop() {
	while (alwaysTrue())
	{
	    Thread.yield();
	    checkAndThrow();
	}
    }

    public void run()
    {
	try {
	    loop();
	    System.out.println("CatchDeath should not be here");
	} catch(Error o) {
	    System.out.println("CD Caught " + o);
	    System.out.flush();
	    throw o;
	}
	System.out.println("Ditto, CatchDeath should not be here");
    }

    public static void main(String av[]) throws Exception
    {
	Thread t[] = new Thread[2];
	CatchDeath_iface cd[] = new CatchDeath_iface[2];
	t[0] = new Thread(cd[0] = new CatchDeath());
	t[1] = new Thread(cd[1] = new CatchSyncDeath());

	for (int i = 0; i < t.length; i++) {
	    t[i].start();
	    Thread.sleep(500);

	    synchronized (t[i]) {
		    cd[i].throwDeath = true;
	    }
	    Thread.sleep(500);
	}
	System.out.flush();
    }
}

class CatchSyncDeath extends CatchDeath_iface
{
    boolean alwaysTrue()        { return (true); }

    void loop() {
	while (alwaysTrue())
	{
	    Thread.yield();
	    checkAndThrow();
	}
    }

    public void run()
    {
	try {
	    synchronized(this) {
		loop();
	    }
	    System.out.println("CatchSyncDeath should not be here");
	} catch(Error o) {
	    System.out.println("CSD Caught " + o);
	    System.out.flush();
	    throw o;
	}
	System.out.println("Ditto, CatchSyncDeath should not be here");
    }
}

// javac flags: -nowarn
/* Expected Output:
CD Caught java.lang.ThreadDeath
CSD Caught java.lang.ThreadDeath
*/
