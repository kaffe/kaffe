
class WaitTest
    implements Runnable
{
    static final long TOLERANCE = 100;

    private final Object waiter;
    
    WaitTest(Object waiter)
    {
	this.waiter = waiter;
    }
    
    public void run()
    {
	synchronized( this.waiter )
	{
	    try { Thread.sleep(500); } catch(InterruptedException e) { }
	    this.waiter.notifyAll();
	}
    }

    public static void main(String args[])
	throws Exception
    {
	Object waiter = new Object();
	long start, actual, end;
	
	synchronized( waiter )
	{
	    final long time = 500;
	    
	    start = System.currentTimeMillis();
	    waiter.wait(time);
	    end = System.currentTimeMillis();
	    actual = end - start;
	    if( (actual > (time - TOLERANCE)) &&
		(actual < (time + TOLERANCE)) )
	    {
		System.out.println("Okie");
	    }
	    else
	    {
		System.out.println("Fail");
	    }
	}

	synchronized( waiter )
	{
	    final long time = 500;
	    
	    new Thread(new WaitTest(waiter)).start();

	    start = System.currentTimeMillis();
	    waiter.wait(Long.MAX_VALUE);
	    end = System.currentTimeMillis();
	    actual = end - start;
	    if( (actual > (time - TOLERANCE)) &&
		(actual < (time + TOLERANCE)) )
	    {
		System.out.println("Okie");
	    }
	    else
	    {
		System.out.println("Fail "
				   + end
				   + " - "
				   + start
				   + " = "
				   + actual);
	    }
	}

    }
}

/* Expected Output:
Okie
Okie
*/
