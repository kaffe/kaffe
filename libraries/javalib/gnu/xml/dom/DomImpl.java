/*
 * DomImpl.java
 * Copyright (C) 1999,2000,2001 The Free Software Foundation
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

package gnu.xml.dom;

import java.util.Vector;

import org.w3c.dom.*;


/**
 * <p> "DOMImplementation" implementation. </p>
 *
 * <p> At this writing, the following features are supported: "XML" (L1, L2),
 * "Events" (L2), "MutationEvents" (L2), "USER-Events" (a conformant extension),
 * "HTMLEvents" (L2), "UIEvents" (L2), "Traversal" (L2).  It is possible to
 * compile the package so it doesn't support some of these features (notably,
 * Traversal).
 *
 * @author David Brownell 
 */
public class DomImpl implements DOMImplementation
{
    /**
     * Constructs a DOMImplementation object which supports
     * "XML" and other DOM Level 2 features.
     */
    public DomImpl () {}

    /**
     * <b>DOM L1</b>
     * Returns true if the specified feature and version are
     * supported.  Note that the case of the feature name is ignored.
     */
    public boolean hasFeature (String name, String version)
    {
	name = name.toLowerCase ();

	if ("xml".equals (name) || "core".equals (name)) {
	    if (version == null || version.equals (""))
		return true;
	    if ("1.0".equals (version))
		return true;
	    if ("2.0".equals (version))
		return true;

	} else if ("events".equals (name)
		|| "mutationevents".equals (name)
		|| "uievents".equals (name)
		// || "mouseevents".equals (name)
		|| "htmlevents".equals (name)
		) {
	    if (version == null || version.equals (""))
		return true;
	    if ("2.0".equals (version))
		return true;
	
	// Extension:  "USER-" prefix event types can
	// be created and passed through the DOM.

	} else if ("user-events".equals (name)) {
	    if (version == null || version.equals (""))
		return true;
	    if ("0.1".equals (version))
		return true;

	// NOTE:  "hasFeature" for events is here interpreted to
	// mean the DOM can manufacture those sorts of events,
	// since actually choosing to report the events is more
	// often part of the environment or application.  It's
	// only really an issue for mutation events.

	} else if (DomNode.reportMutations
		    && "traversal".equals (name)) {

		// Note:  nyet tree walker support ...

		if (version == null || version.equals (""))
		    return true;
		if ("2.0".equals (version))
		    return true;
	} else if ("xpath".equals (name)) {
          if (version == null || version.equals (""))
            return true;
          if ("3.0".equals (version))
            return true;
        }

	// views
	// stylesheets
	// css, css2
	// range

	return false;
    }


    /**
     * <b>DOM L2</b>
     * Creates and returns a DocumentType, associated with this
     * implementation.  This DocumentType can have no associated
     * objects (notations, entities) until the DocumentType is
     * first associated with a document.
     *
     * <p> Note that there is no implication that this DTD will
     * be parsed by the DOM, or ever have contents.  Moreover, the
     * DocumentType created here can only be added to a document by
     * the createDocument method (below).  <em>That means that the only
     * portable way to create a Document object is to start parsing,
     * queue comment and processing instruction (PI) nodes, and then only
     * create a DOM Document after <b>(a)</b> it's known if a DocumentType
     * object is needed, and <b>(b) the name and namespace of the root
     * element is known.  Queued comment and PI nodes would then be
     * inserted appropriately in the document prologue, both before and
     * after the DTD node, and additional attributes assigned to the
     * root element.</em>
     * (One hopes that the final DOM REC fixes this serious botch.)
     */
    public DocumentType createDocumentType (
	String rootName,
	String publicId,
	String systemId
	// CR2 deleted internal subset, ensuring DocumentType
	// is 100% useless instead of just 90% so.
    ) {
	DomDocument.verifyNamespaceName (rootName);
	return new DomDoctype (this, rootName, publicId, systemId, null);
    }


    /**
     * <b>DOM L2</b>
     * Creates and returns a Document, populated only with a root element and
     * optionally a document type (if that was provided).
     */
    public Document createDocument (
	String namespaceURI,
	String rootName,
	DocumentType doctype
    ) {
	Document	doc = new DomDocument (this);
	Element		root;
	
	root = doc.createElementNS (namespaceURI, rootName);
	if (rootName.startsWith ("xmlns:"))
	    throw new DomEx (DomEx.NAMESPACE_ERR,
		"xmlns is reserved", null, 0);
	// Bleech -- L2 seemingly _requires_ omission of xmlns attributes.
	if (doctype != null)
	    doc.appendChild (doctype);		// handles WRONG_DOCUMENT error
	doc.appendChild (root);
	return doc;
    }

    // DOM Level 3

    public Object getFeature (String feature, String version)
      {
        // TODO
        return null;
      }
}
