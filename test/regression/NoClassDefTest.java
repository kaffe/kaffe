
import java.io.File;
import java.io.FileInputStream;

/**
 * Test for NoClassDefFoundErrors when ClassLoaders are involved.
 */
public class NoClassDefTest
    extends ClassLoader
{
    /**
     * A class that is visible during the compile, but will be missing at run
     * time.
     */
    public static class MissingClass
    {
	/**
	 * A static field we can reference to force a NoClassDefFoundError.
	 */
	public static int foo = 1;

	/**
	 * A method we can reference to force a NoClassDefFoundError.
	 */
	public MissingClass()
	{
	}
    }

    /**
     * This is an intermediate class we use to reference the missing class'
     * method.
     */
    public static class MethodReferenceClass
	implements Runnable
    {
	public MethodReferenceClass()
	{
	}

	public void run()
	{
	    try
	    {
		new MissingClass();
	    }
	    catch(NoClassDefFoundError e)
	    {
		System.out.println("Caught NoClassDefError for method "
				   + "MissingClass.<init> in class: "
				   + e.getMessage());
	    }
	}
    }

    /**
     * This is an intermediate class we use to reference the missing class'
     * field.
     */
    public static class FieldReferenceClass
	implements Runnable
    {
	public FieldReferenceClass()
	{
	}

	public void run()
	{
	    try
	    {
		MissingClass.foo = 1;
	    }
	    catch(NoClassDefFoundError e)
	    {
		System.out.println("Caught NoClassDefError for field "
				   + "MissingClass.foo in class: "
				   + e.getMessage());
	    }
	}
    }

    /**
     * This is an intermediate class we use to test a cast of missing class.
     */
    public static class CastReferenceClass
	implements Runnable
    {
	public CastReferenceClass()
	{
	}

	public void run()
	{
	    try
	    {
		Object obj = null;

		obj = (MissingClass)obj;
	    }
	    catch(NoClassDefFoundError e)
	    {
		System.out.println("Caught NoClassDefError for cast of class: "
				   + e.getMessage());
	    }
	}
    }

    /**
     * Read in the byte code for the given class.
     *
     * @param name The name of the class to read in.
     * @return The byte code for the class.
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
    
    public synchronized Class loadClass(String name, boolean resolve)
	throws ClassNotFoundException
    {
	Class retval = null;

	if( !name.startsWith("NoClassDefTest") )
	{
	    /* System class... */
	    retval = super.findSystemClass(name);
	}
	else if( !name.equals("NoClassDefTest$MissingClass") )
	{
	    /* One of our class' and not the MissingClass. */
	    try
	    {
		byte []b = readin(name + ".class");
		retval = defineClass(name, b, 0, b.length);
	    }
	    catch(Exception e)
	    {
		throw new ClassNotFoundException(name);
	    }
	}
	else
	{
	    throw new ClassNotFoundException(name);
	}
	if( resolve )
	    super.resolveClass(retval);
	return retval;
    }

    public static void main(String args[])
	throws Throwable
    {
	NoClassDefTest test;
	Runnable run;
	Class cl;

	/* Create our ClassLoader */
	test = new NoClassDefTest();

	/* Load the intermediate classes and force NoClassDefFoundErrors. */
	
	cl = test.loadClass("NoClassDefTest$MethodReferenceClass");
	run = (Runnable)cl.newInstance();
	run.run();
	cl = test.loadClass("NoClassDefTest$FieldReferenceClass");
	run = (Runnable)cl.newInstance();
	run.run();
	cl = test.loadClass("NoClassDefTest$CastReferenceClass");
	run = (Runnable)cl.newInstance();
	run.run();
    }
}

/* Expected Output:
Caught NoClassDefError for method MissingClass.<init> in class: NoClassDefTest$MissingClass
Caught NoClassDefError for field MissingClass.foo in class: NoClassDefTest$MissingClass
Caught NoClassDefError for cast of class: NoClassDefTest$MissingClass
*/
