/*
 * Test for ClassLoader
 *
 * Tests simple call of defineClass and various error conditions resulting
 * from repeated calls to defineClass
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */
import java.io.File;
import java.io.FileInputStream;

public class CLTest extends ClassLoader
{
    public static class Hello
    {
	public Hello()
	{
	    System.out.println("Hello World");
	}
    }

    protected synchronized Class loadClass(String name, boolean resolve)
	throws ClassNotFoundException
    {
	Class c = findSystemClass(name);
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
    
    /*
     * load class using 4 parameter interface
     */
    Class doit4(String name, byte [] cb) throws Exception
    {
	return defineClass(name, cb, 0, cb.length);
    }

    /*
     * load class using 3 parameter interface
     * This is deprecated, but we still want to test it
     */
    Class doit3(byte [] cb) throws Exception
    {
	return defineClass(cb, 0, cb.length);
    }

    public static void main(String []av) throws Exception
    {
	String secondName = "b";

	CLTest mcl = new CLTest();
	Class a = null;
	byte [] cb = readin("CLTest$Hello.class");

	try {
	    a = mcl.doit3(cb);
	    if (a != null) {
		System.out.println("Load returned class -" + a.getName() + "-");
		System.out.println("Trying to create new instance");
		a.newInstance();
	    } else {
		System.out.println("Load failed!");
	    }
	} catch (Throwable e) {
	    System.out.println("This failed: " + e);
	    e.printStackTrace(System.out);
	}

	try {
	    /* this is the same .class file, load will fail with
	     * Duplicate name
	     */
	    System.out.println("Trying same file under different name");
	    Class b = mcl.doit3(cb);
	} catch (Throwable e) {
	    System.out.println(e.getMessage());
	}

	try {
	    System.out.println("Trying file with wrong name");
	    Class b = mcl.doit4(secondName, cb);
	} catch (Throwable e) {
	    System.out.println(e.getMessage());
	}
  
	try {
	    System.out.println("Now second load attempt for a");
	    Class b = mcl.doit4(a.getName(), cb);
	    b.newInstance();
	} catch (Throwable e) {
	    System.out.println(e.getMessage());
	}
	System.out.println("Done.");
    }
}

// javac flags: -nowarn
/* Expected Output:
Load returned class -CLTest$Hello-
Trying to create new instance
Hello World
Trying same file under different name
Duplicate name
Trying file with wrong name
Wrong name
Now second load attempt for a
Duplicate name
Done.
*/
