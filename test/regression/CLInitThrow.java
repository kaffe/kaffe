
public class CLInitThrow
{
    static void throwError()
    {
	throw new Error();
    }
    
    static void throwRuntimeException()
    {
	throw new RuntimeException();
    }
    
    public static class ThrowError
    {
	static {
	    CLInitThrow.throwError();
	}
    }

    public static class ThrowRuntimeException
    {
	static {
	    CLInitThrow.throwRuntimeException();
	}
    }

    public static void main(String args[])
    {
	try
	{
	    new ThrowError();
	}
	catch(Error e)
	{
	    System.out.println("Caught error");
	}
	try
	{
	    new ThrowRuntimeException();
	}
	catch(ExceptionInInitializerError eiie)
	{
	    if( eiie.getException() instanceof RuntimeException )
		System.out.println("Caught runtime exception");
	}
    }
}

/* Expected Output:
Caught error
Caught runtime exception
*/
