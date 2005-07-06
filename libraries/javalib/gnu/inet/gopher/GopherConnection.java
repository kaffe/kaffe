/*
 * GopherConnection.java
 * Copyright (C) 2003 The Free Software Foundation
 * 
 * This file is part of GNU inetlib, a library.
 * 
 * GNU inetlib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU inetlib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obliged to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 */

package gnu.inet.gopher;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;

import gnu.inet.util.CRLFInputStream;
import gnu.inet.util.MessageInputStream;

/**
 * A gopher client.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class GopherConnection
{

  /**
   * The default gopher port.
   */
  public static final int DEFAULT_PORT = 80;

  protected Socket socket;
  protected InputStream in;
  protected OutputStream out;

  /**
   * Creates a new connection to the gopher server at the specified
   * hostname.
   * @param hostname the hostname
   */
  public GopherConnection(String host)
    throws IOException
  {
    this(host, DEFAULT_PORT);
  }
  
  /**
   * Creates a new connection to the gopher server at the specified
   * hostname with the specified non-standard port.
   * @param hostname the hostname
   * @param port the non-standard port to use
   */
  public GopherConnection(String host, int port)
    throws IOException
  {
    if (port <= 0)
      {
        port = DEFAULT_PORT;
      }
    
    socket = new Socket(host, port);
    in = socket.getInputStream();
    out = socket.getOutputStream();
  }
  
  /**
   * Returns the directory listing for this gopher server.
   * When all entries have been read from the listing, the connection will
   * be closed.
   */
  public DirectoryListing list()
    throws IOException
  {
    byte[] CRLF = { 0x0d, 0x0a };
    out.write(CRLF);
    out.flush();
    InputStream listStream = new CRLFInputStream(in);
    listStream = new MessageInputStream(listStream);
    return new DirectoryListing(listStream);
  }
  
  /**
   * Returns the resource identified by the specified selector.
   * If the resource is text-based, it will need to be wrapped in a
   * CRLFInputStream.
   */
  public InputStream get(String selector)
    throws IOException
  {
    byte[] chars = selector.getBytes("US-ASCII");
    byte[] line = new byte[chars.length + 2];
    System.arraycopy(chars, 0, line, 0, chars.length);
    line[chars.length] = 0x0d;
    line[chars.length + 1] = 0x0a;
    out.write(line);
    out.flush();
    return in;
  }

}

