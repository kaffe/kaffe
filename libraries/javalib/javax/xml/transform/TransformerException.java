/*
 * TransformerException.java
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
package javax.xml.transform;

// Imports
import java.io.*;

/**
 * Encapsulates a problem exposed during a transformation.
 * @author	Andrew Selkirk, David Brownell
 * @version	1.0
 */
public class TransformerException
  extends Exception
{

  //-------------------------------------------------------------
  // Variables --------------------------------------------------
  //-------------------------------------------------------------

  private	SourceLocator	locator			= null;
  private Throwable	containedException	= null;
  private boolean		causeKnown;


  //-------------------------------------------------------------
  // Initialization ---------------------------------------------
  //-------------------------------------------------------------

  public TransformerException(String msg)
  {
    super(msg);
  }

  public TransformerException(Throwable cause)
  {
    super();
    initCause (cause);
  }

  public TransformerException(String msg, Throwable cause)
  {
    super(msg);
    initCause (cause);
  }

  public TransformerException(String msg, SourceLocator locator)
  {
    super(msg);
    setLocator (locator);
  }

  public TransformerException(String msg, SourceLocator locator, 
                              Throwable cause)
  {
    super(msg);
    setLocator (locator);
    initCause (cause);
  }


  //-------------------------------------------------------------
  // Methods ----------------------------------------------------
  //-------------------------------------------------------------

  /**
   * Returns the root cause of this exception,
   * or null if none is known.
   */
  public Throwable getCause()
  {
    return containedException;
  }

  /**
   * Synonym for {@link #getCause}.
   */
  public Throwable getException()
  {
    return containedException;
  }

  /**
   * Returns a readable version of the locator info, or null
   * if there is no locator.
   */
  public String getLocationAsString()
  {
    if (locator == null)
      {
        return null;
      }

    StringBuffer	retval = new StringBuffer ();

    if (locator.getPublicId () != null)
    {
      retval.append ("public='");
      retval.append (locator.getPublicId ());
      retval.append ("' ");
    }
    if (locator.getSystemId () != null)
    {
      retval.append ("uri='");
      retval.append (locator.getSystemId ());
      retval.append ("' ");
    }
    if (locator.getLineNumber () != -1)
    {
      retval.append ("line=");
      retval.append (locator.getLineNumber ());
      retval.append (" ");
    }
    if (locator.getColumnNumber () != -1)
    {
      retval.append ("column=");
      retval.append (locator.getColumnNumber ());
      //retval.append (" ");
    }
    return retval.toString ();
  }

  public SourceLocator getLocator()
  {
    return locator;
  }

  /**
   * Returns this exception's message, with readable location
   * information appended if it is available.
   */
  public String getMessageAndLocation()
  {
    if (locator == null)
      {
        return getMessage ();
      }
    return getMessage () + ": " + getLocationAsString ();
  }

  /**
   * Records the root cause of this exception; may be
   * called only once, normally during initialization.
   */
  public synchronized Throwable initCause(Throwable cause)
  {
    if (cause == this)
      {
        throw new IllegalArgumentException ();
      }
    if (containedException != null)
      {
        throw new IllegalStateException ();
      }
    containedException = cause;
    causeKnown = true;
    // FIXME: spec implies "this" may be the right value; another bug?
    return cause;
  }

  public void printStackTrace()
  {
    printStackTrace(System.out);// shouldn't it be System.err?
  }

  public void printStackTrace(PrintStream stream)
  {
    PrintWriter out = new PrintWriter(
                                      new OutputStreamWriter(stream));
    printStackTrace(out);
    out.flush ();
  }

  public void printStackTrace(PrintWriter writer)
  {
    if (containedException != null)
    {
      containedException.printStackTrace(writer);
    }
    super.printStackTrace(writer);
  }

  public void setLocator (SourceLocator location)
  {
    locator = location;
  }

}
