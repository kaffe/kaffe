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
 * Base class for DNS implementations.
 */
abstract class InetAddressImpl
{
    public static final int INET_ADDRESS_MIN = 0;

    /**
     * IPv4 address family.
     */
    public static final int INET_ADDRESS_V4 = 1;

    /**
     * IPv6 address family.
     */
    public static final int INET_ADDRESS_V6 = 2;
    
    public static final int INET_ADDRESS_MAX = 3;

    static
    {
	System.loadLibrary("net");
    }

    /**
     * Map an INET_ADDRESS_V* value to the address family value.
     *
     * @param kind One of the INET_ADDRESS_V* constants.
     * @return The integer representing the address family used by the OS.
     */
    static native int getInetFamily(int kind);

    /**
     * @return The machine's host name.
     */
    static native String getLocalHostName();

    /**
     * Convert an address string to the appropriate address bits.
     *
     * @param addr The address string to convert.
     * @return Null if the string was not in a known format, a four byte array
     * if it was an IPv4 address, and a sixteen byte array if it was an IPv6
     * address.
     */
    static native byte[] stringToBits(String addr);

    /**
     * Call back used to map a host name to its addresses.
     *
     * @param host The host name to query the DNS server about.
     * @return An array of byte arrays containing the IP addresses.  Sub-arrays
     * of length four are IPv4 addresses and length sixteen are IPv6.
     * @throws UnknownHostException if the DNS lookup failed.
     */
    abstract byte[][] lookupAllHostAddr(String host)
	throws UnknownHostException;

    /**
     * Call back used to perform a reverse lookup.
     *
     * @param addr The IP address to map to a host name.
     * @return The host name.
     * @throws UnknownHostException if the DNS lookup failed.
     */
    abstract String getHostByAddr(byte addr[])
	throws UnknownHostException;
}
