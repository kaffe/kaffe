/* InetAddress.java -- Class to model an Internet address
   Copyright (C) 1998, 1999, 2002 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.
 
GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


package java.net;

import java.io.Serializable;
import java.util.HashMap;
import java.util.StringTokenizer;
import gnu.classpath.Configuration;

/**
 * This class models an Internet address.  It does not have a public
 * constructor.  Instead, new instances of this objects are created
 * using the static methods getLocalHost(), getByName(), and
 * getAllByName().
 * <p>
 * This class fulfills the function of the C style functions gethostname(),
 * gethostbyname(), and gethostbyaddr().  It resolves Internet DNS names
 * into their corresponding numeric addresses and vice versa.
 *
 * @author Aaron M. Renn <arenn@urbanophile.com>
 * @author Per Bothner
 *
 * @specnote This class is not final since JK 1.4
 */
public class InetAddress implements Serializable
{
  // Static Initializer to load the shared library needed for name resolution
  static
  {
    if (Configuration.INIT_LOAD_LIBRARY)
      {
        System.loadLibrary ("net");
      }
  }

  private static final long serialVersionUID = 3286316764910316507L;

  /**
   * The default DNS hash table size,
   * use a prime number happy with hash table
   */
  private static final int DEFAULT_CACHE_SIZE = 89;

  /**
   * The default caching period in minutes
   */
  private static final int DEFAULT_CACHE_PERIOD = (4 * 60);

  /**
   * Percentage of cache entries to purge when the table gets full
   */
  private static final int DEFAULT_CACHE_PURGE_PCT = 30;
  
  /**
   * The special IP address INADDR_ANY
   */
  private static InetAddress inaddr_any;

  /**
   * dummy InetAddress, used to bind socket to any (all) network interfaces
   */
  static InetAddress ANY_IF;

  /**
   * loopback InetAddress
   */
  static InetAddress LOOPBACK_ADDRESS;

  /**
   * The size of the cache
   */
  private static int cache_size = 0;

  /**
   * The length of time we will continue to read the address from cache
   * before forcing another lookup
   */
  private static int cache_period = 0;

  /**
   * What percentage of the cache we will purge if it gets full
   */
  private static int cache_purge_pct = 0;

  /**
   * HashMap to use as DNS lookup cache
   * use HashMap because all accesses to cache are already synchronized
   */
  private static HashMap cache;

  // Static initializer for the cache
  static
  {
    // Look for properties that override default caching behavior
    cache_size = Integer.getInteger ("gnu.java.net.dns_cache_size",
				     DEFAULT_CACHE_SIZE).intValue ();
    cache_period = Integer.getInteger ("gnu.java.net.dns_cache_period",
				       DEFAULT_CACHE_PERIOD * 60 *
				       1000).intValue ();

    cache_purge_pct = Integer.getInteger ("gnu.java.net.dns_cache_purge_pct",
					  DEFAULT_CACHE_PURGE_PCT).
      intValue ();

    // Fallback to  defaults if necessary
    if ((cache_purge_pct < 1) || (cache_purge_pct > 100))
      cache_purge_pct = DEFAULT_CACHE_PURGE_PCT;

    // Create the cache
    if (cache_size != 0)
      cache = new HashMap (cache_size);

    // precompute the ANY_IF address
    try
      {
        ANY_IF = getInaddrAny ();
      }
    catch (UnknownHostException uhe)
      {
        // Hmmm, make one up and hope that it works.
        byte[]zeros = { 0, 0, 0, 0 };
        ANY_IF = new InetAddress (zeros);
      }

    byte[] loopback_bits = new byte[] { 127, 0, 0, 1};
    LOOPBACK_ADDRESS = new InetAddress (loopback_bits, "localhost");
  }

  /**
   * An array of octets representing an IP address
   */
  transient byte[] addr;

  /**
   * The name of the host for this address
   */
  String hostName;

  /**
   * Backup hostname alias for this address.
   */
  transient String hostname_alias;

  /**
   * The time this address was looked up
   */
  transient long lookup_time;

  /**
   * Required for serialized form
   */
  int address;
  int family;

