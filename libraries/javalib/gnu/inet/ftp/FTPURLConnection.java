/*
 * $Id: FTPURLConnection.java,v 1.3 2004/07/25 22:46:18 dalibor Exp $
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

package gnu.inet.ftp;

import java.io.FileNotFoundException;
import java.io.FilterInputStream;
import java.io.FilterOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.URL;
import java.net.URLConnection;
import java.security.AccessController;
import java.security.PrivilegedAction;

import gnu.inet.util.GetLocalHostAction;
import gnu.inet.util.GetSystemPropertyAction;

/**
 * An FTP URL connection.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.3 $ $Date: 2004/07/25 22:46:18 $
 */
public class FTPURLConnection extends URLConnection
{

  /**
   * The connection managing the protocol exchange.
   */
  protected FTPConnection connection;

  /**
   * Constructs an FTP connection to the specified URL.
   * @param url the URL
   */
  public FTPURLConnection (URL url)
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
    String username = url.getUserInfo ();
    String password = null;
    if (username != null)
      {
        int ci = username.indexOf (':');
        if (ci != -1)
          {
            password = username.substring (ci + 1);
            username = username.substring (0, ci);
          }
      }
    else
      {
        username = "anonymous";
        PrivilegedAction a = new GetSystemPropertyAction ("user.name");
        String systemUsername = (String) AccessController.doPrivileged (a);
        a = new GetLocalHostAction ();
        InetAddress localhost = (InetAddress) AccessController.doPrivileged (a);
        password = systemUsername + "@" +
          ((localhost == null) ? "localhost" : localhost.getHostName ());
      }
    connection = new FTPConnection (host, port);
    if (!connection.authenticate (username, password))
      {
        throw new SecurityException ("Authentication failed");
      }
  }
  
  /**
   * This connection supports doInput.
   */
  public void setDoInput (boolean doinput)
  {
    doInput = doinput;
  }

  /**
   * This connection supports doOutput.
   */
  public void setDoOutput (boolean dooutput)
  {
    doOutput = dooutput;
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
    connection.setRepresentationType (FTPConnection.TYPE_BINARY);
    connection.setPassive (true);
    String path = url.getPath ();
    String filename = null;
    int lsi = path.lastIndexOf ('/');
    if (lsi != -1)
      {
        filename = path.substring (lsi + 1);
        path = path.substring (0, lsi);
        if (!connection.changeWorkingDirectory (path))
          {
            throw new FileNotFoundException (path);
          }
      }
    if (filename != null && filename.length () > 0)
      {
        return this.new ClosingInputStream (connection.retrieve (filename));
      }
    else
      {
        return this.new ClosingInputStream (connection.list (null));
      }
  }
  
  /**
   * Returns an output stream that writes to this connection.
   */
  public OutputStream getOutputStream () throws IOException
  {
    if (!connected)
      {
        connect ();
      }
    String dir = url.getPath ();
    String filename = url.getFile ();
    connection.setRepresentationType (FTPConnection.TYPE_BINARY);
    connection.setPassive (true);
    if (!connection.changeWorkingDirectory (dir))
      {
        throw new FileNotFoundException (dir);
      }
    if (filename != null)
      {
        return this.new ClosingOutputStream (connection.store (filename));
      }
    else
      {
        throw new FileNotFoundException (filename);
      }
  }
  
  // TODO allow user to configure the FTPConnection using setRequestProperty

  class ClosingInputStream
    extends FilterInputStream
  {

    ClosingInputStream (InputStream in)
    {
      super (in);
    }

    public void close ()
      throws IOException
    {
      super.close ();
      connection.logout ();
    }
    
  }

  class ClosingOutputStream
    extends FilterOutputStream
  {

    ClosingOutputStream (OutputStream out)
    {
      super (out);
    }

    public void close ()
      throws IOException
    {
      super.close ();
      connection.logout ();
    }
    
  }

}
