/*
 * Test for concurrency in ClassLoaders
 * 
 * Note that this test needs a second public class which is in 
 * CLTestConc_run.java
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */
import java.io.File;
import java.io.FileInputStream;

public class CLTestFindLoaded extends ClassLoader
{
    static boolean verbose = false;
    static Thread t1, t2;
    static CLTestFindLoaded first;
    static CLTestFindLoaded second;

    protected synchronized Class loadClass(String name, boolean resolve)
	throws ClassNotFoundException
    {
	Thread current = Thread.currentThread();
	Class c = null;
	if (verbose)
	    System.out.println(current + " asked to load " + name);

	if (current == t2) {
	    System.out.println("Wrong. T2 shouldn't ask anything.");
	}

	if (this == first) {
		c = second.loadClass(name, resolve);
		if (name.startsWith("CLTestConc_bis")) {
			c = null;
		}
		Class fc = findLoadedClass(name);
		if (fc != null)
		    System.out.println("Wrong.  I did not load " + name);
	} else {
	    if (name.startsWith("CLTestConc")) {
		byte [] b = null;
		try {
		    b = readin(name + ".class");
		} catch (Exception e) {
		    System.out.println(e);
		}
		c = defineClass(name, b, 0, b.length);

		Class fc = findLoadedClass(name);
		if (verbose)
		    System.out.println("No. 2 loaded " + name + " " + fc);
		if (fc == null)
		    System.out.println("Wrong: No. 2" + name);
		fc = first.findLoadedClass(name);
		if (verbose)
		    System.out.println("No. 1 loaded " + name + " " + fc);
		if (fc != null)
		    System.out.println("Wrong: No. 1" + name);
	    } else {
		c = findSystemClass(name);
		Class fc = findLoadedClass(name);
		if (fc != null)
		    System.out.println("Wrong: I loaded " + name + " " + fc);
	    }
	}
	if (resolve && c != null)
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
    
    private static class Loader extends Thread {
	Class c;
	Loader(Class c) { 
	    this.c = c; 
	}
	public void run() {
	    setName("Success.");
	    try {
		System.out.println(c.newInstance());
	    } catch (Exception a) {
		System.out.println(a);
	    }
	}
    }
    public static void main(String []av) throws Exception
    {
	verbose = av.length > 0;
	first = new CLTestFindLoaded();
	second = new CLTestFindLoaded();
	final Class c = first.loadClass("CLTestConc_run", true);
	t1 = new Loader(c);
	t1.start();
	t1.join();
	if (first.findLoadedClass("CLTestConc_bis") != null)
	    System.out.println("Wrong 4.");
	t2 = new Loader(c);
	t2.start();
	t2.join();
    }
}

// sources: CLTestConc_run.java
/* Expected Output:
Success.
Success.
*/
