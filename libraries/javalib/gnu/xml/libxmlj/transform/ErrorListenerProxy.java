/* 
 * $Id: ErrorListenerProxy.java,v 1.1 2004/04/14 19:40:35 dalibor Exp $
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

import javax.xml.transform.ErrorListener;
import javax.xml.transform.TransformerException;

public class ErrorListenerProxy 
   implements ErrorListener 
{
  private ErrorListener remote;

  public void error (TransformerException exception)
    throws TransformerException
  {
    this.remote.error (exception);
  }

  public void fatalError (TransformerException exception)
    throws TransformerException
  {
    this.remote.fatalError (exception);
  }

  public void warning (TransformerException exception)
    throws TransformerException
  {
    this.remote.warning (exception);
  }

  public void set (ErrorListener remote)
  {
    this.remote = remote;
  }

  public ErrorListener get ()
  {
    return this.remote;
  }
}


