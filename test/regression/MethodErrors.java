
import a.*;

class MethodErrors
{
    private static boolean verbose = false;

    public static void test0()
    {
	new DefinedToAbstract() {
	    }.method();
    }
    
    public static void test1()
    {
	InstToStatic fits;
	
	fits = new InstToStatic();
	fits.method();
    }
    
    public static void test2()
    {
	StaticToInst.method();
    }
    
    public static void test3()
    {
	PublicToPrivate test;

	test = new PublicToPrivate();
	test.method();
    }
    
    public static void test3_static()
    {
	PublicToPrivate.static_method();
    }
    
    public static void test4()
    {
	PublicToPackage test;

	test = new PublicToPackage();
	test.method();
    }
    
    public static void test4_static()
    {
	PublicToPrivate.static_method();
    }
    
    public static void test5()
    {
	PublicToProtected test;

	test = new PublicToProtected();
	test.method();
    }
    
    public static void test5_static()
    {
	PublicToProtected.static_method();
    }
    
    public static void test6()
    {
	new ProtectedToPrivate() {
		public void run()
		{
		    this.method();
		}
	    }.run();
    }
    
    public static void test6_static()
    {
	new ProtectedToPrivate() {
		public void run()
		{
		    static_method();
		}
	    }.run();
    }
    
    public static void test7()
    {
	DefinedToUndefined test;

	test = new DefinedToUndefined();
	test.method();
    }
    
    public static void test7_static()
    {
	DefinedToUndefined.static_method();
    }
    
    public static void test8()
    {
	IntToFloat test;

	test = new IntToFloat();
	test.method();
    }
    
    public static void test8_static()
    {
	IntToFloat.static_method();
    }

    public static void test9()
    {
	ClassToInterface cti;

	cti = new ClassToInterface();
    }
    
    public static void main(String args[])
    {
	try
	{
	    test0();
	}
	catch(AbstractMethodError e)
	{
	    System.out.println("0.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test1();
	}
	catch(IncompatibleClassChangeError e)
	{
	    System.out.println("1.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test2();
	}
	catch(IncompatibleClassChangeError e)
	{
	    System.out.println("2.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test3();
	}
	catch(IncompatibleClassChangeError e)
	{
	    System.out.println("3.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test3_static();
	}
	catch(IncompatibleClassChangeError e)
	{
	    System.out.println("3.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test4();
	}
	catch(IncompatibleClassChangeError e)
	{
	    System.out.println("4.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test4_static();
	}
	catch(IncompatibleClassChangeError e)
	{
	    System.out.println("4.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test5();
	}
	catch(IncompatibleClassChangeError e)
	{
	    System.out.println("5.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try { ((Object)null).toString(); } catch(NullPointerException e) { }
	try
	{
	    test5_static();
	}
	catch(IncompatibleClassChangeError e)
	{
	    System.out.println("5.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test6();
	}
	catch(IncompatibleClassChangeError e)
	{
	    System.out.println("6.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test6_static();
	}
	catch(IncompatibleClassChangeError e)
	{
	    System.out.println("6.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test7();
	}
	catch(NoSuchMethodError e)
	{
	    System.out.println("7.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test7_static();
	}
	catch(NoSuchMethodError e)
	{
	    System.out.println("7.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test8();
	}
	catch(NoSuchMethodError e)
	{
	    System.out.println("8.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test8_static();
	}
	catch(NoSuchMethodError e)
	{
	    System.out.println("8.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test9();
	}
	catch(InstantiationError e)
	{
	    System.out.println("9.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
    }
}

/* Expected Output:
0.1 Success.
1.1 Success.
2.1 Success.
3.1 Success.
3.2 Success.
4.1 Success.
4.2 Success.
5.1 Success.
5.2 Success.
6.1 Success.
6.2 Success.
7.1 Success.
7.2 Success.
8.1 Success.
8.2 Success.
9.1 Success.
*/
