/* DNSJavaInetAddressImpl.java -- Internet address implementation using
   DNSJava.
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

import java.lang.reflect.Array;
import java.net.UnknownHostException;
import java.io.IOException;
import org.xbill.DNS.Lookup;
import org.xbill.DNS.Type;
import org.xbill.DNS.Record;
import org.xbill.DNS.ARecord;
import org.xbill.DNS.TextParseException;
import org.xbill.DNS.Resolver;
import org.xbill.DNS.Message;
import org.xbill.DNS.ExtendedResolver;
import org.xbill.DNS.Section;
import org.xbill.DNS.DClass;
import org.xbill.DNS.ReverseMap;

public class DNSJavaInetAddressImpl implements InetAddressImpl
{
  static private Resolver resolver = null;

  public String getHostByAddr(byte[] ip)
    throws UnknownHostException
  {
    if (resolver == null)
      resolver = new ExtendedResolver();

    int[] addr = new int[ip.length];
    for (int i = 0; i < ip.length; i++)
      addr[i] = ip[i];

    Record rec = Record.newRecord(ReverseMap.fromAddress(addr), Type.PTR, DClass.IN);
    Message query = Message.newQuery(rec);
    Message response;
    Record[] rec_responses;

    try
      {
	response = resolver.send(query);
      }
    catch (IOException e)
      {
	throw new UnknownHostException (e.getMessage());
      }

    rec_responses = response.getSectionArray(Section.ANSWER);
    
    return rec_responses[0].getName().toString();
  }

  public byte[][] getHostByName(String hostname)
    throws UnknownHostException
  {
    Lookup lookup;

    try
      {
	lookup = new Lookup(hostname, Type.A);
      }
    catch (TextParseException e)
      {
	throw new UnknownHostException("Invalid host name");
      }

    lookup.run();
    
    int result = lookup.getResult();
    if (result != Lookup.SUCCESSFUL)
      throw new UnknownHostException(lookup.getErrorString());
    
    Record[] answers = lookup.getAnswers();
    byte[][] addresses = (byte[][])
      Array.newInstance(Byte.TYPE, 
			new int[] { answers.length, 8 });
    for (int i = 0; i < answers.length; i++)
      {
	if (answers[i] instanceof ARecord)
	  continue;

	System.arraycopy(((ARecord)answers[i]).getAddress().getAddress(), 0,
			 addresses[i], 0, 8);
      }

    return addresses;
  }
}
