/*
 * Check that our system class loader doesn't initialize classes too early
 */

class SLTest_class1 {
	static {
		System.out.println("SLTest_class1.init invoked");
	}
}

class SLTest_class2 extends SLTest_class1 {
	static {
		System.out.println("SLTest_class2.init invoked");
	}
}

public class SystemLoaderTest extends ClassLoader {

    public static void main(String []av) throws Exception {
	Class c = new SystemLoaderTest().loadClass("SLTest_class2");
	System.out.println("LOADED");
	c.newInstance();
	System.out.println("DONE");
    }
}

/* Expected Output:
LOADED
SLTest_class1.init invoked
SLTest_class2.init invoked
DONE
*/

