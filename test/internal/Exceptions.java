
class Exceptions
{

    public static final boolean test__boolean_exception = true;

    public static boolean boolean_exception()
    {
	boolean retval = false;

	try
	{
	    throw new Throwable();
	}
	catch(Throwable th)
	{
	    retval = true;
	}
	return retval;
    }


    public static final int test_0__int_exception = 1;

    public static int int_exception(int a)
    {
	try
	{
	    throw new Throwable();
	}
	catch(Throwable th)
	{
	    a++;
	}
	return a;
    }


    public static final int test_0__int_exception0 = 1;

    public static int int_exception0(int a)
    {
	try
	{
	    a++;
	    throw new Throwable();
	}
	catch(Throwable th)
	{}

	return a;
    }

   
    public static final int test_0__int_exception1 = 2;

    public static int int_exception1(int a)
    {
	a++;

	try
	{
	    a++;
	    throw new Throwable ();
	}
	catch (Throwable th)
	{}

	return a;
    }


    public static final int test_0__int_exception2 = 2;

    public static int int_exception2(int a)
    {
	a++;
	try
	{
	    throw new Throwable ();
	}
	catch (Throwable th)
	{
	    a++;
	}

	return a;
    }


    public static final int test_0__int_exception3 = 2;

    public static int int_exception3(int a)
    {
	try
	{
	    a++;
	    throw new Throwable ();
	}
	catch (Throwable th)
	{
	    a++;
	}
	return a;
    }
	
    public static Throwable int_exception3(Throwable th, Integer th2[])
	{
		return th;
	}

    private Exceptions()
    {
    }
}
