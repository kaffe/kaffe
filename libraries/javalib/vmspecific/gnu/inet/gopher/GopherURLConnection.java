/*
 * GopherURLConnection.java
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
import java.net.UnknownServiceException;
import java.net.URL;
import java.net.URLConnection;

/**
 * A Gopher URL connection.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class GopherURLConnection
  extends URLConnection
{

  /**
   * The connection managing the protocol exchange.
   */
  protected GopherConnection connection;

  /**
   * Constructs a Gopher connection to the specified URL.
   * @param url the URL
   */
  public GopherURLConnection(URL url)
  {
    super(url);
  }
  
  /**
   * Establishes the connection.
   */
  public void connect()
    throws IOException
  {
    if (connected)
      {
        return;
      }
    String host = url.getHost();
    int port = url.getPort();
    connection = new GopherConnection(host, port);
  }
  
  /**
   * Returns an input stream that reads from this open connection.
   */
  public InputStream getInputStream()
    throws IOException
  {
    if (!connected)
      {
        connect();
      }
    String dir = url.getPath();
    String filename = url.getFile();
    if (dir == null && filename == null)
      {
        throw new UnsupportedOperationException("not implemented");
      }
    else
      {
        String selector = (dir == null) ? filename :
          dir + '/' + filename;
        return connection.get(selector);
      }
  }

  /**
   * Returns an output stream that writes to this connection.
   */
  public OutputStream getOutputStream()
    throws IOException
  {
    throw new UnknownServiceException();
  }

  public Object getContent()
    throws IOException
  {
    return new GopherContentHandler().getContent(this);
  }

  public Object getContent(Class[] classes)
    throws IOException
  {
    return new GopherContentHandler().getContent(this, classes);
  }

}

