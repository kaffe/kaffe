/*
 * $Id: FTPURLConnection.java,v 1.2 2004/03/22 11:24:08 dalibor Exp $
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
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.URLConnection;

/**
 * An FTP URL connection.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.2 $ $Date: 2004/03/22 11:24:08 $
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
  public FTPURLConnection(URL url)
  {
    super(url);
  }

        /**
	 * Establishes the connection.
	 */
  public void connect() throws IOException
  {
    if (connected)
      return;
    String host = url.getHost();
    int port = url.getPort();
    String username = url.getUserInfo();
    String password = null;
    if (username != null)
    {
      int ci = username.indexOf(':');
      if (ci != -1)
      {
        password = username.substring(ci + 1);
        username = username.substring(0, ci);
      }
    }
    connection = new FTPConnection(host, port);
    if (username != null && !connection.authenticate(username, password))
      throw new SecurityException("Authentication failed");
  }

        /**
	 * This connection supports doInput.
	 */
  public void setDoInput(boolean doinput)
  {
    doInput = doinput;
  }

        /**
	 * This connection supports doOutput.
	 */
  public void setDoOutput(boolean dooutput)
  {
    doOutput = dooutput;
  }

        /**
	 * Returns an input stream that reads from this open connection.
	 */
  public InputStream getInputStream() throws IOException
  {
    String dir = url.getPath();
    String filename = url.getFile();
      connection.setRepresentationType(FTPConnection.TYPE_BINARY);
      connection.setPassive(true);
    if (!connection.changeWorkingDirectory(dir))
      throw new FileNotFoundException(dir);
    if (filename != null)
        return connection.retrieve(filename);
    else
        return connection.list(null);
    // TODO provide a means to close the connection
  }

        /**
	 * Returns an output stream that writes to this connection.
	 */
  public OutputStream getOutputStream() throws IOException
  {
    String dir = url.getPath();
    String filename = url.getFile();
      connection.setRepresentationType(FTPConnection.TYPE_BINARY);
      connection.setPassive(true);
    if (!connection.changeWorkingDirectory(dir))
      throw new FileNotFoundException(dir);
    if (filename != null)
        return connection.store(filename);
    else
        throw new FileNotFoundException(filename);
    // TODO provide a means to close the connection
  }

  // TODO allow user to configure the FTPConnection using setRequestProperty

}
