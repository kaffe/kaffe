/**
 * simple test for class finalization
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.io.*;

public class ClassGC extends ClassLoader {

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

    static void doit(String testclass) throws Exception {
	ClassGC l = new ClassGC();
	l.loadClass(testclass).newInstance();
    }

    static void cleanup() {
	System.gc();
	System.runFinalization();
    }

    public static void main(String av[]) throws Exception {
	verbose = av.length > 0;
	String testclass = "ClassGCTest";

	for (int i = 0; i < 10; i++) {
	    doit(testclass);
	    cleanup();
	}
    }
    public static boolean gotOne;
}
