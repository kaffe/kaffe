/* 
 * Copyright (C) 2003, 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Classpathx/jaxp.
 * 
 * GNU Classpath is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *  
 * GNU Classpath is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Classpath; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
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
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
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
