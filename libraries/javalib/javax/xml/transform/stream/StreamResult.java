/*
 * StreamResult.java
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
package javax.xml.transform.stream;

import java.io.OutputStream;
import java.io.Writer;
import java.io.File;
import java.io.IOException;
import javax.xml.transform.Result;


/**
 * Stream Result
 * @author	Andrew Selkirk, David Brownell
 * @version	1.0
 */
public class StreamResult
  implements Result
{

  //-------------------------------------------------------------
  // Variables --------------------------------------------------
  //-------------------------------------------------------------

  public static final String FEATURE =
    "http://javax.xml.transform.stream.StreamResult/feature";

  private String		systemId	= null;
  private OutputStream	outputStream	= null;
  private Writer		writer		= null;


  //-------------------------------------------------------------
  // Initialization ---------------------------------------------
  //-------------------------------------------------------------

  public StreamResult()
  {
  }

  public StreamResult(OutputStream stream)
  {
    this.outputStream = stream;
  }

  public StreamResult(Writer writer)
  {
    this.writer = writer;
  }

  public StreamResult(String systemID)
  {
    this.systemId = systemID;
  }

  public StreamResult(File file)
  {
    setSystemId (file);
  }


  //-------------------------------------------------------------
  // Methods ----------------------------------------------------
  //-------------------------------------------------------------

  public OutputStream getOutputStream()
  {
    return outputStream;
  }

  public String getSystemId()
  {
    return systemId;
  }

  public Writer getWriter()
  {
    return writer;
  }

  public void setOutputStream(OutputStream stream)
  {
    this.outputStream = stream;
  }

  public void setWriter(Writer writer)
  {
    this.writer = writer;
  }

  public void setSystemId(File file)
  {
    try
      {
        this.systemId = StreamSource.fileToURL (file).toString ();
      }
    catch (IOException e)
      {
        // can't happen
        throw new RuntimeException (e.getMessage ());
      }
  }

  public void setSystemId(String systemID)
  {
    this.systemId = systemID;
  }

}
