
/**
 * Test simple control flow constructs, if's, loops, etc...
 */
class ControlFlowMethods
{
  
    public static final int test_1__int_if_int = 2;
    public static final int test_4__int_if_int = 144;
    public static final int test_5__int_if_int = 0;
    
    public static int int_if_int(int a)
    {
	int retval;

	if( a == 1 )
	{
	    retval = 2;
	}
	else if( a == 4 )
	{
	    retval = 144;
	}
	else
	{
	    retval = 0;
	}
	return( retval );
    }
    
    public static final int test__int_for = 10;
    
    public static int int_for()
    {
	int lpc;

	for( lpc = 0; lpc < 10; lpc++ )
	{
	}
	return( lpc );
    }
    
    public static final int test__int_while = 10;
    
    public static int int_while()
    {
	int lpc = 0;

	while( lpc < 10 )
	{
	    lpc += 1;
	}
	return( lpc );
    }
    
    public static final int test__int_do_while = 10;
    
    public static int int_do_while()
    {
	int lpc = 0;

	do {
	    lpc += 1;
	} while( lpc < 10 );
	return( lpc );
    }

    /*
     * A switch that reduces to a TABLESWITCH bytecode.
     */
    
    public static final int test_0__int_tableswitch_int = -1;
    public static final int test_1__int_tableswitch_int = 2;
    public static final int test_2__int_tableswitch_int = 144;
    public static final int test_3__int_tableswitch_int = 169;
    public static final int test_4__int_tableswitch_int = 0xdeadbeef;
    public static final int test_5__int_tableswitch_int = -1;
    
    public static int int_tableswitch_int(int a)
    {
	int retval;
	
	switch( a )
	{
	case 1:
	    retval = 2;
	    break;
	case 2:
	    retval = 144;
	    break;
	case 3:
	    retval = 169;
	    break;
	case 4:
	    retval = 0xdeadbeef;
	    break;
	default:
	    retval = -1;
	    break;
	}
	return( retval );
    }
    
    /*
     * A switch that reduces to a LOOKUPSWITCH bytecode.
     */
    
    public static final int test_0__int_lookupswitch_int = -1;
    public static final int test_0xdeadbeef__int_lookupswitch_int = 1;
    public static final int test_0xd0decade__int_lookupswitch_int = 2;
    public static final int test_0xbeefdead__int_lookupswitch_int = 3;
    public static final int test_0xffffffff__int_lookupswitch_int = -1;

    public static int int_lookupswitch_int(int a)
    {
	int retval;

	switch( a )
	{
	case 0xdeadbeef:
	    retval = 1;
	    break;
	case 0xd0decade:
	    retval = 2;
	    break;
	case 0xbeefdead:
	    retval = 3;
	    break;
	default:
	    retval = -1;
	    break;
	}
	return retval;
    }
    
    private ControlFlowMethods()
    {
    }
}
