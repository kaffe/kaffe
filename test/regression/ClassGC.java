/**
 * simple test for class finalization
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.io.*;
import java.lang.reflect.*;

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

class ClassGCTest
{
	public static class HObject {
		protected void finalize() throws Throwable {
			if (!ClassGC.gotOneForF) {
				ClassGC.gotOneForF = true;
				System.out.println("Success.");
			}
		}
	}

	public static Object f = new HObject();

	/* Make sure interfaces are GC'd also */
	public interface HInterface {
		void func();
	}

	public static class HImplementor implements HInterface {
		public void func()
		{
		}

		protected void finalize() throws Throwable {
			if (!ClassGC.gotOneForG) {
				ClassGC.gotOneForG = true;
				System.out.println("Success.");
			}
		}
	}

	public static Object g = new HImplementor();
}

class ClassGCTestLater
{
    public ClassGCTestLater() throws Exception
    {
	Class c = ClassGCTest.class;
	String s = c.getName();
	if (!s.equals("ClassGCTest"))
	    System.out.println("Failure: name is " + s);
	/* I think getConstructor should be enough, since we're in the
           same package, but it fails :-(  -oliva */
	Constructor cc = c.getDeclaredConstructor(new Class [] {});
	if (!cc.toString().equals("ClassGCTest()"))
	    System.out.println("Failure: name is " + cc.toString());
	if (!cc.newInstance(new Object[] {}).
		toString().startsWith("ClassGCTest")) {
	    System.out.println("Failure newInstance.");
	}
    }
}


/* Expected Output:
Success.
Success.
*/
