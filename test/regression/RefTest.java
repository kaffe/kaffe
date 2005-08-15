/*
 * Simple test for basic java references capabilities of the VM.
 *
 * See "licence.terms" for information on usage and redistribution of this 
 * file.
 *
 * @author Guilhem Lavaux <guilhem@kaffe.org>
 */
import java.lang.ref.*;

public class RefTest
{
	static ReferenceQueue q = new ReferenceQueue();

  static class Watchdog extends Thread
  {
    public int m_timeout;

    public Watchdog(int timeout)
    {
      m_timeout = timeout;
    }

    public void run()
    {
      boolean repeat;

      do
	{
	  try
	    {
	      repeat = false;
	      Thread.sleep(m_timeout);
	    }
	  catch (InterruptedException _)
	    {
	      repeat = true;
	    }
	}
      while (repeat);

      System.out.println("Watchdog timeout !");
      System.exit(-1);
    }
  }

  static class TestObject
  {
    public void finalize()
    {
      System.out.println("finalizer called");
    }
  }

  static class RefBuilder extends Thread
  {
    WeakReference ref;
    Class cls;

    public void run()
    {
      try
	{
	  ref = new WeakReference(cls.newInstance(), q);
	}
      catch (Exception e)
	{
	  System.out.println("Invalid exception " + e);
	  e.printStackTrace();
	}
    }
  }

  static WeakReference buildRef()
  {
    RefBuilder rb = new RefBuilder();

    rb.cls = Object.class;
    rb.start();
    try
      {
	rb.join();
      }
    catch (InterruptedException _)
      {
      }
    return rb.ref;
  }

  static WeakReference buildRefWithFinal()
  {
    RefBuilder rb = new RefBuilder();
    
    rb.cls = TestObject.class;
    rb.start();
    try
      {
	rb.join();
      }
    catch (InterruptedException _)
      {
      }
    return rb.ref;
  }

  static public void main(String args[]) throws Exception
  {
    WeakReference ref = buildRef(); 
    Watchdog dog = new Watchdog(10000);

    dog.start();

    while (!ref.isEnqueued())
      System.gc();

    System.gc();

    if (ref.get() == null)
      System.out.println("Reference cleared");
    else
      System.out.println("Reference not cleared");

    ref = buildRefWithFinal();
    while (!ref.isEnqueued())
      System.gc();

    System.gc();
    System.gc();
    System.gc();

    if (ref.get() == null)
      System.out.println("Reference cleared");
    else
      System.out.println("Reference not cleared");

    System.exit(0);
  }
}
// Sort Output
/* Expected Output:
Reference cleared
Reference cleared
finalizer called
*/
