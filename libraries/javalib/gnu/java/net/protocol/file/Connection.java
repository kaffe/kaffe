/* FileURLConnection.java -- URLConnection class for "file" protocol
   Copyright (C) 1998, 1999, 2003 Free Software Foundation, Inc.

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
exception statement from your version.  */

package gnu.java.net.protocol.file;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FilePermission;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.ProtocolException;
import java.net.URL;
import java.net.URLConnection;
import java.security.Permission;
import java.util.AbstractSet;
import java.util.Iterator;
import java.util.Set;
import java.util.NoSuchElementException;

/**
 * This subclass of java.net.URLConnection models a URLConnection via
 * the "file" protocol.
 *
 * @author Aaron M. Renn <arenn@urbanophile.com>
 * @author Nic Ferrier <nferrier@tapsellferrier.co.uk>
 * @author Warren Levy <warrenl@cygnus.com>
 */
public class Connection extends URLConnection
{
  /**
   * Default permission for a file
   */
  private static final String DEFAULT_PERMISSION = "read";

  /**
   * This is a File object for this connection
   */
  private File file;

  /**
   * InputStream if we are reading from the file
   */
  private InputStream inputStream;

  /**
   * OutputStream if we are writing to the file
   */
  private OutputStream outputStream;
  
  /**
   * FilePermission to read the file
   */
  private FilePermission permission;

  /**
   * Calls superclass constructor to initialize.
   */
  public Connection(URL url)
  {
    super (url);

    permission = new FilePermission(getURL().getFile(), DEFAULT_PERMISSION);
  }
  
  /**
   * "Connects" to the file by opening it.
   */
  public void connect() throws IOException
  {
    // Call is ignored if already connected.
    if (connected)
      return;
    
    // If not connected, then file needs to be openned.
    file = new File (getURL().getFile());
    if (doInput)
      inputStream = new BufferedInputStream(new FileInputStream(file));
    
    if (doOutput)
      outputStream = new BufferedOutputStream(new FileOutputStream(file));
    
    connected = true;
  }
  
  /**
   * Opens the file for reading and returns a stream for it.
   *
   * @return An InputStream for this connection.
   *
   * @exception IOException If an error occurs
   */
  public InputStream getInputStream()
    throws IOException
  {
    if (!doInput)
      throw new ProtocolException("Can't open InputStream if doInput is false");
    
    if (!connected)
      connect();
    
    return inputStream;
  }

  /**
   * Opens the file for writing and returns a stream for it.
   *
   * @return An OutputStream for this connection.
   *
   * @exception IOException If an error occurs.
   */
  public OutputStream getOutputStream()
    throws IOException
  {
    if (!doOutput)
      throw new
	ProtocolException("Can't open OutputStream if doOutput is false");

    if (!connected)
      connect();
    
    return outputStream;
  }

  /**
   * Get the last modified time of the resource.
   *
   * @return the time since epoch that the resource was modified.
   */
  public long getLastModified()
  {
    try
      {
	if (!connected)
	  connect();

	return file.lastModified();
      }
    catch (IOException e)
      {
	return -1;
      }
  }

  /**
   * Get the length of content.
   *
   * @return the length of the content.
   */
  public int getContentLength()
  {
    try
      {
	if (!connected)
	  connect();
        
	return (int) file.length();
      }
    catch (IOException e)
      {
	return -1;
      }
  }
  
  /**
   * This method returns a <code>Permission</code> object representing the
   * permissions required to access this URL.  This method returns a
   * <code>java.io.FilePermission</code> for the file's path with a read
   * permission.
   *
   * @return A Permission object
   */
  public Permission getPermission() throws IOException
  {
    return permission;
  }

  /**
   * Does the resource pointed to actually exist?
   */
  public final boolean exists()
  {
    if (file == null)
      return false;

    return file.exists();
  }

  /**
   * Is the resource pointed to a directory?
   */
  public final boolean isDirectory()
  {
    return file.isDirectory();
  }
  
  /**
   * Get a listing of the directory, if it is a directory.
   *
   * @return a set which can supply an iteration of the
   * contents of the directory.
   *
   * @throws IllegalStateException if this is not pointing
   * to a directory.
   */
  public Set getListing()
  {
    if (!file.isDirectory())
      throw new IllegalStateException ("this is not a directory");
    
    final File[] directoryList = file.listFiles();
    return new AbstractSet()
      {
	File[] dirList = directoryList;

	public int size()
	{
	  return dirList.length;
	}

	public Iterator iterator()
	{
	  return new Iterator()
	    {
	      int index = 0;

	      public boolean hasNext()
	      {
		return index < dirList.length;
	      }

	      public Object next()
	      {
		try
		  {
		    String value = dirList [index++].getName();
		    return value;
		  }
		catch (ArrayIndexOutOfBoundsException e)
		  {
		    throw new NoSuchElementException ("no more content");
		  }
	      }

	      public void remove()
	      {
		try
		  {
		    File[] newDirList = new File [dirList.length - 1];
		    int realIndex = index - 1;
                    
		    if (realIndex < 1)
		      {
			System.arraycopy (dirList, 1, newDirList, 0,
                                          dirList.length - 1);
			index--;
		      }
		    else
		      {
			System.arraycopy (dirList, 0, newDirList, 0, realIndex);
                        
			if (index < dirList.length - 1)
			  System.arraycopy (dirList, index,
					    newDirList, realIndex,
                                            dirList.length - realIndex);
		      }
		    dirList = newDirList;
		  }
		catch (ArrayIndexOutOfBoundsException e)
		  {
		    throw new NoSuchElementException("no more content");
		  }
	      }
	    };
	}
      };
  }
}
