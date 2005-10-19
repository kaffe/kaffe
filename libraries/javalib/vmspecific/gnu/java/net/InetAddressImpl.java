/* InetAddressImpl.java -- Class to model an Internet address implementation
   Copyright (C) 2003 Free Software Foundation, Inc.

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
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

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
package gnu.java.net;

import java.net.UnknownHostException;

/**
 * This interface precises the shape of the implementation of an InetAddress.
 * This interface can be used to build new way to resolve host names. This
 * can be useful when system dns resolution is failing in a way or another.
 *
 * @author Guilhem Lavaux
 */
public interface InetAddressImpl {
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
  public String getHostByAddr (byte[] ip)
    throws UnknownHostException;

  /**
   * Returns a list of all IP addresses for a given hostname.  Will throw
   * an UnknownHostException if the hostname cannot be resolved.
   */
  public byte[][] getHostByName (String hostname)
    throws UnknownHostException;
}
