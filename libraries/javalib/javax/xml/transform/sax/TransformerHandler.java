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
package javax.xml.transform.sax;

import org.xml.sax.ContentHandler;
import org.xml.sax.DTDHandler;
import org.xml.sax.ext.LexicalHandler;
import javax.xml.transform.Result;
import javax.xml.transform.Transformer;


/**
 * Transforms SAX input events to a Result, according to some
 * stylesheet.  Note the expectation that XSLT transforms will
 * discard most DTD declarations (DeclHandler is not supported).
 *
 * @author	Andrew Selkirk, David Brownell
 * @version	1.0
 */
public interface TransformerHandler
    extends ContentHandler, LexicalHandler, DTDHandler
{
    /**
     * Assigns the result of the transform.
     */
    public void setResult(Result result) 
	    throws IllegalArgumentException;

    public void setSystemId(String systemID);

    public String getSystemId();

    /**
     * Returns the associated transformer, for use in setting
     * parameters and output properties.
     */
    public Transformer getTransformer();
}
