
public class BadClassVersion
    extends ClassLoader
{
    public static byte badByteCode[] = {
        (byte)0xca, (byte)0xfe, (byte)0xba, (byte)0xbe,
        (byte)0x04, (byte)0x3, (byte)0x05, (byte)0x2d,
    };

    public static void main(String args[])
    {
	try
	{
	    BadClassVersion bcv;

	    bcv = new BadClassVersion();
	    bcv.defineClass("BrokenClass",
			    badByteCode,
			    0,
			    badByteCode.length);
	    System.out.println("Version number wasn't checked?");
	}
	catch(UnsupportedClassVersionError e)
	{
	    /* jdk1.2+ */
	    System.out.println("Success");
	}
	catch(ClassFormatError e)
	{
	    /* Pre jdk1.2 */
	    System.out.println("Success");
	}
    }
}

/* Expected Output:
Success
*/
