/*
 * NetworkInterfaceImpl.java
 *
 * Copyright (c) 2003 University of Utah and the Flux Group.
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

import java.util.Hashtable;

/**
 * Implementation specific functions for querying the set of NetworkInterfaces
 */
class NetworkInterfaceImpl
{
    static
    {
	/* Our native implementation is in kaffenet */
	System.loadLibrary("net");
    }
    
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
     * @return The IP address of this interface as a string or null if the
     * native object doesn't have an IP address.
     */
    private static native String getIPAddress(kaffe.util.Ptr ifaddr);
    
    /**
     * The detected interfaces, the table maps names to NetworkInterface
     * objects.
     */
    private final Hashtable detectedInterfaces = new Hashtable();
    
    /**
     * Secondary mapping from InetAddresses to interface names.
     */
    private final Hashtable addressToName = new Hashtable();

    /**
     * Construct a NetworkInterfaceImpl.
     *
     * @throws SocketException if there was a problem reading the set of
     * NetworkInterfaces.
     */
    NetworkInterfaceImpl()
	throws SocketException
    {
	kaffe.util.Ptr ifaddrs = null, curr = null;

	try
	{
	    /* Detect all the network interfaces. */
	    curr = ifaddrs = detectInterfaces();
	    while( curr != null )
	    {
		NetworkInterface ni;
		String name;
		
		name = getName(curr);
		if( (ni = (NetworkInterface)this.detectedInterfaces.get(name))
		    == null )
		{
		    ni = new NetworkInterface(name, name /* XXX */);
		    this.detectedInterfaces.put(name, ni);
		}
		try
		{
		    String address;

		    if( (address = getIPAddress(curr)) != null )
		    {
			InetAddress ia;

			ia = InetAddress.getByName(address);
			ni.setPrimaryAddress(ia);
			this.addressToName.put(ia, ni.getName());
			ni.getInetAddressesInternal().addElement(ia);
		    }
		}
		catch(UnknownHostException e)
		{
		    /* Ignore... */
		}
		curr = getNext(curr);
	    }
	    this.detectedInterfaces.elements();
	}
	finally
	{
	    /* Make sure to free the native objects. */
	    freeInterfaces(ifaddrs);
	}
    }

    /**
     * @return The table of detected interfaces.
     */
    Hashtable getDetectedInterfaces()
    {
	return this.detectedInterfaces;
    }

    /**
     * @return The table that maps InetAddress' to the NIC name.
     */
    Hashtable getAddressToName()
    {
	return this.addressToName;
    }

    /**
     * @param ia An InetAddress to map to a NIC name.
     * @return The NIC name that has the given address or null if there is no
     * mapping.
     */
    String nameForAddress(InetAddress ia)
    {
	return (String)this.addressToName.get(ia);
    }
    
    public String toString()
    {
	return "NetworkInterfaceImpl[detectedInterfaces={ "
	    + this.detectedInterfaces
	    + " }; addressToName={ "
	    + this.addressToName
	    + "]";
    }
}
