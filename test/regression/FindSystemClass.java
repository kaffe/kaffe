import java.lang.reflect.*;

class BaseFSC {
    static {
	System.out.println("findSystemClass initializes me.");
    }
    public static void staticMethod() {
	System.out.println("staticMethod invoked.");
    }
};

public class FindSystemClass {
    public static void main(String av[]) throws Exception {
	ClassLoader cl = new ClassLoader() {
	    public Class loadClass(String name, boolean b) 
	    	throws ClassNotFoundException
	    {
	    	return findSystemClass(name);
	    }
	};
	Class c = cl.loadClass("BaseFSC");
	Method m = c.getMethod("staticMethod", new Class[] {});
	m.invoke(null, null);
    }
}

/* Expected Output:
findSystemClass initializes me.
staticMethod invoked.
*/
