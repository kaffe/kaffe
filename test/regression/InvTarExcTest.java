import java.lang.reflect.*;

public class InvTarExcTest
{
    public static void f() throws Exception {
	throw new Exception("throwing inner exception");
    }

    public static void main(String av[]) {
	try {
	    Class c = Class.forName("InvTarExcTest");
	    Method m = c.getMethod("f", new Class[0]);
	    m.invoke(null, null);
	} catch (InvocationTargetException e) {
	    Throwable t = e.getTargetException();
	    System.out.println("Caught itexception: " + e.getMessage());
	    // e.printStackTrace(System.out);
	    System.out.println("Target is: " + t.getMessage());
	    // t.printStackTrace(System.out);
	} catch (Throwable t) {
	    System.out.println("Caught throwable: " + t);
	    t.printStackTrace(System.out);
	    System.out.println("This should not happen.");
	}
    }
}

/* Expected Output:
Caught itexception: null
Target is: throwing inner exception
*/
