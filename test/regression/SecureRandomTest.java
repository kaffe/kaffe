
import java.security.*;

class SecureRandomTest
{
    private static final int HISTORY_SIZE = 20;
    private static final int ITERATIONS = 10;
    
    public static final String toPlainString(byte bytes[], int maxBytes)
    {
	String retval = "";

	if( bytes != null )
	{
	    int lpc;
	    
	    if( maxBytes > bytes.length )
		maxBytes = bytes.length;
	    for( lpc = 0; lpc < maxBytes; lpc++ )
	    {
		if( (bytes[lpc] & 0xf0) == 0 )
		    retval += "0" + Integer.toHexString(bytes[lpc] & 0xff);
		else
		    retval += Integer.toHexString(bytes[lpc] & 0xff);
	    }
	}
	else
	{
	    retval = "(null)";
	}
	return retval;
    }
    
    private static void checkHistory(byte history[][], byte data[])
    {
	int lpc;

	for( lpc = 0; lpc < history.length; lpc++ )
	{
	    boolean match = true;
	    int lpc2;

	    if( history[lpc] == null )
		continue;
	    for( lpc2 = 0; lpc2 < data.length; lpc2++ )
	    {
		match = match && (history[lpc][lpc2] == data[lpc2]);
	    }
	    if( match )
	    {
		throw new Error("The \"secure\" random isn't! : lpc=  " + lpc 
				+ " lpc2 = " + lpc2 
				+ " data = " + toPlainString(data, Integer.MAX_VALUE));
	    }
	}
    }
    
    public static void main(String args[])
	throws Throwable
    {
	byte history[][];
	SecureRandom sr;
	int lpc, hist;
	byte data[];

	Security.addProvider(new kaffe.security.provider.Kaffe());
	
	/*
	 * Make sure the SecureRandom's produce different sequences after
	 * initialization.
	 */
	history = new byte[HISTORY_SIZE][];
	/*
	 * Now that we have added new kaffe.security.provider.Kaffe()
	 * to the list of security providers, we may well explicitly
	 * request Kaffe's SHA1PRNG to be used.
	 * Otherwise, in some poor environment, GNU classpath's default
	 * provider may be used, which, at the moment of this writing,
	 * is not strong enough to pass this test. (Thu Jul 28, 2005)
	 */
	sr = SecureRandom.getInstance("SHA1PRNG", "KAFFE");
	for( lpc = 0, hist = 0; lpc < ITERATIONS; lpc++, hist++ )
	{
	    data = new byte[20];
	    sr.nextBytes(data);
	    history[hist % HISTORY_SIZE] = data;
	}
	sr = SecureRandom.getInstance("SHA1PRNG", "KAFFE");
	for( lpc = 0, hist = 0; lpc < ITERATIONS; lpc++, hist++ )
	{
	    data = new byte[20];
	    sr.nextBytes(data);
	    checkHistory(history, data);
	}
	System.out.println("Two SecureRandoms produce different output.");
    }
}

/* Expected Output:
Two SecureRandoms produce different output.
*/
