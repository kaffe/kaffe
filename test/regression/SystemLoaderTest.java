/*
 * Check that our system class loader doesn't initialize classes too early
 */

class l {
	static {
		System.out.println("l.init invoked");
	}
}

class k extends l {
	static {
		System.out.println("k.init invoked");
	}
}

public class SystemLoaderTest extends ClassLoader {

    public static void main(String []av) throws Exception {
	Class c = new SystemLoaderTest().loadClass("k");
	System.out.println("LOADED");
	c.newInstance();
	System.out.println("DONE");
    }
}

/* Expected Output:
LOADED
l.init invoked
k.init invoked
DONE
*/

