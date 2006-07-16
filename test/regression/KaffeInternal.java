
import org.kaffe.lang.UNIXProcess;
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
            UNIXProcess.sendSignal(0,0);
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
java.lang.NoClassDefFoundError: org/kaffe/lang/UNIXProcess
java.lang.NoClassDefFoundError: gnu/classpath/SystemProperties
*/
