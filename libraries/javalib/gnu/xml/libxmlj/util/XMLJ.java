/*
 * XMLJ.java
 * Copyright (C) 2004 The Free Software Foundation
 * 
 * This file is part of GNU JAXP, a library.
 * 
 * GNU JAXP is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU JAXP is distributed in the hope that it will be useful,
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
package gnu.xml.libxmlj.util;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PushbackInputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;

import javax.xml.transform.Result;
import javax.xml.transform.Source;
import javax.xml.transform.sax.SAXSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import org.xml.sax.InputSource;

import gnu.xml.libxmlj.transform.GnomeTransformerFactory;

/**
 * Utility functions for libxmlj.
 */
public final class XMLJ
{

  static class XMLJShutdownHook
    implements Runnable
  {
    
    public void run ()
    {
      // Make sure finalizers are run
      System.gc ();
      Runtime.getRuntime ().runFinalization ();
      
      // Perform global cleanup on the native level
      GnomeTransformerFactory.freeLibxsltGlobal ();
    }
  
  }

  private static boolean initialised = false;
  
  public static void init ()
  {
    if (!initialised)
      {
        System.loadLibrary ("xmlj");
        
        XMLJShutdownHook hook = new XMLJShutdownHook ();  
        Runtime.getRuntime ().addShutdownHook (new Thread (hook));
      }
    initialised = true;
  }

  private static final int LOOKAHEAD = 50;
  
  /**
   * Returns an input stream for the specified input source.
   * This returns a pushback stream that libxmlj can use to detect the
   * character encoding of the stream.
   */
  public static NamedInputStream getInputStream (InputSource input)
    throws IOException
  {
    InputStream in = input.getByteStream ();
    String systemId = input.getSystemId ();
    if (in == null)
      {
        if (systemId == null)
          {
            throw new IOException ("no system ID");
          }
        try
          {
            in = new URL (systemId).openStream ();
          }
        catch (MalformedURLException e)
          {
            in = new FileInputStream (systemId);
          }
      }
    return new NamedInputStream (systemId, in, LOOKAHEAD);
  }

  /**
   * Returns an input stream for the specified transformer source.
   * This returns a pushback stream that libxmlj can use to detect the
   * character encoding of the stream.
   */
  public static NamedInputStream getInputStream (Source source)
    throws IOException
  {
    if (source instanceof SAXSource)
      {
        return getInputStream (((SAXSource) source).getInputSource ());
      }
    InputStream in = null;
    String systemId = source.getSystemId ();
    if (source instanceof StreamSource)
      {
        in = ((StreamSource) source).getInputStream ();
      }
    if (in == null)
      {
        if (systemId == null)
          {
            throw new IOException ("no system ID");
          }
        try
          {
            in = new URL (systemId).openStream ();
          }
        catch (MalformedURLException e)
          {
            in = new FileInputStream (systemId);
          }
      }
    return new NamedInputStream (systemId, in, LOOKAHEAD);
  }

  /**
   * Returns an input stream for the specified URL.
   * This returns a pushback stream that libxmlj can use to detect the
   * character encoding of the stream.
   */
  public static NamedInputStream getInputStream (URL url)
    throws IOException
  {
    return new NamedInputStream (url.toString (), url.openStream(),
                                 LOOKAHEAD);
  }

  /**
   * Returns an output stream for the specified transformer result.
   */
  public static OutputStream getOutputStream (Result result)
    throws IOException
  {
    OutputStream out = null;
    if (result instanceof StreamResult)
      {
        out = ((StreamResult) result).getOutputStream ();
      }
    if (out == null)
      {
        String systemId = result.getSystemId ();
        if (systemId == null)
          {
            throw new IOException ("no system ID");
          }
        try
          {
            URL url = new URL (systemId);
            URLConnection connection = url.openConnection ();
            connection.setDoOutput (true);
            out = connection.getOutputStream ();
          }
        catch (MalformedURLException e)
          {
            out = new FileOutputStream (systemId);
          }
      }
    return out;
  }

  /**
   * Returns the absolute form of the specified URI.
   * If the URI is already absolute, returns it as-is.
   * Otherwise returns a new URI relative to the given base URI.
   */
  public static String getAbsoluteURI (String base, String uri)
  {
    if (uri != null &&
        base != null &&
        (uri.length () > 0) &&
        (uri.indexOf (':') == -1) &&
        (uri.charAt (0) != '/'))
      {
        // URI is relative
        return base + uri;
      }
    else
      {
        // URI is absolute or no base specified
        return uri;
      }
  }

  public static String getBaseURI (String uri)
  {
    if (uri != null)
      {
        int si = uri.lastIndexOf('/');
        if (si != -1)
          {
            uri = uri.substring (0, si + 1);
          }
      }
    return uri;
  }
  
}