  /**
   * This method checks the DNS cache to see if we have looked this hostname
   * up before. If so, we return the cached addresses unless it has been in the
   * cache too long.
   *
   * @param hostname The hostname to check for
   *
   * @return The InetAddress for this hostname or null if not available
   */
  private static synchronized InetAddress[] checkCacheFor (String hostname)
  {
    InetAddress[]addresses = null;

    if (cache_size == 0)
      return (null);

    Object obj = cache.get (hostname);
    if (obj == null)
      return (null);

    if (obj instanceof InetAddress[])
      addresses = (InetAddress[])obj;

    if (addresses == null)
      return (null);

    if (cache_period != -1)
      if ((System.currentTimeMillis () - addresses[0].lookup_time) >
          cache_period)
        {
          cache.remove (hostname);
          return (null);
        }

    return addresses;
  }

  /**
   * This method adds an InetAddress object to our DNS cache.  Note that
   * if the cache is full, then we run a purge to get rid of old entries.
   * This will cause a performance hit, thus applications using lots of
   * lookups should set the cache size to be very large.
   *
   * @param hostname The hostname to cache this address under
   * @param obj The InetAddress or InetAddress array to store
   */
  private static synchronized void addToCache (String hostname, Object obj)
  {
    if (cache_size == 0)
      return;

    // Check to see if hash table is full
    if (cache_size != -1)
      if (cache.size () == cache_size)
        {
          // FIXME Add code to purge later.
        }

    cache.put (hostname, obj);
  }

  /**
   * Returns the special address INADDR_ANY used for binding to a local
   * port on all IP addresses hosted by a the local host.
   *
   * @return An InetAddress object representing INDADDR_ANY
   *
   * @exception UnknownHostException If an error occurs
   */
  static InetAddress getInaddrAny () throws UnknownHostException
  {
    if (inaddr_any == null)
      {
        byte[]tmp = lookupInaddrAny ();
        inaddr_any = new InetAddress (tmp);
      }

    return (inaddr_any);
  }

  /**
   * Returns an array of InetAddress objects representing all the host/ip
   * addresses of a given host, given the host's name.  This name can be
   * either a hostname such as "www.urbanophile.com" or an IP address in
   * dotted decimal format such as "127.0.0.1".  If the value is null, the
   * hostname of the local machine is supplied by default.
   *
   * @param hostname The name of the desired host, or null for the local machine
   *
   * @return All addresses of the host as an array of InetAddress's
   *
   * @exception UnknownHostException If no IP address can be found for the
   * given hostname
   */
  public static InetAddress[] getAllByName (String hostname)
    throws UnknownHostException
  {
    // Default to current host if necessary
    if (hostname == null)
      {
        InetAddress local = getLocalHost ();
        return getAllByName (local.getHostName ());
      }

    // Check the cache for this host before doing a lookup
    InetAddress[] addresses = checkCacheFor (hostname);
    if (addresses != null)
      return (addresses);

    // Not in cache, try the lookup
    byte[][]iplist = getHostByName (hostname);
    if (iplist.length == 0)
      throw new UnknownHostException (hostname);

    addresses = new InetAddress[iplist.length];

    for (int i = 0; i < iplist.length; i++)
      {
        if (iplist[i].length != 4)
          throw new UnknownHostException (hostname);

        // Don't store the hostname in order to force resolution of the
        // canonical names of these ip's when the user asks for the hostname
        // But do specify the host alias so if the IP returned won't
        // reverse lookup we don't throw an exception.
        addresses[i] = new InetAddress (iplist[i], null, hostname);
      }

    addToCache (hostname, addresses);

    return addresses;
  }

