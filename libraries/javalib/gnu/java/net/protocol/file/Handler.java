/* Handler.java -- "file" protocol handler for java.net
   Copyright (C) 1998, 1999, 2000, 2002, 2003 Free Software Foundation, Inc.

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

package gnu.java.net.protocol.file;

import gnu.java.io.PlatformHelper;
import java.io.IOException;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLStreamHandler;

/**
 * This is the protocol handler for the "file" protocol.
 * It implements the abstract openConnection() method from
 * URLStreamHandler by returning a new FileURLConnection object (from
 * this package).  All other methods are inherited
 *
 * @author Aaron M. Renn (arenn@urbanophile.com)
 * @author Warren Levy <warrenl@cygnus.com>
 */
public class Handler extends URLStreamHandler
{
  /**
   * A do nothing constructor
   */
  public Handler()
  {
  }

  /**
   * This method returs a new FileURLConnection for the specified URL
   *
   * @param url The URL to return a connection for
   *
   * @return The URLConnection
   *
   * @exception IOException If an error occurs
   */
  protected URLConnection openConnection(URL url) throws IOException
  {
    // If a hostname is set, then we need to switch protocols to ftp
    // in order to transfer this from the remote host.
    String host = url.getHost();
    if ((host != null) && (! host.equals("")))
      {
        throw new IOException("ftp protocol handler not yet implemented.");
        /*
	// Reset the protocol (and implicitly the handler) for this URL.
	// Then have the URL attempt the connection again, as it will
	// get the changed handler the next time around.
	setURL (url, "ftp", url.getHost(), url.getPort(), url.getFile(),
		url.getRef());
	// Until the ftp protocol handler is written, this will cause
	// a NullPointerException.
	return url.openConnection();
	*/
      }

    return new Connection(url);
  }

