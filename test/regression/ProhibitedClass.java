
import java.io.InputStream;

/**
 * Make sure a user level class loader can't define things in the bootstrap
 * packages 'java' and 'kaffe'.
 */
class ProhibitedClass
    extends ClassLoader
{
    public static void main(String args[])
	throws Throwable
    {
	ProhibitedClass pc = new ProhibitedClass();
	InputStream is;
	byte bc[] = new byte[32 * 1024];
	int len;

	is = ClassLoader.getSystemResourceAsStream("java/lang/Object.class");
	len = is.read(bc);
	pc.defineClass("java.lang.Object", bc, 0, len);
    }
}

/* Expected Output:
java.lang.SecurityException: Prohibited package: java/lang/Object
   at java.lang.VMClassLoader.defineClass (VMClassLoader.java)
   at java.lang.ClassLoader.defineClass (ClassLoader.java:473)
   at java.lang.ClassLoader.defineClass (ClassLoader.java:438)
   at ProhibitedClass.main (ProhibitedClass.java:21)
*/
