/**
 * Check that stack overflows are handled properly in main thread and
 * other threads.
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.io.*;

class OverflowTester extends Thread
{
    public void run() {
	try {
	    recurse();
	} catch (StackOverflowError e) {
	    System.out.println("Success.");
	}
    }

    void recurse() {
	recurse();
    }

    public OverflowTester() throws Exception
    {
	    run();	// check for main thread
		    
	    start();	// check for other thread
	    join();
    }
}

class Overflow extends ClassLoader {
	
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

	public Class loadClass(String name, boolean resolve) 
		throws ClassNotFoundException 
	{
		Class c = null;
	
		try {
			byte []b = readin(name + ".class");
			return defineClass(name, b, 0, b.length);
		} catch (Exception e) {
			return findSystemClass(name);
		}
	}
	
	public static void main(String av[]) throws Exception
	{
		OverflowTester oft;
		Overflow l = new Overflow();
		Class c;
		
		c = l.loadClass("OverflowTester");
		c.newInstance();
		oft = new OverflowTester();
	}
	
}

/* Expected Output:
Success.
Success.
Success.
Success.
*/
