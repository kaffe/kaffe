/* 
 * $Id: LibxsltStylesheet.java,v 1.1 2004/04/14 19:40:35 dalibor Exp $
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

import java.io.OutputStream;
import java.io.PushbackInputStream;

import java.util.Iterator;
import java.util.Map;
import java.util.Properties;

import javax.xml.transform.Result;
import javax.xml.transform.Source;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.stream.StreamSource;

public class LibxsltStylesheet
{
  private long nativeStylesheetHandle;
  private Properties outputProperties;
  private static int unfinalized = 0;

  LibxsltStylesheet (Source xsltSource, JavaContext javaContext)
    throws TransformerConfigurationException
  {
    try
      {
        this.outputProperties = new Properties ();
        this.nativeStylesheetHandle
          = newLibxsltStylesheet (IOToolkit.getSourceInputStream (xsltSource),
                                  xsltSource.getSystemId (),
                                  (xsltSource instanceof StreamSource) ?
                                  ((StreamSource) xsltSource).
                                  getPublicId () : null, javaContext,
                                  this.outputProperties);
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

  public long getNativeStylesheetHandle ()
  {
    return this.nativeStylesheetHandle;
  }

  public Properties getOutputProperties ()
  {
    return this.outputProperties;
  }

  public void finalize ()
  {
    if (0 != nativeStylesheetHandle)
      {
	freeLibxsltStylesheet (nativeStylesheetHandle);
	nativeStylesheetHandle = 0;
      }
  }

  public void transform (Source source, Result result, Map parameters,
			 JavaContext javaContext) throws TransformerException
  {

    // Make an array from the parameters so JNI interface's job is
    // easier
    String[]parameterArray = new String[parameters.size () * 2];
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

    libxsltTransform (nativeStylesheetHandle,
		      IOToolkit.getSourceInputStream (source),
		      source.getSystemId (),
		      (source instanceof StreamSource) ? ((StreamSource)
							  source).
		      getPublicId () : null,
		      IOToolkit.getResultOutputStream (result),
		      parameterArray, javaContext);
  }

  /*
   *  Native interface to libxslt.
   */
  private static synchronized native long 
  newLibxsltStylesheet (PushbackInputStream in,
                        String inSystemId,
                        String inPublicId,
                        JavaContext javaContext,
                        Properties
                        outputProperties);

  private static synchronized native void 
  freeLibxsltStylesheet (long handle);

  private static synchronized native void 
  libxsltTransform (long nativeStylesheetHandle,
                    PushbackInputStream in,
                    String inSystemId,
                    String inPublicId,
                    OutputStream out,
                    String[]parameterArray,
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
