/**
 * Test for Inner classes and related methods in class Class.
 *
 * @author Edouard G. Parmelan <egp@free.fr>
 */

import java.lang.reflect.Modifier;

/**
 * Super class of InnerTest that define some inner classes and interfaces.
 */
class InnerBase
{
    private class privateInstanceClass
    {
	public void run() {
	    System.out.println ("InnerBase.privateInstanceClass");
	}

	public void start() { run(); }
    }

    private static class privateStaticClass
    {
	public void run() {
	    System.out.println ("InnerBase.privatStaticClass");
	}

	public void start() { run(); }
    }

    protected class protectedInstanceClass
    {
	public void run() {
	    System.out.println ("InnerBase.protectedInstanceClass");
	}

	public void start() { run(); }
    }

    protected static class protectedStaticClass
    {
	public void run() {
	    System.out.println ("InnerBase.privatStaticClass");
	}

	public void start() { run(); }
    }

    public class publicInstanceClass
    {
	public void run() {
	    System.out.println ("InnerBase.publicStaticClass");
	}

	public void start() { run(); }
    }

    public static class publicStaticClass
    {
	public void run() {
	    System.out.println ("InnerBase.publicStaticClass");
	}

	public void start() { run(); }
    }

    public static interface publicStaticInterface
    {
	public void run();
    }

    public interface publicInstanceInterface
    {
	public void run();
    }
}


public class InnerTest
    extends InnerBase
{
    public static class Inner
    {
    }

    private static void printClass(String prefix, Class clazz)
    {
	if (clazz == null) {
	    System.out.println (prefix + "<null>");
	}
	else {
	    int mods = clazz.getModifiers();
	    System.out.println (prefix + clazz + " [" + Integer.toString(mods, 16) + "] " + Modifier.toString (mods));
	}
    }

    private static void printClasses(String prefix, Class[] classes)
    {
	if (classes.length == 0) {
	    System.out.println (prefix + "<empty>");
	}
	for (int i = classes.length; i-- > 0; ) {
	    printClass (prefix, classes[i]);
	}
    }

    public static void main(String args[])
    {
	if (args.length == 0) {
	    args = new String[] { "java.lang.Object", "InnerBase", "InnerTest" };
	}

	for (int i = 0; i < args.length; i++) {
	    String name = args[i];
	    if (name.endsWith (".class")) {
		name = name.substring (0, name.length() - 6);
	    }
	    try {
		Class clazz = Class.forName (name);

		printClass (name + ": declaring: ", clazz.getDeclaringClass());
		printClasses (name + ": declared: ", clazz.getDeclaredClasses());
		printClasses (name + ": get: ", clazz.getClasses());
	    }
	    catch (Throwable t) {
		t.printStackTrace(System.out);
	    }
	}
    }

    void useAnonymous()
    {
	new protectedInstanceClass() { public void run() {} }.start();
	new protectedStaticClass() { public void run() {} }.start();
	new publicInstanceClass() { public void run() {} }.start();
	new publicStaticClass() { public void run() {} }.start();
    }

}

// Sort output
/* Expected Output:
InnerBase: declared: class InnerBase$privateInstanceClass [2] private
InnerBase: declared: class InnerBase$privateStaticClass [a] private static
InnerBase: declared: class InnerBase$protectedInstanceClass [4] protected
InnerBase: declared: class InnerBase$protectedStaticClass [c] protected static
InnerBase: declared: class InnerBase$publicInstanceClass [1] public
InnerBase: declared: class InnerBase$publicStaticClass [9] public static
InnerBase: declared: interface InnerBase$publicInstanceInterface [609] public abstract static interface
InnerBase: declared: interface InnerBase$publicStaticInterface [609] public abstract static interface
InnerBase: declaring: <null>
InnerBase: get: class InnerBase$publicInstanceClass [1] public
InnerBase: get: class InnerBase$publicStaticClass [9] public static
InnerBase: get: interface InnerBase$publicInstanceInterface [609] public abstract static interface
InnerBase: get: interface InnerBase$publicStaticInterface [609] public abstract static interface
InnerTest: declared: class InnerTest$Inner [9] public static
InnerTest: declaring: <null>
InnerTest: get: class InnerBase$publicInstanceClass [1] public
InnerTest: get: class InnerBase$publicStaticClass [9] public static
InnerTest: get: class InnerTest$Inner [9] public static
InnerTest: get: interface InnerBase$publicInstanceInterface [609] public abstract static interface
InnerTest: get: interface InnerBase$publicStaticInterface [609] public abstract static interface
java.lang.Object: declared: <empty>
java.lang.Object: declaring: <null>
java.lang.Object: get: <empty>
*/
