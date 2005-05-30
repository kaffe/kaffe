
import kaffe.lang.PackageHelper;
import gnu.classpath.SystemProperties;

/**
 * Test to make sure a user level class can't access kaffe internal classes.
 */
class KaffeInternal
{
    public static void main(String args[])
    {
	try
	{
            PackageHelper.getPackageName (KaffeInternal.class);
	}
	catch(Throwable th)
	{
	    System.out.println(th);
	}

        try
        {
            SystemProperties.getProperty("java.home");
        }
        catch(Throwable th)
        {
            System.out.println(th);
        }
    }
}

/* Expected Output:
java.lang.NoClassDefFoundError: kaffe/lang/PackageHelper
java.lang.NoClassDefFoundError: gnu/classpath/SystemProperties
*/
