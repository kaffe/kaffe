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
    /**
     * XXX Currently, we just create a new NetworkInterfaceImpl for each
     * request.  It would be nicer to either timeout the cache or check for
     * diffs before throwing the objects away.
     */
    
    public static NetworkInterface getByName(String name)
	throws SocketException
    {
	NetworkInterfaceImpl nii = new NetworkInterfaceImpl();
	NetworkInterface retval;
	
	retval = (NetworkInterface)nii.getDetectedInterfaces().get(name);
	return retval;
    }

    public static NetworkInterface getByInetAddress(InetAddress ia)
	throws SocketException
    {
	NetworkInterfaceImpl nii = new NetworkInterfaceImpl();
	NetworkInterface retval = null;
	String name;
	
	if( (name = nii.nameForAddress(ia)) != null )
	{
	    retval = (NetworkInterface)nii.getDetectedInterfaces().get(name);
	}
	return retval;
    }

    public static Enumeration getNetworkInterfaces()
	throws SocketException
    {
	NetworkInterfaceImpl nii = new NetworkInterfaceImpl();
	Enumeration retval = null;

	if( nii.getDetectedInterfaces().size() > 0 )
	{
	    retval = nii.getDetectedInterfaces().elements();
	}
	return retval;
    }

    /**
     * The OS provided NIC name.
     */
    private final String name;

    /**
     * Same as the above for now.
     */
    private final String displayName;

    /**
     * An IPv4 address suitable for identifying this interface.
     */
    private InetAddress primaryAddress;

    /**
     * The set of InetAddresses for this interface.
     */
    private final Vector inetAddresses = new Vector();

    /**
     * Construct a NetworkInterface with the given values.
     *
     * @param name The OS provided NIC name.
     * @param displayName The user-interpretable NIC name.
     */
    NetworkInterface(String name, String displayName)
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

    /**
     * @return The Vector containing the NetworkInterface's addresses.
     */
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