  /**
   * This method overrides URLStreamHandler's for parsing url of protocol "file"
   *
   * @param url The URL object in which to store the results
   * @param url_string The String-ized URL to parse
   * @param start The position in the string to start scanning from
   * @param end The position in the string to stop scanning
   */
  protected void parseURL (URL url, String url_string, int start, int end)
  {
    // This method does not throw an exception or return a value.  Thus our
    // strategy when we encounter an error in parsing is to return without
    // doing anything.

    // Bunches of things should be true.  Make sure.
    if (end < start)
        return;
    if (end - start < 2)
        return;
    if (start > url_string.length())
        return;
    if (end > url_string.length())
        end = url_string.length(); // This should be safe
    
    // Turn end into an offset from the end of the string instead of 
    // the beginning
    end = url_string.length() - end;
    
    // Skip remains of protocol
    url_string = url_string.substring (start);
    
    if ( !url.getProtocol().equals ("file"))
      return;
    
    // Normalize the file separator
    url_string = url_string.replace
      (System.getProperty ("file.separator").charAt (0), '/');
    
    // Deal with the case: file:///d|/dir/dir/file and file:///d%7C/dir/dir/file
    url_string = url_string.replace ('|', ':');
    int i;
    
    if ((i = url_string.toUpperCase().indexOf ("%7C")) >= 0)
      url_string = url_string.substring (0, i) + ":" + url_string.substring (i + 3);

    boolean needContext = url.getFile() != null;
    // Skip the leading "//"
    if (url_string.startsWith ("//"))
      {
        url_string = url_string.substring (2);
        needContext = false;
      }

    // Declare some variables
    String host = null;
    int port = -1;
    String file = null;
    String anchor = null;
    String prefix = "/";  //root path prefix of a file: could be "/", and for some windows file: "drive:/"
    
    if (!needContext)
      {
        boolean hostpart = true; //whether host part presents
        
        // Deal with the UNC case: //server/file
        if (url_string.startsWith ("//"))
          {
            hostpart = true;
            url_string = url_string.substring (2);
          }
        else
          {
            // If encounter another "/", it's end of a null host part or beginning of root path 
            if (url_string.startsWith ("/"))
              {
                hostpart = false;
                // url_string = url_string.substring (1);
              }
          }
        
        // If another "/" or "drive:/" or "drive:\\" encounters, 
        if ((i = PlatformHelper.beginWithRootPathPrefix (url_string)) > 0)
          {
            hostpart = false;
            // Skip root path prefix
            prefix = url_string.substring (0, i);
            url_string = url_string.substring(i);
          }
        
        if (hostpart)
          {
            // Process host and port
            int slash_index = url_string.indexOf ("/");
            int colon_index = url_string.indexOf (":");
        
            if (slash_index > (url_string.length() - end))
              return;
            else if (slash_index == -1)
              slash_index = url_string.length() - end;
        
            if ((colon_index == -1)
                || (colon_index > slash_index))
              {
                host = url_string.substring (0, slash_index);
              }
            else
              {
                host = url_string.substring (0, colon_index);
                String port_str = url_string.substring (colon_index + 1,
                                                        slash_index);

                try
                  {
                    port = Integer.parseInt(port_str);
                  }
                catch (NumberFormatException e)
                  {
                    return;
                  }
              }
            
            if (slash_index < (url_string.length() - 1))
              url_string = url_string.substring (slash_index + 1);
            else
              url_string = "";
          }
      }
    
    // Process file and anchor 
    if (needContext)
      {
        host = url.getHost();
        port = url.getPort();

        if ((i = PlatformHelper.beginWithRootPathPrefix (url_string)) > 0)
          { //url string is an absolute path
            file = url.getFile();
            int j = PlatformHelper.beginWithRootPathPrefix (file);

            if (j >= i)
              file = file.substring (0, j) + url_string.substring (i);
            else
              file = url_string;
          }
        else
          {
            file = url.getFile();

            /*
            // Is the following necessary?
            java.io.File f = new java.io.File(file);
            if(f.isDirectory() && !PlatformHelper.endWithSeparator(file)){
                file += "/";
            }
            */
            
            int idx = file.lastIndexOf ("/");  

            if (idx == -1) //context path is weird
              file = "/" + url_string; 
            else if (idx == (file.length() - 1))
              //just concatenate two parts
              file = file + url_string;
            else
              file = file.substring (0, idx + 1) + url_string;
          }
      }
    else
      file = prefix + url_string;  
        
    if (end == 0)
      {
        anchor = null;
      }
    else
      {
        // Only set anchor if end char is a '#'.  Otherwise assume we're
        // just supposed to stop scanning for some reason
        if (file.charAt (file.length() - end) == '#')
          {
            int len = file.length();
            anchor = file.substring ( len - end + 1, len);
            file = file.substring (0, len - end);
          }
        else
          anchor = null;
      }
    
    file = PlatformHelper.toCanonicalForm (file, '/');

    if (host == null)
      {
        host = "";
      }

    // Now set the values
    setURL (url, url.getProtocol(), host, port, file, anchor); 
  }

  /**
   * This method overrides URLStreamHandler's as a specialized 
   * and more efficient toExternalForm 
   *
   * @param url The URL object whose external form will be returned 
   */
  protected String toExternalForm (URL url)
  { 
    StringBuffer sb = new StringBuffer (PlatformHelper.INITIAL_MAX_PATH);
    sb.append ("file:");
    String prefix = url.getHost();

    if (prefix != null
        && prefix.length() > 0)
      {
        sb.append (prefix);
        int port = url.getPort();
        
        if (port > 0)
          {
            sb.append (':');
            sb.append (port);
          }
      }
    
    String file = url.getFile();
    if (file != null)
      sb.append (file);
    else
      sb.append ('/');
    
    String anchor = url.getRef();
    if (anchor != null)
      {
        sb.append ('#');
        sb.append (anchor);
      }
    
    return sb.toString();
  }
} // class Handler
