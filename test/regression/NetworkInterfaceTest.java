
import java.util.Enumeration;

import java.net.InetAddress;
import java.net.SocketException;
import java.net.NetworkInterface;

public class NetworkInterfaceTest
{
    private static boolean verbose = false;
    
    public static void main(String args[])
    {
	if( args.length > 0 )
	{
	    verbose = true;
	}
	try
	{
	    Enumeration enum;
	    int lpc;
	    
	    enum = NetworkInterface.getNetworkInterfaces();
	    for( lpc = 0; enum.hasMoreElements(); lpc++ )
	    {
		NetworkInterface ni, ni2;
		Enumeration ips;
		
		ni = (NetworkInterface)enum.nextElement();
		if( verbose )
		{
		    System.out.println("Detected interface #" + lpc);
		    System.out.println("  Name: " + ni.getName());
		    System.out.println("  Display Name: "
				       + ni.getDisplayName());
		}
		ni2 = NetworkInterface.getByName(ni.getName());
		if( !ni.equals(ni2) )
		{
		    System.out.println("getByName failed: "
				       + ni
				       + " != "
				       + ni2);
		}

		ips = ni.getInetAddresses();
		while( ips.hasMoreElements() )
		{
		    InetAddress ia;

		    ia = (InetAddress)ips.nextElement();
		    if( verbose )
		    {
			System.out.println("  Address: " + ia);
		    }
		    ni2 = NetworkInterface.getByInetAddress(ia);
		    if( !ni.equals(ni2) )
		    {
			System.out.println("getByInetAddress failed: "
					   + ni
					   + " != "
					   + ni2);
		    }
		}
	    }
	    System.out.println("Done");
	}
	catch(SocketException e)
	{
	    e.printStackTrace();
	    System.exit(77);
	}
	catch(kaffe.util.NotImplemented e)
	{
	    e.printStackTrace();
	    System.exit(77);
	}
    }
}

/* Expected Output:
Done
*/
