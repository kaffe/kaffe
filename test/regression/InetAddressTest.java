
import java.net.InetAddress;
import java.net.Inet6Address;

public class InetAddressTest
{
    public static void main(String args[])
	throws Throwable
    {
	InetAddress ia;

	ia = InetAddress.getByName(null);
	System.out.println("(null) = " + check(ia));
	
	ia = InetAddress.getByName("");
	System.out.println("\"\" = " + check(ia));

	ia = InetAddress.getByName("localhost");
	System.out.println("localhost = " + check(ia));
	if( !ia.isLoopbackAddress() )
	{
	    System.out.println("Not a loopback?");
	}
    }

    private static String check(InetAddress ia)
         throws Throwable
    {
        String expected = (
            ia instanceof Inet6Address ?
                "localhost/::::::::1" :
	        "localhost/127.0.0.1" );
        if (ia.toString().equals(expected)) return "localhost/127.0.0.1";
        return ia.toString();
    }

}

/* Expected Output:
(null) = localhost/127.0.0.1
"" = localhost/127.0.0.1
localhost = localhost/127.0.0.1
*/
