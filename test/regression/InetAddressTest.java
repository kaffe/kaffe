
import java.net.InetAddress;

public class InetAddressTest
{
    public static void main(String args[])
	throws Throwable
    {
	InetAddress ia;

	ia = InetAddress.getByName(null);
	System.out.println("(null) = " + ia);
	
	ia = InetAddress.getByName("");
	System.out.println("\"\" = " + ia);
	
	ia = InetAddress.getByName("localhost");
	System.out.println("localhost = " + ia);
	if( !ia.isLoopbackAddress() )
	{
	    System.out.println("Not a loopback?");
	}
    }
}

/* Expected Output:
(null) = localhost/127.0.0.1
"" = localhost/127.0.0.1
localhost = localhost/127.0.0.1
*/
