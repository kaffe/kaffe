import java.io.*;

/**
 * Can we gc classes which where once accessible from multiple
 * loaders, or does something die in the process.
 *
 * @author <a href=mailto://jbaker@cs.utah.edu> Jason Baker </a>
 */
public class CLTestDelegate extends ClassLoader {
	static boolean firstDead = false;
	static boolean secondDead = false;
	public static boolean verbose = false;

	static String CNAME = "CLTestDelegate$C";
	static Class def = null;
	static byte[] bytes = readin(CNAME + ".class");

	static String DNAME = "CLTestDelegate$D";
	public static class C {
	}

	public static class D {
		static C dummy = new C();
		static {
			if (verbose)
				System.err.println("D is loaded");
		}
	}

	boolean amFirst = true;

	protected Class loadClass(String name, boolean resolve)
		throws ClassNotFoundException
	{
		Class c;
		if (name.equals(CNAME)) {
			if (def == null) 
				def = defineClass(bytes, 0, bytes.length);
			else
				amFirst = false;
			c = def;
		} else if (name == DNAME) {
			byte[] dBytes = readin(DNAME + ".class");
			c = defineClass(dBytes, 0, dBytes.length);
		} else {
			 c = findSystemClass(name);
		}
		resolveClass(c);
		return c;
	}

	static byte [] readin(String name) 
	{
		try {
			File cf = new File(name);
			FileInputStream cfi = new FileInputStream(cf);

			int len = (int)cf.length();
			byte [] cb = new byte[len];
			if (cfi.read(cb) != len) {
				System.err.println("short read for " + name);
				throw new RuntimeException();
			}
			return cb;
		} catch (IOException e) {
			System.err.println("couldn't read " + name
					   + ": " + e.getMessage());
			throw new RuntimeException();
		}
	}

	protected void finalize() {
		if (verbose)
			System.err.println((amFirst ? "first " : "second ")
					   + "dying");
		if (amFirst)
			firstDead = true;
		else
			secondDead = true;
	}

	/*
	public static String trashStack(int count)
	{
		if (count > 0)
			return "trash: " + trashStack(count - 1);
		else
			return "gone from stack";
	}

	static String dummy;
	*/
	public static void doit()
	{
		try {
			CLTestDelegate ck;

			// The first loader defines a class
			ck = new CLTestDelegate();
			int hk = System.identityHashCode(ck);
			ck.loadClass(DNAME).newInstance();

			// The second loader delagates to the first
			ck = new CLTestDelegate();
			if (verbose)
			    System.out.println("first loader at " + hk);
			hk = System.identityHashCode(ck);
			if (verbose)
			    System.out.println("second loader at " + hk);
			ck.loadClass(DNAME).newInstance();
			ck = null;

			def = null;
		}
		catch (Exception e) {
			throw new RuntimeException("No such luck : "
						   + e.getMessage());
		}
	}

	public static void main(String[] args)
	{
		doit();

		// As of 10-25-00, it takes a minimum of 3 gcs to
		// manifest the problem, and this argument parsing
		// code seems to be needed to clean up the stack. 
		int gcCount = 6;
		int idx = 0;
		while (args.length > idx) {
			if (args[idx].equals("-c")
			    && args.length > idx + 1)
				gcCount = Integer.parseInt(args[++idx]);
			else if (args[idx] == "-v") 
				verbose = true;
			else {
				System.err.println
					("usage: CLTestDelegate "
					 + " {-c GC-COUNT}?"
					 + " -v?");
			}
			idx++;
		}

		// give time for finalizers to run, and for classes
		// our loaders kept alive to be collected
		for (int i = 0; i < gcCount; i++) 
			System.gc();
		if (!(firstDead))
			System.err.println("First ClassLoader not GCed");
		else
		if (!(secondDead))
			System.err.println("Second ClassLoader not GCed");
		else
			// If we didn't crash in destroyClass, life is
			// good. 
			System.out.println("success");
	}
}
/* Expected Output:
success
*/
