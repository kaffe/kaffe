/*
 * $Id: GopherURLConnection.java,v 1.3 2004/10/04 19:33:57 robilad Exp $
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
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
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
 * @version $Revision: 1.3 $ $Date: 2004/10/04 19:33:57 $
 */
public class GopherURLConnection extends URLConnection
{

  /**
   * The connection managing the protocol exchange.
   */
  protected GopherConnection connection;

  /**
   * Constructs a Gopher connection to the specified URL.
   * @param url the URL
   */
  public GopherURLConnection (URL url)
    {
      super (url);
    }

  /**
   * Establishes the connection.
   */
  public void connect () throws IOException
    {
      if (connected)
        {
          return;
        }
      String host = url.getHost ();
      int port = url.getPort ();
      connection = new GopherConnection (host, port);
    }

  /**
   * Returns an input stream that reads from this open connection.
   */
  public InputStream getInputStream () throws IOException
    {
      if (!connected)
        {
          connect ();
        }
      String dir = url.getPath ();
      String filename = url.getFile ();
      if (dir == null && filename == null)
        {
          throw new UnsupportedOperationException ("not implemented");
        }
      else
        {
          String selector = (dir == null) ? filename :
            dir + '/' + filename;
          return connection.get (selector);
        }
    }

  /**
   * Returns an output stream that writes to this connection.
   */
  public OutputStream getOutputStream () throws IOException
    {
      throw new UnknownServiceException ();
    }

  public Object getContent () throws IOException
    {
      return new GopherContentHandler ().getContent (this);
    }

  public Object getContent (Class[] classes) throws IOException
    {
      return new GopherContentHandler ().getContent (this, classes);
    }

}
