
package java.net;

public class InetSocketAddress
    extends SocketAddress
{
    private static final InetAddress ANY_ADDRESS;

    static {
	try
	{
	    ANY_ADDRESS = InetAddress.getByAddress(new byte[] { 0, 0, 0, 0 });
	    ANY_ADDRESS.hostName = "";
	}
	catch(UnknownHostException e)
	{
	    throw new InternalError();
	}
    }
    
    private final String hostname;
    private final InetAddress addr;
    private final int port;

    private InetSocketAddress(String hostname, InetAddress addr, int port)
    {
	if( (port < 0) || (port > 65535) )
	{
	    throw new IllegalArgumentException("Bad port number: "
					       + port);
	}

	if( hostname != null )
	{
	    try
	    {
		addr = InetAddress.getByName(hostname);
	    }
	    catch(UnknownHostException e)
	    {
		/* Ignore... */
	    }
	}
	else if( addr == null )
	{
	    addr = ANY_ADDRESS;
	    hostname = "*";
	}
	else
	{
	    hostname = addr.getHostName();
	}
	this.hostname = hostname;
	this.addr = addr;
	this.port = port;
    }
    
    public InetSocketAddress(InetAddress addr, int port)
    {
	this(null, addr, port);
    }
    
    public InetSocketAddress(int port)
    {
	this(null, ANY_ADDRESS, port);
    }
    
    public InetSocketAddress(String hostname, int port)
    {
	this(hostname, null, port);

	if( hostname == null )
	{
	    throw new IllegalArgumentException("Null host name value");
	}
    }

    public final int getPort()
    {
	return this.port;
    }

    public final InetAddress getAddress()
    {
	return this.addr;
    }

    public final String getHostName()
    {
	return this.hostname;
    }

    public final boolean isUnresolved()
    {
	return (this.hostname != null) && (this.addr == null);
    }

    public final boolean equals(Object obj)
    {
	boolean retval = false;

	if( obj instanceof InetSocketAddress )
	{
	    InetSocketAddress isa = (InetSocketAddress)obj;

	    if( (this.isUnresolved() == isa.isUnresolved()) &&
		(this.port == isa.port) )
	    {
		if( this.addr != null )
		{
		    if( this.addr.equals(isa.addr) )
		    {
			retval = true;
		    }
		}
		else if( this.hostname.equals(isa.hostname) )
		{
		    retval = true;
		}
	    }
	}
	return retval;
    }

    public final int hashCode()
    {
	return this.hostname.hashCode();
    }

    public String toString()
    {
	String retval;
	
	if( this.isUnresolved() )
	{
	    retval = this.hostname + ":" + this.port;
	}
	else
	{
	    retval = this.addr + ":" + this.port;
	}
	return retval;
    }
}
