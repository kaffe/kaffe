/**
 * simple test for class finalization
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.io.*;
import java.lang.reflect.*;

public class ClassGC extends ClassLoader {
    // Make sure our helper classers are compiled from the test script
    static Class imp1 = ClassGCTest.class;
    static Class imp2 = ClassGCTestLater.class;

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

    static boolean verbose = false;

    public Class loadClass(String name, boolean resolve) 
	throws ClassNotFoundException 
    {
	Class c = null;
	if (!name.startsWith("ClassGCTest")) {
	    if (verbose)
		System.out.println("finding " + name);
	    return findSystemClass(name);
	} else {
	    if (verbose)
		System.out.println("loading " + name);
	    try {
		byte []b = readin(name + ".class");
		return defineClass(name, b, 0, b.length);
	    } catch (Exception e) {
		System.out.println(e);
		return null;
	    }
	}
    }

    /*
     * load a class and create an instance, but drop the class
     */
    static ClassLoader doit(String testclass) throws Exception {
	ClassGC l = new ClassGC();
	l.loadClass(testclass).newInstance();
	return l;
    }

    static void cleanup() {
	System.gc();
	System.runFinalization();
    }

    public static void main(String av[]) throws Exception {
	verbose = av.length > 0;
	String testclass = "ClassGCTest";

	for (int i = 0; i < 10; i++) {
	    ClassLoader l = doit(testclass);
	    cleanup();
	    /* this class refers to the former to which we didn't keep
	     * a ref.  If our class gc is too eager, it will have freed it
	     * and bad things will happen.
	     */
	    Class c = l.loadClass("ClassGCTestLater");
	    c.newInstance();
	}
    }
    public static boolean gotOneForF;
    public static boolean gotOneForG;
}

// Sources: ClassGCTest.java ClassGCTestLater.java
/* Expected Output:
Success.
Success.
*/
