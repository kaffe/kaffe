/* 
 * $Id: IOToolkit.java,v 1.1 2004/04/14 19:40:35 dalibor Exp $
 * Copyright (C) 2003 Julian Scheid
 * 
 * This file is part of GNU LibxmlJ, a JAXP-compliant Java wrapper for
 * the XML and XSLT C libraries for Gnome (libxml2/libxslt).
 * 
 * GNU LibxmlJ is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *  
 * GNU LibxmlJ is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU LibxmlJ; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA. 
 */

package gnu.xml.libxmlj.transform;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PushbackInputStream;

import java.net.URL;

import javax.xml.transform.Result;
import javax.xml.transform.Source;
import javax.xml.transform.TransformerException;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

class IOToolkit
{

  /**
   *  Prevent instantiation.
   */
  private IOToolkit ()
  {
  }

  /**
   *  Read the XML document described by the given xmlSource fully
   *  into memory and return as a byte array.
   */
  static PushbackInputStream getSourceInputStream (Source xmlSource) 
    throws TransformerException
  {
    try
      {
        InputStream in = null;
        if (xmlSource instanceof StreamSource)
          {
            StreamSource streamSource = (StreamSource) xmlSource;
	    in = streamSource.getInputStream ();
          }

        if (null == in)
          {
            try
              {
                String filename = xmlSource.getSystemId ();
                if (filename.startsWith ("file:"))
                  filename = filename.substring (5);
                in = new FileInputStream (filename);
              }
            catch (FileNotFoundException ignore)
              {
                in = new URL (xmlSource.getSystemId ()).openStream ();
              }
          }
        return new PushbackInputStream (in, 50);
      }
    catch (Exception e)
      {
        throw new TransformerException (e);
      }
  }

  static OutputStream
  getResultOutputStream (Result result) throws TransformerException
  {
    try
      {
        OutputStream out = null;
        if (result instanceof StreamResult)
          {
            StreamResult streamResult = (StreamResult) result;
	    out = streamResult.getOutputStream ();
          }

        if (null == out)
          {
            String filename = result.getSystemId ();
            if (filename.startsWith ("file:"))
              {
                filename = filename.substring (5);
              }
            out = new FileOutputStream (filename);
          }
        return out;
      }
    catch (Exception e)
      {
        throw new TransformerException (e);
      }
  }
}
