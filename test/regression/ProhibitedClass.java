
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
	try {
	  pc.defineClass("java.lang.Object", bc, 0, len);
	} catch (SecurityException e) {
	  if (e.getMessage().startsWith("Prohibited package"))
		  System.out.println("Success.");
	  else
		  e.printStackTrace();
	}
    }
}

/* Expected Output:
Success.
*/
