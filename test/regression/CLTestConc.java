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
		     * in here.  In this case, all we'll be stopped.
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

	    if (current == second) {
		if (verbose)
		    System.out.println("stopping second thread");
		current.stop();
		throw new ThreadDeath();
	    }
	} else {
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

class CLTestConc_bis {
    public String toString() {
	return Thread.currentThread().getName();
    }
}

class CLTestConc_run {
    public String toString() {
        return new CLTestConc_bis().toString();
    }
}

// javac flags: -nowarn
/* Expected Output:
Success.
*/
