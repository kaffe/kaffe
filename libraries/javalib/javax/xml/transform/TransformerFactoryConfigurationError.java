/*
 * Copyright (C) 2001 Andrew Selkirk
 * Copyright (C) 2001 David Brownell
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
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License. 
 */
package javax.xml.transform;

/**
 * Reports a {@link TransformerFactory} configuration error.
 * @author	Andrew Selkirk, David Brownell
 * @version	1.0
 */
public class TransformerFactoryConfigurationError extends Error
{
    private Exception	exception	= null;


    //-------------------------------------------------------------
    // Initialization ---------------------------------------------
    //-------------------------------------------------------------

    public TransformerFactoryConfigurationError ()
	{ }

    public TransformerFactoryConfigurationError (Exception e)
	{ exception = e; }

    public TransformerFactoryConfigurationError (Exception e, String msg)
    {
	super (msg);
	exception = e;
    }

    public TransformerFactoryConfigurationError (String msg)
	{ super (msg); }


    //-------------------------------------------------------------
    // Methods ----------------------------------------------------
    //-------------------------------------------------------------

    public String getMessage ()
    {
	if (super.getMessage () == null && exception != null)
	    return exception.getMessage ();
	return super.getMessage ();
    }

    public Exception getException ()
	{ return exception; }
}
