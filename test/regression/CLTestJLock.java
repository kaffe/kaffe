/*
 * Test for ClassLoader
 *
 * This tests that we don't hold the translator lock when calling
 * loadClass.
 *
 * This test depends on CLTestConc_run.java
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */
import java.io.File;
import java.io.FileInputStream;

public class CLTestJLock extends ClassLoader
{
    protected synchronized Class loadClass(String name, boolean resolve)
	throws ClassNotFoundException
    {
	Class c = null;
	// System.out.println(Thread.currentThread() + " asked for " + name);

	if (name.equals("CLTestConc_bis")) {
	    try {
		Thread.sleep(5000);
	    } catch (InterruptedException e) { }
	}
	if (name.startsWith("CLTestConc")) {
	    byte []b = null;
	    try {
		b = readin(name + ".class");
	    } catch (Exception e) {
		e.printStackTrace();
	    }
	    c = defineClass(name, b, 0, b.length);
	} else {
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

    public static void main(String []av) throws Exception
    {
	CLTestJLock mcl = new CLTestJLock();
	final Class a = mcl.loadClass("CLTestConc_run", true);

	// start watchdog
	new Thread() {
	    public void run() {
		try {
		    Thread.sleep(9000);
		} catch (InterruptedException e) { }
		System.out.println("Time Out. Failure.");
		System.exit(-1);
	    }
	}.start();

	// start thread which will do some jitting
	new Thread() {
	    public void run() {
		try {
		    setName("Failure.");
		    String s = a.newInstance().toString();
		    System.out.println(s);
		} catch (Exception e) { 
		    e.printStackTrace();
		}
	    }
	}.start();

	// give thread a chance to sleep in loadClass when it loads
	// CLTestConc_bis
	try {
	    Thread.sleep(1000);
	} catch (InterruptedException e) { }

	// if the other thread holds the translator lock while in
	// loadClass, this will not complete immediately, though it should.
	new Thread() {
	    public void run() {
		System.out.println("Success.");
		System.exit(0);
	    }
	}.start();
    }
}

// sources: CLTestConc_run.java
/* Expected Output:
Success.
*/