  /**
   * Returns an InetAddress object representing the IP address of the given
   * hostname.  This name can be either a hostname such as "www.urbanophile.com"
   * or an IP address in dotted decimal format such as "127.0.0.1".  If the
   * hostname is null, the hostname of the local machine is supplied by
   * default.  This method is equivalent to returning the first element in
   * the InetAddress array returned from GetAllByName.
   *
   * @param hostname The name of the desired host, or null for the local machine
   *
   * @return The address of the host as an InetAddress
   *
   * @exception UnknownHostException If no IP address can be found for the
   * given hostname
   */
  public static InetAddress getByName (String hostname)
    throws UnknownHostException
  {
    // Default to current host if necessary
    if (hostname == null || hostname == "")
      return LOOPBACK_ADDRESS;

    // First, check to see if it is an IP address.  If so, then don't 
    // do a DNS lookup.
    StringTokenizer st = new StringTokenizer (hostname, ".");
    if (st.countTokens () == 4)
      {
        int    i;
        short  n;
        byte[] ip = new byte[4];
        for (i = 0; i < 4; i++)
          {
            try
              {
                n = Short.parseShort(st.nextToken());
                if ((n < 0) || (n > 255))
                  break;
                ip[i] = (byte) n;
              }
            catch (NumberFormatException e)
              {
                break;
              }
          }
        if (i == 4)
          {
            return (new InetAddress (ip));
          }
      }

    // Wasn't an IP, so try the lookup
    InetAddress[]addresses = getAllByName (hostname);

    return addresses[0];
  }

  /**
   * Returns an InetAddress object representing the address of the current
   * host.
   *
   * @return The local host's address
   *
   * @exception UnknownHostException If an error occurs
   */
  public static InetAddress getLocalHost () throws UnknownHostException
  {
    String hostname = getLocalHostName ();
    return getByName (hostname);
  }

  /**
   * Initializes this object's addr instance variable from the passed in
   * int array.  Note that this constructor is protected and is called
   * only by static methods in this class.
   *
   * @param ipaddr The IP number of this address as an array of bytes
   */
  InetAddress (byte[]ipaddr)
  {
    this (ipaddr, null, null);
  }

  /**
   * Initializes this object's addr instance variable from the passed in
   * int array.  Note that this constructor is protected and is called
   * only by static methods in this class.
   *
   * @param ipaddr The IP number of this address as an array of bytes
   * @param hostname The hostname of this IP address.
   */
  InetAddress (byte[]ipaddr, String hostname)
  {
    this (ipaddr, hostname, null);
  }

  /**
   * Initializes this object's addr instance variable from the passed in
   * int array.  Note that this constructor is protected and is called
   * only by static methods in this class.
   *
   * @param ipaddr The IP number of this address as an array of bytes
   * @param hostname The hostname of this IP address.
   * @param hostname_alias A backup hostname to use if hostname is null to
   * prevent reverse lookup failures
   */
  InetAddress (byte[]ipaddr, String hostname, String hostname_alias)
  {
    addr = new byte[ipaddr.length];

    for (int i = 0; i < ipaddr.length; i++)
      addr[i] = ipaddr[i];

    this.hostName = hostname;
    this.hostname_alias = hostname_alias;
    lookup_time = System.currentTimeMillis ();

    family = 2;			/* AF_INET */
    address = addr[3] & 0xff;
    address |= ((addr[2] << 8) & 0xff00);
    address |= ((addr[1] << 16) & 0xff0000);
    address |= ((addr[0] << 24) & 0xff000000);

  }

  /**
   * Tests this address for equality against another InetAddress.  The two
   * addresses are considered equal if they contain the exact same octets.
   * This implementation overrides Object.equals()
   *
   * @param obj The address to test for equality
   *
   * @return true if the passed in object's address is equal to this one's,
   * false otherwise
   */
  public boolean equals (Object obj)
  {
    if (!(obj instanceof InetAddress))
      return false;

    byte[]test_ip = ((InetAddress) obj).getAddress ();

    if (test_ip.length != addr.length)
      return (false);

    for (int i = 0; i < addr.length; i++)
      if (test_ip[i] != (byte) addr[i])
        return (false);

    return (true);
  }

  /**
   * Returns the IP address of this object as a int array.
   *
   * @return IP address
   */
  public byte[] getAddress ()
  {
    byte[]ipaddr = new byte[addr.length];

    for (int i = 0; i < addr.length; i++)
      {
        ipaddr[i] = (byte) addr[i];
      }

    return ipaddr;
  }

  /**
   * Returns the IP address of this object as a String.  The address is in 
   * the dotted octet notation, for example, "127.0.0.1".
   *
   * @return The IP address of this object in String form
   */
  public String getHostAddress ()
  {
    StringBuffer sb = new StringBuffer ();

    for (int i = 0; i < addr.length; i++)
      {
        sb.append (addr[i] & 0xff);
        if (i < (addr.length - 1))
          sb.append (".");
      }

    return sb.toString ();
  }

