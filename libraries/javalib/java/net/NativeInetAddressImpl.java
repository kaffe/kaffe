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

/**
 * InetAddressImpl that uses native functions (getaddrinfo(), getnameinfo()).
 */
class NativeInetAddressImpl
    extends InetAddressImpl
{
    native byte[][] lookupAllHostAddr0(String host)
	throws UnknownHostException;

    byte[][] lookupAllHostAddr(String host)
	throws UnknownHostException
    {
	return this.lookupAllHostAddr0(host);
    }
    
    native String getHostByAddr0(byte addr[])
	throws UnknownHostException;
    
    String getHostByAddr(byte addr[])
	throws UnknownHostException
    {
	return this.getHostByAddr0(addr);
    }
}
