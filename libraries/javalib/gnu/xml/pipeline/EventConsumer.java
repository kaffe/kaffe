/*
 * Copyright (C) 1999-2001 David Brownell
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

package gnu.xml.pipeline;

import org.xml.sax.*;


/**
 * Collects the event consumption apparatus of a SAX pipeline stage.
 * Consumers which permit some handlers or other characteristics to be
 * configured will provide methods to support that configuration.
 *
 * <p> Two important categories of consumers include <em>filters</em>, which
 * process events and pass them on to other consumers, and <em>terminus</em>
 * (or <em>terminal</em>) stages, which don't pass events on.  Filters are not
 * necessarily derived from the {@link EventFilter} class, although that
 * class can substantially simplify their construction by automating the
 * most common activities.
 *
 * <p> Event consumers which follow certain conventions for the signatures
 * of their constructors can be automatically assembled into pipelines
 * by the {@link PipelineFactory} class.
 *
 * @author David Brownell
 */
public interface EventConsumer
{
    /** Most stages process these core SAX callbacks. */
    public ContentHandler getContentHandler ();

    /** Few stages will use unparsed entities. */
    public DTDHandler getDTDHandler ();

    /**
     * This method works like the SAX2 XMLReader method of the same name,
     * and is used to retrieve the optional lexical and declaration handlers
     * in a pipeline.
     *
     * @param id This is a URI identifying the type of property desired.
     * @return The value of that property, if it is defined.
     *
     * @exception SAXNotRecognizedException Thrown if the particular
     *	pipeline stage does not understand the specified identifier.
     */
    public Object getProperty (String id)
    throws SAXNotRecognizedException;

    /**
     * This method provides a filter stage with a handler that abstracts
     * presentation of warnings and both recoverable and fatal errors.
     * Most pipeline stages should share a single policy and mechanism
     * for such reports, since application components require consistency
     * in such activities.  Accordingly, typical responses to this method
     * invocation involve saving the handler for use; filters will pass
     * it on to any other consumers they use.
     *
     * @param handler encapsulates error handling policy for this stage
     */
    public void setErrorHandler (ErrorHandler handler);
}
