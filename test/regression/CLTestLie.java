/*
 * Test for ClassLoader
 *
 * Tests what happens if a classloader lies and returns a class with the
 * wrong name.
 *
 * This test relies on CLTestConc_run.java and CLTestConc_bis.java
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */
import java.io.File;
import java.io.FileInputStream;

public class CLTestLie extends ClassLoader
{
    protected synchronized Class loadClass(String name, boolean resolve)
	throws ClassNotFoundException
    {
	Class c = null;

	if (name.equals("CLTestConc_bis")) {
		name = "java.lang.Object";	// whatever
	}
	if (name.equals("CLTestConc_run")) {
		try {
			byte [] b = readin(name + ".class");
			c = defineClass(name, b, 0, b.length);
		} catch (Exception e) {
			e.printStackTrace();
		}
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
	CLTestLie mcl = new CLTestLie();
	Class a = mcl.loadClass("CLTestConc_run", true);
	Object o = a.newInstance();
	try {
	    System.out.println(o);
	    System.out.println("Failure");
	} catch (NoClassDefFoundError e) {
	    System.out.println("Success.");
	    // System.out.println(e);
	}
    }
}

// sources: CLTestConc_run.java
/* Expected Output:
Success.
*/
