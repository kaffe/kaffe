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

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;

import java.util.Iterator;
import java.util.Map;
import java.util.Properties;

import javax.xml.transform.Result;
import javax.xml.transform.Source;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.stream.StreamSource;

import gnu.xml.libxmlj.dom.GnomeDocument;
import gnu.xml.libxmlj.util.NamedInputStream;
import gnu.xml.libxmlj.util.XMLJ;

public class LibxsltStylesheet
{
  private Object nativeStylesheetHandle;
  private Properties outputProperties;
  private static int unfinalized = 0;

  LibxsltStylesheet (Source xsltSource, JavaContext javaContext)
    throws TransformerConfigurationException
  {
    try
      {
        outputProperties = new Properties ();
        NamedInputStream in = XMLJ.getInputStream (xsltSource);
        String systemId = in.getName ();
        String base = XMLJ.getBaseURI (systemId);
        String publicId = (xsltSource instanceof StreamSource) ?
          ((StreamSource) xsltSource).getPublicId () : null;
        byte[] detectBuffer = in.getDetectBuffer ();
        if (detectBuffer == null)
          {
            throw new TransformerException ("No document element");
          }
        this.nativeStylesheetHandle
          = newLibxsltStylesheet (in,
                                  detectBuffer,
                                  publicId,
                                  systemId,
                                  base,
                                  javaContext,
                                  outputProperties);
      }
    catch (IOException e)
      {
        throw new TransformerConfigurationException (e);
      }
    catch (TransformerException e)
      {
        if (null != e.getCause ())
          {
            throw new TransformerConfigurationException (e.getCause ());
          }
        else
          {
            throw new TransformerConfigurationException (e);
          }
      }
  }

  public Object getNativeStylesheetHandle ()
  {
    return this.nativeStylesheetHandle;
  }

  public Properties getOutputProperties ()
  {
    return this.outputProperties;
  }

  public void finalize ()
  {
    if (nativeStylesheetHandle != null)
      {
	freeLibxsltStylesheet (nativeStylesheetHandle);
	nativeStylesheetHandle = null;
      }
  }

  public void transform (Source source, Result result, Map parameters,
			 JavaContext javaContext) throws TransformerException
  {

    // Make an array from the parameters so JNI interface's job is
    // easier
    String[] parameterArray = new String[parameters.size () * 2];
    int index = 0;

    for (Iterator it = parameters.keySet ().iterator (); it.hasNext ();
	 ++index)
      {
	String parameterKey = (String) it.next ();
	String parameterValue = (String) parameters.get (parameterKey);
	parameterArray[index * 2 + 0] = parameterKey;
	parameterArray[index * 2 + 1] =
	  "'" + ((parameterValue != null) ? parameterValue : "") + "'";
      }

    // Transform

    try
      {
        GnomeDocument document = javaContext.parseDocumentCached (source);
        libxsltTransform (nativeStylesheetHandle,
                          document,
                          XMLJ.getOutputStream (result),
                          parameterArray,
                          javaContext);
      }
    catch (IOException e)
      {
        throw new TransformerException (source.getSystemId (), e);
      }
  }

  /*
   *  Native interface to libxslt.
   */
  private static synchronized native Object
  newLibxsltStylesheet (InputStream in,
                        byte[] detectBuffer,
                        String publicId,
                        String systemId,
                        String base,
                        JavaContext javaContext,
                        Properties
                        outputProperties);

  private static synchronized native void 
  freeLibxsltStylesheet (Object handle);

  private static synchronized native void 
  libxsltTransform (Object nativeStylesheetHandle,
                    GnomeDocument document,
                    OutputStream out,
                    String[] parameterArray,
                    JavaContext javaContext)
    throws TransformerException;

  /**
   *  Load libxmlj native code.
   */
  static
  {
    System.loadLibrary ("xmlj");
  }

}
