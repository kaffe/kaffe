/*
 * SAXSource.java
 * Copyright (C) 2001 Andrew Selkirk
 * Copyright (C) 2001 The Free Software Foundation
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
 * obliged to do so.  If you do not wish to do so, delete this
 * exception statement from your version. 
 */
package javax.xml.transform.sax;

import org.xml.sax.XMLReader;
import org.xml.sax.InputSource;
import javax.xml.transform.Source;
import javax.xml.transform.stream.StreamSource;


/**
 * Acts as a holder for "pull style" inputs to an XSLT transform.
 * SAX based transforms can support a second style of inputs,
 * driving by a {@link TransformerHandler} as output of some
 * other SAX processing pipeline. stage.
 *
 * @see SAXTransformerFactory#newTransformerHandler
 * 
 * @author	Andrew Selkirk, David Brownell
 * @version	1.0
 */
public class SAXSource
  implements Source
{
  /**
   * Used with <em>TransformerFactory.getFeature()</em> to determine
   * whether the transformers it produces support SAXSource objects
   * (possibly without URIs) as inputs.
   */
  public static final String FEATURE =
    "http://javax.xml.transform.sax.SAXSource/feature";

  private XMLReader	reader		= null;
  private InputSource	inputSource	= null;


  //-------------------------------------------------------------
  // Initialization ---------------------------------------------
  //-------------------------------------------------------------

  public SAXSource()
  {
  } // SAXSource()

  public SAXSource(XMLReader reader, InputSource source)
  {
    this.reader = reader;
    this.inputSource = source;
  } // SAXSource()

  public SAXSource(InputSource source)
  {
    this.inputSource = source;
  } // SAXSource()


  //-------------------------------------------------------------
  // Methods ----------------------------------------------------
  //-------------------------------------------------------------

  public void setXMLReader(XMLReader reader)
  {
    this.reader = reader;
  } // setXMLReader()

  public XMLReader getXMLReader()
  {
    return reader;
  } // getXMLReader()

  public void setInputSource(InputSource source)
  {
    this.inputSource = source;
  } // setInputSource()

  public InputSource getInputSource()
  {
    return inputSource;
  } // inputSource()

  public void setSystemId(String systemID)
  {
    if (inputSource != null)
      {
        inputSource.setSystemId(systemID);
      }
  } // setSystemId()

  public String getSystemId()
  {
    if (inputSource != null)
      {
        return inputSource.getSystemId();
      } // if
    return null;
  } // getSystemId()

  /**
   * Creates a SAX input source from its argument.
   * Understands StreamSource and System ID based input sources,
   * and insists on finding either a system ID (URI) or some kind
   * of input stream (character or byte).
   *
   * @param in TRAX style input source
   * @return SAX input source, or null if one could not be
   *	created.
   */
  public static InputSource sourceToInputSource (Source in)
  {
    InputSource	retval;
    boolean	ok = false;

    if (in instanceof SAXSource)
      {
        return ((SAXSource) in).inputSource;
      }

    if (in.getSystemId () != null)
      {
        retval = new InputSource (in.getSystemId ());
        ok = true;
      }
    else
      {
        retval = new InputSource ();
      }

    if (in instanceof StreamSource)
      {
        StreamSource	ss = (StreamSource) in;
        
        if (ss.getReader () != null)
          {
            retval.setCharacterStream (ss.getReader ());
            ok = true;
          }
        else if (ss.getInputStream () != null)
          {
            retval.setByteStream (ss.getInputStream ());
            ok = true;
          }
        if (ss.getPublicId () != null)
          {
            retval.setPublicId (ss.getPublicId ());
          }
      }
    
    return ok ? retval : null;
  }

}
