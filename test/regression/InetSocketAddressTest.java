
import java.net.InetAddress;
import java.net.Inet6Address;
import java.net.InetSocketAddress;

/**
 * Test for the InetSocketAddress class.
 */
public class InetSocketAddressTest
{
    public static void main(String args[])
    {
	InetSocketAddress isa;

	/* Argument checking */
	try { new InetSocketAddress((String)null, 0); }
	catch(IllegalArgumentException e) { System.out.println(e); }
	
	try { new InetSocketAddress(100000); }
	catch(IllegalArgumentException e) { System.out.println(e); }
	try { new InetSocketAddress(65536); }
	catch(IllegalArgumentException e) { System.out.println(e); }
	try { new InetSocketAddress(-1); }
	catch(IllegalArgumentException e) { System.out.println(e); }
	try { new InetSocketAddress(-128); }
	catch(IllegalArgumentException e) { System.out.println(e); }

	/* Unresolved vs. Resolved addresses. */
	isa = new InetSocketAddress("bad.bad.bad", 0);
	System.out.println("Unresolved "
			   + isa.getHostName()
			   + ": "
			   + isa.isUnresolved());
	System.out.println("Unresolved toString(): " + isa);
	if( isa.equals(new InetSocketAddress("bad.bad.bad", 128)) )
	{
		System.out.println("Bad equals?");
	}
	if( !isa.equals(new InetSocketAddress("bad.bad.bad", 0)) )
	{
		System.out.println("Bad equals?");
	}
	
	isa = new InetSocketAddress("localhost", 0);
	System.out.println("Unresolved "
			   + isa.getHostName()
			   + ": "
			   + isa.isUnresolved());
	System.out.println("Resolved toString(): " + check(isa));
	if( isa.equals(new InetSocketAddress("localhost", 128)) )
	{
		System.out.println("Bad equals?");
	}
	if( !isa.equals(new InetSocketAddress("localhost", 0)) )
	{
		System.out.println("Bad equals?");
	}

	/* Wildcard address */
	isa = new InetSocketAddress(128);
	System.out.println("Wildcard address: "
			   + isa.getAddress().isAnyLocalAddress());
	System.out.println("Port: " + isa.getPort());
	isa = new InetSocketAddress((InetAddress)null, 0);
	System.out.println("Null address is wildcard: "
			   + isa.getAddress().isAnyLocalAddress());
    }

    private static String check(InetSocketAddress isa)
    {
        if (isa.getAddress() instanceof Inet6Address) {
            if (isa.toString().equals("localhost/::::::::1:0"))
            {
                return "localhost/127.0.0.1:0";
            }
        }
        return isa.toString();
    }

}

/* Expected Output:
java.lang.IllegalArgumentException: Null host name value
java.lang.IllegalArgumentException: Bad port number: 100000
java.lang.IllegalArgumentException: Bad port number: 65536
java.lang.IllegalArgumentException: Bad port number: -1
java.lang.IllegalArgumentException: Bad port number: -128
Unresolved bad.bad.bad: true
Unresolved toString(): bad.bad.bad:0
Unresolved localhost: false
Resolved toString(): localhost/127.0.0.1:0
Wildcard address: true
Port: 128
Null address is wildcard: true
*/