  /**
   * Returns the hostname for this address.  This will return the IP address
   * as a String if there is no hostname available for this address
   *
   * @return The hostname for this address
   */
  public String getHostName ()
  {
    if (hostName != null)
      return (hostName);

    try
      {
        hostName = getHostByAddr (addr);
        return (hostName);
      }
    catch (UnknownHostException e)
      {
        if (hostname_alias != null)
          return (hostname_alias);
        else
          return (getHostAddress ());
      }
  }

  /**
   * Returns a hash value for this address.  Useful for creating hash
   * tables.  Overrides Object.hashCode()
   *
   * @return A hash value for this address.
   */
  public int hashCode ()
  {
    long val1 = 0, val2 = 0;

    // Its obvious here that I have no idea how to generate a good
    // hash key
    for (int i = 0; i < addr.length; i++)
      val1 = val1 + (addr[i] << ((addr.length - i) / 8));

    for (int i = 0; i < addr.length; i++)
      val2 = val2 + (addr[i] * 10 * i);

    val1 = (val1 >> 1) ^ val2;

    return ((int) val1);
  }

  /**
   * Returns true if this address is a wildcard address, false otherwise.
   * 
   * @return true if it is wildcard address
   * @since 1.4
   */
  public boolean isAnyLocalAddress()
  {
    return equals(ANY_IF);
  }

  /**
   * Returns true if this address represents a loopback address.
   *
   * @return true if this is the loopback address, false otherwise.
   * @since 1.4
   */
  public boolean isLoopbackAddress()
  {
    return equals(LOOPBACK_ADDRESS);
  }

  /**
   * Returns true if this address is a multicast address, false otherwise.
   * An address is multicast if the high four bits are "1110".  These are
   * also known as "Class D" addresses.
   *
   * @return true if mulitcast, false if not
   */
  public boolean isMulticastAddress ()
  {
    if (addr.length == 0)
      return (false);

    // Mask against high order bits of 1110
    if ((addr[0] & 0xF0) == 224)
      return (true);

    return (false);
  }

  /**
   * Converts this address to a String.  This string contains the IP in
   * dotted decimal form. For example: "127.0.0.1"  This method is equivalent
   * to getHostAddress() and overrides Object.toString()
   *
   * @return This address in String form
   */
  public String toString ()
  {
    StringBuffer sb;
    if (hostName != null)
      sb = new StringBuffer (hostName).append ('/');
    else if (hostname_alias != null)
      sb = new StringBuffer (hostname_alias).append ('/');
    else
      sb = new StringBuffer ();

    sb.append (getHostAddress ());
    return (sb.toString ());
  }

  /**
   * Returns an InetAddress object given the raw IP address.
   *
   * The argument is in network byte order: the highest order byte of the
   * address is in getAddress()[0].
   *
   * @param addr The IP address to create the InetAddress object from
   *
   * @exception UnknownHostException If IP address has illegal length
   *
   * @since 1.4
   */
  public static InetAddress getByAddress (byte[]addr)
    throws UnknownHostException
  {
    if (addr.length != 4 && addr.length != 16)
      throw new UnknownHostException ("IP address has illegal length");

    if (addr.length == 4)
      return new Inet4Address (addr, null);

    return new Inet6Address (addr, null);
  }

  /**
   * This native method looks up the hostname of the local machine
   * we are on.  If the actual hostname cannot be determined, then the
   * value "localhost" we be used.  This native method wrappers the
   * "gethostname" function.
   *
   * @return The local hostname.
   */
  private static native String getLocalHostName ();

  /**
   * Returns the value of the special address INADDR_ANY
   */
  private static native byte[] lookupInaddrAny () throws UnknownHostException;

  /**
   * This method returns the hostname for a given IP address.  It will
   * throw an UnknownHostException if the hostname cannot be determined.
   *
   * @param ip The IP address as a int array
   * 
   * @return The hostname
   *
   * @exception UnknownHostException If the reverse lookup fails
   */
  private static native String getHostByAddr (byte[] ip)
    throws UnknownHostException;

  /**
   * Returns a list of all IP addresses for a given hostname.  Will throw
   * an UnknownHostException if the hostname cannot be resolved.
   */
  private static native byte[][] getHostByName (String hostname)
    throws UnknownHostException;
}
