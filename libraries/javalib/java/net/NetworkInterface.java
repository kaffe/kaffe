/*
 * NetworkInterface.java
 *
 * Copyright (c) 2002, 2003 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

package java.net;

import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;

public final class NetworkInterface
{
    /*
     * This implementation specific stuff should probably be moved elsewhere.
     * Also, the implementation will not detect changes in the interfaces.
     * It does the detection at class initialization and saves these objects
     * in a global variable.
     */

    /**
     * The detected interfaces, the table maps names to NetworkInterface
     * objects.
     */
    private static final Hashtable DETECTED_INTERFACES = new Hashtable();

    /**
     * Secondary mapping from InetAddresses to interface names.
     */
    private static final Hashtable ADDRESS_TO_NAME = new Hashtable();

    /**
     * If not null, the SocketException thrown on initialization.
     */
    private static SocketException detectException;

    /**
     * Detect the interfaces in this machine.
     *
     * @return The native list of interfaces.
     * @throws SocketException if there is a problem with the native code.
     */
    private static native kaffe.util.Ptr detectInterfaces()
	throws SocketException;

    /**
     * Free the native objects returned by detectInterfaces.
     *
     * @param ifaddrs The pointer returned by detectInterfaces or null.
     */
    private static native void freeInterfaces(kaffe.util.Ptr ifaddrs);

    /**
     * Get the next interface in the native list.
     *
     * @param ifaddr The current interface.
     * @return The next interface in the list or null if there are no more.
     */
    private static native kaffe.util.Ptr getNext(kaffe.util.Ptr ifaddr);

    /**
     * @param ifaddr The current interface.
     * @return The name encoded in the native object.
     */
    private static native String getName(kaffe.util.Ptr ifaddr);
    
    /**
     * @param ifaddr The current interface.
     * @return The IPv4 address of this interface as a string or null if the
     * native object doesn't have an IPv4 address.
     */
    private static native String getIPv4Address(kaffe.util.Ptr ifaddr);
    
    static
    {
	kaffe.util.Ptr ifaddrs = null, curr = null;

	/* Our native implementation is in kaffenet */
	System.loadLibrary("net");

	try
	{
	    /* Detect all the network interfaces. */
	    curr = ifaddrs = detectInterfaces();
	    while( curr != null )
	    {
		NetworkInterface ni;
		String name;
		
		name = getName(curr);
		if( (ni = (NetworkInterface)DETECTED_INTERFACES.get(name))
		    == null )
		{
		    ni = new NetworkInterface(name, name /* XXX */);
		    DETECTED_INTERFACES.put(name, ni);
		}
		try
		{
		    String address;

		    if( (address = getIPv4Address(curr)) != null )
		    {
			InetAddress ia;

			ia = InetAddress.getByName(address);
			ni.setPrimaryAddress(ia);
			ADDRESS_TO_NAME.put(ia, ni.getName());
			ni.getInetAddressesInternal().addElement(ia);
		    }
		}
		catch(UnknownHostException e)
		{
		    /* Ignore... */
		}
		curr = getNext(curr);
	    }
	    DETECTED_INTERFACES.elements();
	}
	catch(SocketException e)
	{
	    detectException = e;
	}
	finally
	{
	    /* Make sure to free the native objects. */
	    freeInterfaces(ifaddrs);
	}
    }
    
    public static NetworkInterface getByName(String name)
	throws SocketException
    {
	if( detectException == null )
	{
	    NetworkInterface retval = null;
	    
	    retval = (NetworkInterface)DETECTED_INTERFACES.get(name);
	    return retval;
	}
	else
	{
	    throw detectException;
	}
    }

    public static NetworkInterface getByInetAddress(InetAddress ia)
	throws SocketException
    {
	if( detectException == null )
	{
	    NetworkInterface retval = null;
	    String name;
	    
	    if( (name = (String)ADDRESS_TO_NAME.get(ia)) != null )
	    {
		retval = (NetworkInterface)DETECTED_INTERFACES.get(name);
	    }
	    return retval;
	}
	else
	{
	    throw detectException;
	}
    }

    public static Enumeration getNetworkInterfaces()
	throws SocketException
    {
	if( detectException == null )
	{
	    Enumeration retval = null;

	    if( DETECTED_INTERFACES.size() > 0 )
	    {
		retval = DETECTED_INTERFACES.elements();
	    }
	    return retval;
	}
	else
	{
	    throw detectException;
	}
    }
    
    private final String name;
    private final String displayName;
    private InetAddress primaryAddress;
    private final Vector inetAddresses = new Vector();
    
    private NetworkInterface(String name, String displayName)
    {
	this.name = name;
	this.displayName = displayName;
    }

    public String getName()
    {
	return this.name;
    }

    public String getDisplayName()
    {
	return this.displayName;
    }

    void setPrimaryAddress(InetAddress ia)
    {
	this.primaryAddress = ia;
    }

    /**
     * @return An IPv4 address suitable for identifying this interface.
     */
    InetAddress getPrimaryAddress()
    {
	return this.primaryAddress;
    }

    Vector getInetAddressesInternal()
    {
	return this.inetAddresses;
    }

    public Enumeration getInetAddresses()
    {
	Enumeration retval = null;
	SecurityManager sm;

	sm = System.getSecurityManager();
	if( sm != null )
	{
	    Vector accessible = new Vector();
	    
	    synchronized( this.inetAddresses )
	    {
		int lpc, size;
		
		size = this.inetAddresses.size();
		for( lpc = 0; lpc < size; lpc++ )
		{
		    InetAddress ia;
		    
		    ia = (InetAddress)this.inetAddresses.elementAt(lpc);
		    try
		    {
			sm.checkConnect(ia.getHostName(), -1);
			accessible.addElement(ia);
		    }
		    catch(SecurityException e)
		    {
			/* Ignore... */
		    }
		}
	    }
	    retval = accessible.elements();
	}
	else
	{
	    retval = this.inetAddresses.elements();
	}
	return retval;
    }

    public int hashCode()
    {
	return this.name.hashCode();
    }

    public boolean equals(Object obj)
    {
	boolean retval = false;

	if( this == obj )
	{
	    retval = true;
	}
	else if( obj instanceof NetworkInterface )
	{
	    NetworkInterface ni;

	    ni = (NetworkInterface)obj;
	    retval = this.name.equals(ni.name);
	}
	return retval;
    }

    public String toString()
    {
	return "NetworkInterface[name="
	    + this.name
	    + "; inetAddresses="
	    + this.inetAddresses
	    + "]";
    }
}
