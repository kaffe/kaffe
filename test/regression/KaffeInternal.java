
import kaffe.lang.ThreadStack;

/**
 * Test to make sure a user level class can't access kaffe internal classes.
 */
class KaffeInternal
{
    public static void main(String args[])
    {
	try
	{
	    ThreadStack.getClassStack();
	}
	catch(Throwable th)
	{
	    System.out.println(th);
	}
    }
}

/* Expected Output:
java.lang.NoClassDefFoundError: kaffe/lang/ThreadStack
*/
