/*
 * Test for concurrency in ClassLoaders
 * 
 * Note that this test needs a second public class which is in 
 * CLTestConc_run.java
 *
 * This test tests that the VM can safely recover from threads stopping 
 * while loading classes via loadClass.
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */
import java.io.File;
import java.io.FileInputStream;

public class CLTestConc extends ClassLoader
{
    static boolean verbose = false;
    Thread	first, second;

    /*
     * We first create one loader and load "CLTestConc_run".
     * Then we start three threads that attempt to create a new instance 
     * of that class.  CLTestConc_run references CLTestConc_bis.
     *
     * We must ensure several things.
     * First, we must ensure that the VM does not call out to a custom loader
     * while another call is in progress.
     * 
     * Second, we must ensure that the VM attempts to
     * load a class after its first attempt failed because the thread
     * executing the loadClass method died.
     *
     * Third, we must ensure that a class is considered loaded (and all
     * threads waiting for it) are resumed as soon as the class has been
     * defined via defineClass.
     *
     * We use three threads, which can execute in arbitrary order.
     * Thread 1 is the first one trying to load the class.
     * We make it wait for 2 seconds and the die *before* calling defineClass.
     * Thread 2 is the second thread trying to load the class.
     * It loads the class's bytes and defines it via defineClass, but it
     * dies before it has a chance to return from loadClass.
     * Thread 3 just sits and waits until the class is loaded.  Thread 3
     * is unblocked as soon as thread 2 defines the class and returns
     * from loadClass (except, of course, it doesn't return---but loadClass
     * catches ThreadDeath)
     *
     * Thread 3, being the only one alive, will then move on to resolve
     * references of CLTestConc_bis to other classes.  Once that happens,
     * we set its name to "Success", and because thread 3 is the only
     * surviving thread, this is all that's printed.
     */
    protected synchronized Class loadClass(String name, boolean resolve)
	throws ClassNotFoundException
    {
	Thread current = Thread.currentThread();
	if (verbose)
	    System.out.println(current + " asked to load " + name);

	if (name.equals("CLTestConc_bis")) {
	    if (first == null) {
		if (verbose)
		    System.out.println("first thread asked to load " + name);
		current.setName("Failure 1.");
		try {
		    /* A wrong implementation might now send the other threads
		     * in here.  In this case, all will be stopped.
		     */
		    wait(2000);
		} catch (InterruptedException _) { }
		first = current;

		if (verbose)
		    System.out.println("stopping first thread");
		// interestingly, JDK 1.2 doesn't throw a ThreadDeath when
		// I call stop here --- it just ignores it.
		current.stop();
		if (verbose)
		    System.out.println("first thread was not stopped???");
		// this convinces even 1.2 that I want out.
		throw new ThreadDeath();
	    } else 
	    if (second == null) {
		if (verbose)
		    System.out.println("second thread asked to load " + name);
		second = current;
		/* We will stop the second thread below.  Like the first thread,
		 * it is another unlucky candidate trying to load this class
		 */
		second.setName("Failure 2.");
	    } else {
		if (verbose)
		    System.out.println("third thread asked to load " + name);
		System.out.println("Failure 3.");
		System.exit(1);
	    }
	}

	Class c = null;
	if (name.startsWith("CLTestConc")) {
	    byte [] b = null;
	    try {
		b = readin(name + ".class");
	    } catch (Exception e) {
		System.out.println(e);
	    }
	    c = defineClass(name, b, 0, b.length);

	    /* The second thread dies after defining the class 
	     * This should unblock the third thread.
	     */
	    if (current == second) {
		if (verbose)
		    System.out.println("stopping second thread");
		current.stop();
		throw new ThreadDeath();
	    }
	} else {
	    /* The third thread, loading stuff to which CLTestConc_* refers */
	    if (current != second && second != null) {
		current.setName("Success.");
	    }
	    if (verbose)
		System.out.println(current + " is calling findSystemClass");
	    c = findSystemClass(name);
	}
	if (resolve)
	    resolveClass(c);
	return c;
    }

    /*
     * read a .class file
     */
    static byte [] readin(String name) throws Exception
    {
	File cf = new File(name);
	FileInputStream cfi = new FileInputStream(cf);

	int len = (int)cf.length();
	byte [] cb = new byte[len];
	if (cfi.read(cb) != len)
	    throw new Exception("short read for " + name);
	return cb; 
    }
    
    static class CLTestConc_loader extends Thread {
	Class c;
	CLTestConc_loader(Class c) {
	    this.c = c;
	}

	public void run() {
	    try {
		Object o = c.newInstance();
		System.out.println(o);
	    } catch (Exception e) {
		e.printStackTrace();
	    }
	}
    }

    public static void main(String []av) throws Exception
    {
	verbose = av.length > 0;
	CLTestConc loader = new CLTestConc();
	final Class c = loader.loadClass("CLTestConc_run", true);

	Thread t = new CLTestConc_loader(c);
	Thread t1 = new CLTestConc_loader(c);
	Thread t2 = new CLTestConc_loader(c);

	t.start();
	t1.start();
	t2.start();
	t.join();
	t1.join();
	t2.join();
    }
}

// javac flags: -nowarn
// sources: CLTestConc_run.java
/* Expected Output:
Success.
*/
