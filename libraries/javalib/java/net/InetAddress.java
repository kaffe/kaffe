/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.net;

import java.io.Serializable;
import java.util.StringTokenizer;

/**
 * Base class for IP addresses.  Implementation details are handled by the
 * InetAddressImpl class.
 */
public class InetAddress
    implements Serializable
{
    private static final long serialVersionUID = 3286316764910316507L;

    private static final int IPV4_LOOPBACK_BITS = 0x7F000001;

    /**
     * Our handle on the implementation.
     */
    private static final InetAddressImpl impl;

    /**
     * The list of implementation classes, ordered from most to least
     * desirable.
     */
    static final String INET_ADDRESS_IMPLS[] = {
	"java.net.DNSJavaInetAddressImpl",
	"java.net.NativeInetAddressImpl",
    };
    
    static
    {
	InetAddressImpl iai = null;
	int lpc;

	for( lpc = 0;
	     (lpc < INET_ADDRESS_IMPLS.length) && (iai == null);
	     lpc++ )
	{
	    try
	    {
		Class cl;

		cl = Class.forName(INET_ADDRESS_IMPLS[lpc]);
		iai = (InetAddressImpl)cl.newInstance();
		System.setProperty("org.kaffe.dns", INET_ADDRESS_IMPLS[lpc]);
	    }
	    catch(IllegalAccessException e)
	    {
		/* Really should not happen. */
		throw new InternalError(e);
	    }
	    catch(ExceptionInInitializerError e)
	    {
	    }
	    catch(InstantiationException e)
	    {
	    }
	    catch(NoClassDefFoundError e)
	    {
	    }
	    catch(ClassNotFoundException e)
	    {
	    }
	}
	if( iai == null )
	{
	    /* Not sure about this error type. */
	    throw new UnsatisfiedLinkError(
		"Cannot find working InetAddressImpl");
	}
	impl = iai;
    }

    /**
     * Convert an IPv4 address encoded in a byte to an integer.
     *
     * @param addr The address to convert.
     * @return The address from addr or zero if the length of addr was not
     * four.
     */
    private static int fromBytes(byte addr[])
    {
	int retval = 0;

	if( addr.length == 4 )
	{
	    retval = (((addr[0] & 0xFF) << 24) |
		      ((addr[1] & 0xFF) << 16) |
		      ((addr[2] & 0xFF) <<  8) |
		      ((addr[3] & 0xFF)      ));
	}
	return retval;
    }

    /**
     * Convert an IPv4 address encoded as an integer to a byte array.
     *
     * @param address The address to convert.
     * @return A byte array containing the address value in network order.
     */
    private static byte[] toBytes(int address)
    {
	byte retval[] = new byte[4];

	retval[0] = (byte)((address >> 24) & 0xFF);
	retval[1] = (byte)((address >> 16) & 0xFF);
	retval[2] = (byte)((address >>  8) & 0xFF);
	retval[3] = (byte) (address        & 0xFF);
	return retval;
    }

    /**
     * The host name as specified by the user or derived from a DNS lookup.
     */
    transient String hostName;

    /**
     * The IP address encoded as an array.
     */
    transient byte addr[];

    /**
     * The IPv4 address encoded as an integer.
     */
    int address;

    /**
     * The address family.
     */
    private int family;

    /**
     * The designated InetAddress constructor.
     *
     * @param addr The IP address.
     * @param address The IPv4 address encoded as an integer or zero.
     * @param name The given host name or null if a lookup should eventually be
     * done.
     */
    InetAddress(byte addr[], int address, String name)
    {
	this.family = impl.getInetFamily(this instanceof Inet6Address ?
					 InetAddressImpl.INET_ADDRESS_V6 :
					 InetAddressImpl.INET_ADDRESS_V4);
	this.hostName = name;
	this.addr = addr;
	this.address = address;
    }

    /**
     * @param addr The IP address.
     * @param name The given host name or null if a lookup should eventually be
     * done.
     */
    InetAddress(byte addr[], String name)
    {
	this(addr, fromBytes(addr), name);
    }

    /**
     * @param address The IPv4 address encoded as an integer.
     * @param name The given host name or null if a lookup should eventually be
     * done.
     */
    InetAddress(int address, String name)
    {
	this(toBytes(address), address, name);
    }

    /**
     * Construct an empty InetAddress.
     */
    InetAddress()
    {
	this(null, 0, null);
    }

    public static InetAddress[] getAllByName(String host)
	throws UnknownHostException,
	       SecurityException
    {
	InetAddress retval[];
	
	if( host == null || host.equals("") || host.equals("localhost") )
	{
	    retval = new InetAddress[] {
		new Inet4Address(toBytes(IPV4_LOOPBACK_BITS), "localhost"),
	    };
	}
	else
	{
	    SecurityManager sm = System.getSecurityManager();
	    if( sm != null )
		sm.checkConnect(host, -1);

	    byte ip[] = InetAddressImpl.stringToBits(host);
	    byte addrs[][];
	    int lpc;

	    if( ip == null )
	    {
		/* Its a host name. */
		addrs = impl.lookupAllHostAddr(host);
	    }
	    else
	    {
		/* Its an IP address. */
		addrs = new byte[][] { ip };
	    }
	    retval = new InetAddress[addrs.length];
	    /* Convert to the correct InetAddress type. */
	    for( lpc = 0; lpc < addrs.length; lpc++ )
	    {
		switch( addrs[lpc].length )
		{
		case 4:
		    retval[lpc] = new Inet4Address(addrs[lpc],
						   ip == null ?
						   host :
						   null);
		    break;
		case 16:
		    retval[lpc] = new Inet6Address(addrs[lpc],
						   ip == null ?
						   host :
						   null);
		    break;
		default:
		    throw new InternalError("Unhandled address length: "
					    + addrs[lpc].length);
		}
	    }
	}
	return retval;
    }

    public static InetAddress getByAddress(byte[] addr)
	throws UnknownHostException
    {
	InetAddress retval;
	
	switch( addr.length )
	{
	case 4:
	    retval = new Inet4Address(addr, null);
	    break;
	case 16:
	    retval = new Inet6Address(addr, null);
	    break;

	default:
	    throw new UnknownHostException("Bad address length: "
					   + addr.length
					   + " bytes");
	}

	return retval;
    }

    public static InetAddress getByName(String host)
	throws UnknownHostException
    {
	InetAddress retval = null, addrs[];
	int lpc;

	addrs = getAllByName(host);
	/*
	 * Alas, the spec does not specify _which_ address to return.  So, for
	 * the sake of backwards compatibility, we'll prefer the first IPv4
	 * address.
	 */
	for( lpc = 0; (lpc < addrs.length) && (retval == null); lpc++ )
	{
	    if( addrs[lpc] instanceof Inet4Address )
	    {
		retval = addrs[lpc];
	    }
	}
	if( (retval == null) && (addrs.length > 0) )
	{
	    /* No IPv4 addresses, use the first IPv6. */
	    retval = addrs[0];
	}
	return retval;
    }

    public static InetAddress getLocalHost()
	throws UnknownHostException
    {
	try
	{
	    String name = impl.getLocalHostName();
	    SecurityManager sm = System.getSecurityManager();
	    if (sm != null)
		sm.checkConnect(name, 0);
	    return InetAddress.getByName(name);
	}
	catch(SecurityException se)
	{
	    return InetAddress.getLoopback();
	}
    }

    public String getHostAddress()
    {
	return null;
    }

    public String getCanonicalHostName()
    {
	String retval;

	try
	{
	    SecurityManager sm = System.getSecurityManager();
	    if( sm != null )
		sm.checkConnect(this.hostName, -1);
	    
	    /*
	     * Since this.hostName can be a local reference (e.g.'foo') we need
	     * to do a full lookup to get 'foo.bar.com'.
	     */
	    retval = impl.getHostByAddr(this.addr);

	    /* XXX Should we call checkConnect on retval? */
	}
	catch(UnknownHostException _)
	{
	    retval = this.getHostAddress();
	}
	catch(SecurityException _)
	{
	    retval = this.getHostAddress();
	}
	return retval;
    }

    public String getHostName()
    {
	String retval;
	
	try
	{
	    if( this.hostName == null )
	    {
		/* Lookup not done yet... */
		this.hostName = impl.getHostByAddr(this.addr);
	    }
	    retval = this.hostName;

	    try
	    {
		/* Make sure its visible to them. */
		SecurityManager sm = System.getSecurityManager();
		if( sm != null )
		    sm.checkConnect(this.hostName, -1);
	    }
	    catch(SecurityException e)
	    {
		retval = this.getHostAddress();
	    }
	}
	catch(UnknownHostException _)
	{
	    /* No joy on the lookup. */
	    retval = this.hostName = this.getHostAddress();
	}
	return retval;
    }

    public boolean isMulticastAddress()
    {
	return false;
    }

    public boolean isAnyLocalAddress()
    {
	return false;
    }

    public boolean isLoopbackAddress()
    {
	return false;
    }

    public boolean isLinkLocalAddress()
    {
	return false;
    }

    public boolean isSiteLocalAddress()
    {
	return false;
    }

    public boolean isMCGlobal()
    {
	return false;
    }

    public boolean isMCNodeLocal()
    {
	return false;
	
    }

    public boolean isMCLinkLocal()
    {
	return false;
    }

    public boolean isMCSiteLocal()
    {
	return false;
    }

    public boolean isMCOrgLocal()
    {
	return false;
    }

    public byte[] getAddress()
    {
	return null;
    }

    public int hashCode()
    {
	return 0;
    }

    public boolean equals(Object obj)
    {
	try
	{
	    InetAddress ia;

	    ia = (InetAddress)obj;
	    if( (this.family == ia.family) &&
		(this.address == ia.address) )
	    {
		return true;
	    }
	}
	catch(ClassCastException e)
	{
	}
	return false;
    }

    public String toString()
    {
	StringBuffer result = new StringBuffer();

	if( this.hostName == null )
	{
	    result.append("");
	}
	else
	{
	    result.append(this.hostName);
	}
	result.append("/");
	result.append(getHostAddress());

	return result.toString();
    }

    static InetAddress getLoopback()
    {
	return new InetAddress(IPV4_LOOPBACK_BITS, "localhost");
    }

    /** JanosVM extension for JSR-121 prototype */
    /*package*/ InetAddress dup()
    {
	InetAddress retval = new InetAddress();
	
	retval.hostName = this.hostName == null ? null : new String(this.hostName.toCharArray());
	retval.address = this.address;
	retval.family = this.family;
	return retval;
    }

    static InetAddress getAnyAddress()
    {
	return new InetAddress(new byte[] { 0, 0, 0, 0 }, null);
    }
}
