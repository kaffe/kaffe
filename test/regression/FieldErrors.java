
import a.*;

class FieldErrors
{
    private static boolean verbose = false;
    
    public static void test1_put()
    {
	InstToStatic fits;
	
	fits = new InstToStatic();
	fits.a = 10;
    }
    
    public static void test1_get()
    {
	InstToStatic fits;
	int a;
	
	fits = new InstToStatic();
	a = fits.a;
    }
    
    public static void test2_put()
    {
	StaticToInst fits;
	
	fits = new StaticToInst();
	fits.a = 10;
    }
    
    public static void test2_get()
    {
	StaticToInst fits;
	int a;
	
	fits = new StaticToInst();
	a = fits.a;
    }
    
    public static void test3_put()
    {
	PublicToPrivate test;

	test = new PublicToPrivate();
	test.a = 10;
    }
    
    public static void test3_get()
    {
	PublicToPrivate test;
	int a;
	
	test = new PublicToPrivate();
	a = test.a;
    }
    
    public static void test3_static_put()
    {
	PublicToPrivate.b = 10;
    }
    
    public static int test3_static_get()
    {
	return PublicToPrivate.b;
    }
    
    public static void test4_put()
    {
	PublicToPackage test;

	test = new PublicToPackage();
	test.a = 10;
    }
    
    public static void test4_get()
    {
	PublicToPackage test;
	int a;
	
	test = new PublicToPackage();
	a = test.a;
    }
    
    public static void test4_static_put()
    {
	PublicToPackage.b = 10;
    }
    
    public static int test4_static_get()
    {
	return PublicToPackage.b;
    }
    
    public static void test5_put()
    {
	PublicToProtected test;

	test = new PublicToProtected();
	test.a = 10;
    }
    
    public static void test5_get()
    {
	PublicToProtected test;
	int a;
	
	test = new PublicToProtected();
	a = test.a;
    }
    
    public static void test5_static_put()
    {
	PublicToProtected.b = 10;
    }
    
    public static int test5_static_get()
    {
	return PublicToProtected.b;
    }
    
    public static void test6_put()
    {
	new ProtectedToPrivate() {
		public void run()
		{
		    this.a = 10;
		}
	    }.run();
    }
    
    public static void test6_get()
    {
	new ProtectedToPrivate() {
		public int run()
		{
		    return this.a;
		}
	    }.run();
    }
    
    public static void test6_static_put()
    {
	new ProtectedToPrivate() {
		public void run()
		{
		    b = 10;
		}
	    }.run();
    }
    
    public static void test6_static_get()
    {
	new ProtectedToPrivate() {
		public int run()
		{
		    return b;
		}
	    }.run();
    }

    public static void test7_put()
    {
	DefinedToUndefined test;

	test = new DefinedToUndefined();
	test.a = 10;
    }
    
    public static int test7_get()
    {
	DefinedToUndefined test;

	test = new DefinedToUndefined();
	return test.a;
    }
    
    public static void test7_static_put()
    {
	DefinedToUndefined.b = 10;
    }
    
    public static int test7_static_get()
    {
	return DefinedToUndefined.b;
    }
    
    public static void test8_put()
    {
	IntToFloat test;

	test = new IntToFloat();
	test.a = 10;
    }
    
    public static int test8_get()
    {
	IntToFloat test;

	test = new IntToFloat();
	return test.a;
    }
    
    public static void test8_static_put()
    {
	IntToFloat.b = 10;
    }
    
    public static int test8_static_get()
    {
	return IntToFloat.b;
    }
    
    public static void main(String args[])
    {
	try
	{
	    test1_put();
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
	    test1_get();
	}
	catch(IncompatibleClassChangeError e)
	{
	    System.out.println("1.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test2_put();
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
	    test2_get();
	}
	catch(IncompatibleClassChangeError e)
	{
	    System.out.println("2.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test3_put();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("3.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test3_get();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("3.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test3_static_put();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("3.3 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test3_static_get();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("3.4 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test4_put();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("4.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test4_get();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("4.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test4_static_put();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("4.3 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test4_static_get();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("4.4 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test5_put();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("5.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test5_get();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("5.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test5_static_put();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("5.3 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test5_static_get();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("5.4 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test6_put();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("6.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test6_get();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("6.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test6_static_put();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("6.3 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test6_static_get();
	}
	catch(IllegalAccessError e)
	{
	    System.out.println("6.4 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test7_put();
	}
	catch(NoSuchFieldError e)
	{
	    System.out.println("7.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test7_get();
	}
	catch(NoSuchFieldError e)
	{
	    System.out.println("7.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test7_static_put();
	}
	catch(NoSuchFieldError e)
	{
	    System.out.println("7.3 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test7_static_get();
	}
	catch(NoSuchFieldError e)
	{
	    System.out.println("7.4 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test8_put();
	}
	catch(NoSuchFieldError e)
	{
	    System.out.println("8.1 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test8_get();
	}
	catch(NoSuchFieldError e)
	{
	    System.out.println("8.2 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test8_static_put();
	}
	catch(NoSuchFieldError e)
	{
	    System.out.println("8.3 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
	try
	{
	    test8_static_get();
	}
	catch(NoSuchFieldError e)
	{
	    System.out.println("8.4 Success.");
	    if( verbose )
	    {
		e.printStackTrace();
	    }
	}
    }
}

/* Expected Output:
1.1 Success.
1.2 Success.
2.1 Success.
2.2 Success.
3.1 Success.
3.2 Success.
3.3 Success.
3.4 Success.
4.1 Success.
4.2 Success.
4.3 Success.
4.4 Success.
5.1 Success.
5.2 Success.
5.3 Success.
5.4 Success.
6.1 Success.
6.2 Success.
6.3 Success.
6.4 Success.
7.1 Success.
7.2 Success.
7.3 Success.
7.4 Success.
8.1 Success.
8.2 Success.
8.3 Success.
8.4 Success.
*/
