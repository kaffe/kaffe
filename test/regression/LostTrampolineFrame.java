
import java.util.Vector;

import java.io.File;
import java.io.FileInputStream;

/**
 * This class will check for a bug in the x86 trampoline (and possibly others)
 * that causes kaffe to ignore the stack frame of a routine that jumped through
 * a trampoline.  By "ignore", we mean that an exception handler in the calling
 * routine was not found and executed.  This can occur because the trampoline
 * failed to setup a real stack frame, therefore, the stack walker will assume
 * caller's frame belongs to the trampoline and ignore it when trying to find
 * a handler.
 *
 * Testing is a bit tricky because we have to cause an exception to be thrown
 * while the trampoline is still on the stack, which is very dependent on the
 * jitter.  Currently, we take advantage of static initializers since they will
 * be called by soft_fixup_trampoline when trying to process the class to
 * completion.
 */
class LostTrampolineFrame
    extends ClassLoader
{
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

	if( !name.startsWith("LostTrampolineFrame") )
	{
	    /* System class... */
	    retval = super.findSystemClass(name);
	}
	else
	{
	    /* One of our class'. */
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
	if( resolve )
	    super.resolveClass(retval);
	return retval;
    }

    /**
     * The class used to generate the exception.
     */
    public static class DamagedClass
    {
	static {
	    Object ref = null;

	    /* Cause a NullPointerException to be thrown. */
	    ref.notifyAll();
	}

	/*
	 * A static method to call that will force the class to be processed
	 * to completion.
	 */
	public static void hurtme()
	{
	}
    }

    /**
     * An intermediate class that will try to construct a DamagedClass object.
     */
    public static class IntermediateClass
	implements Runnable
    {
	public IntermediateClass()
	{
	}

	public void run()
	{
	    try
	    {
		/*
		 * Use a static method in DamagedClass to cause a 'first use'
		 * and force the class to be processed to completion.  For this
		 * to work correctly, we need the processing to be done in
		 * soft_fixup_trampoline and nowhere else...
		 */
		DamagedClass.hurtme();
	    }
	    catch(ExceptionInInitializerError e)
	    {
		/*
		 * If the error is not caught here, that means the
		 * trampoline is hiding the stack frame.
		 */
		System.out.println("Success");
	    }
	}
    }

    public static void main(String args[])
	throws Throwable
    {
	LostTrampolineFrame ltf;
	Runnable run;
	Class cl;
	
	ltf = new LostTrampolineFrame();
	cl = ltf.loadClass("LostTrampolineFrame$IntermediateClass");
	try
	{
	    run = (Runnable)cl.newInstance();
	    run.run();
	}
	catch(ExceptionInInitializerError e)
	{
	    /*
	     * If the error is caught here, that means the trampoline is
	     * hiding the stack frame.
	     */
	    System.out.println("Bad trampoline");
	    e.printStackTrace();
	}
    }
}

/* Expected Output:
Success
*/
