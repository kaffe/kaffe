/*
 * DomDocument.java
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

import java.util.Enumeration;

import org.w3c.dom.*;
import org.w3c.dom.traversal.*;


/**
 * <p> "Document" and "DocumentTraversal" implementation.
 *
 * <p> Note that when this checks names for legality, it uses an
 * approximation of the XML rules, not the real ones.  Specifically,
 * it uses Unicode rules, with sufficient tweaks to pass a majority
 * of basic XML conformance tests.  (The huge XML character tables are
 * hairy to implement.)
 *
 * @author David Brownell 
 */
public class DomDocument extends DomNode
    implements Document, DocumentTraversal
{
    private final DOMImplementation	implementation;
    private boolean			checkingCharacters = true;

    // package private
    final static String			xmlNamespace =
	"http://www.w3.org/XML/1998/namespace";
    final static String			xmlnsURI =
	"http://www.w3.org/2000/xmlns/";


    /**
     * Constructs a Document node, associating it with an instance
     * of the DomImpl class.
     *
     * <p> Note that this constructor disables character checking.
     * It is normally used when connecting a DOM to an XML parser,
     * and duplicating such checks is undesirable.  When used for
     * purposes other than connecting to a parser, you should
     * re-enable that checking.
     *
     * @see #setCheckingCharacters
     */
    public DomDocument ()
    {
	super (null);
	implementation = new DomImpl ();
    }


    /**
     * Constructs a Document node, associating it with the specified
     * implementation.  This should only be used in conjunction with
     * a specialized implementation; it will normally be called by
     * that implementation.
     *
     * @see DomImpl
     * @see #setCheckingCharacters
     */
    protected DomDocument (DOMImplementation impl)
    {
	super (null);
	implementation = impl;
    }


    /**
     * <b>DOM L1</b>
     * Returns the constant "#document".
     */
    final public String getNodeName ()
    {
	return "#document";
    }

    /**
     * <b>DOM L1</b>
     * Returns the constant DOCUMENT_NODE.
     */
    final public short getNodeType ()
	{ return DOCUMENT_NODE; }


    /**
     * <b>DOM L1</b>
     * Returns the document's root element, or null.
     */
    final public Element getDocumentElement ()
    {
	for (int i = 0; i < getLength (); i++) {
	    Node n = item (i);
	    if (n.getNodeType () == ELEMENT_NODE)
		return (Element) n;
	}
	return null;
    }


    /**
     * <b>DOM L1</b>
     * Returns the document's DocumentType, or null.
     */
    final public DocumentType getDoctype ()
    {
	for (int i = 0; i < getLength (); i++) {
	    Node n = item (i);
	    if (n.getNodeType () == DOCUMENT_TYPE_NODE)
		return (DocumentType) n;
	}
	return null;
    }


    /**
     * <b>DOM L1</b>
     * Returns the document's DOMImplementation.
     */
    final public DOMImplementation getImplementation ()
    {
	return implementation;
    }


    /**
     * <b>DOM L1 (relocated in DOM L2)</b>
     * Returns the element with the specified "ID" attribute, or null.
     *
     * <p>Returns null unless {@link Consumer} was used to populate internal
     * DTD declaration information, using package-private APIs.  If that
     * internal DTD information is available, the document may be searched for
     * the element with that ID.
     */
    public Element getElementById (String id)
    {
	DomDoctype	doctype = (DomDoctype) getDoctype ();

	if (doctype == null || !doctype.hasIds ()
		|| id == null || id.length () == 0)
	    return null;
	
	// yes, this is linear in size of document.
	// it'd be easy enough to maintain a hashtable.
	Node	current = getDocumentElement ();
	Node	temp;

	if (current == null)
	    return null;
	while (current != this) {
	    // done?
	    if (current.getNodeType () == ELEMENT_NODE) {
		Element			element = (Element) current;
		DomDoctype.ElementInfo	info;

		info = doctype.getElementInfo (current.getNodeName ());
		if (id.equals (element.getAttribute (info.getIdAttr ())))
		    return element;
	    }

	    // descend?
	    if (current.hasChildNodes ()) {
		current = current.getFirstChild ();
		continue;
	    }
	    
	    // lateral?
	    temp = current.getNextSibling ();
	    if (temp != null) {
		current = temp;
		continue;
	    }
	    
	    // back up ... 
	    do {
		temp = current.getParentNode ();
		if (temp == null)
		    return null;
		current = temp;
		temp = current.getNextSibling ();
	    } while (temp == null);
	    current = temp;
	}
	return null;
    }


    private void checkNewChild (Node newChild)
    {
	if (newChild.getNodeType () == ELEMENT_NODE
		&& getDocumentElement () != null)
	    throw new DomEx (DomEx.HIERARCHY_REQUEST_ERR, null, newChild, 0);
	if (newChild.getNodeType () == DOCUMENT_TYPE_NODE
		&& getDoctype () != null)
	    throw new DomEx (DomEx.HIERARCHY_REQUEST_ERR, null, newChild, 0);
    }


    /**
     * <b>DOM L1</b>
     * Appends the specified node to this node's list of children,
     * enforcing the constraints that there be only one root element
     * and one document type child.
     */
    public Node appendChild (Node newChild)
    {
	checkNewChild (newChild);
	return super.appendChild (newChild);
    }


    /**
     * <b>DOM L1</b>
     * Inserts the specified node in this node's list of children,
     * enforcing the constraints that there be only one root element
     * and one document type child.
     */
    public Node insertBefore (Node newChild, Node refChild)
    {
	checkNewChild (newChild);
	return super.insertBefore (newChild, refChild);
    }


    /**
     * <b>DOM L1</b>
     * Replaces the specified node in this node's list of children,
     * enforcing the constraints that there be only one root element
     * and one document type child.
     */
    public Node replaceChild (Node newChild, Node refChild)
    {
	if (!(newChild.getNodeType () == ELEMENT_NODE
		    && refChild.getNodeType () != ELEMENT_NODE)
		&& !(newChild.getNodeType () == DOCUMENT_TYPE_NODE
		    && refChild.getNodeType () != ELEMENT_NODE))
	    checkNewChild (newChild);
	return super.replaceChild (newChild, refChild);
    }

    
    // NOTE:  DOM can't really tell when the name of an entity,
    // notation, or PI must follow the namespace rules (excluding
    // colons) instead of the XML rules (which allow them without
    // much restriction).  That's an API issue.  verifyXmlName
    // aims to enforce the XML rules, not the namespace rules.

    /**
     * Throws a DOM exception if the specified name is not a legal XML 1.0
     * name.  Actually this uses a very similar set of rules, closer to
     * Unicode rules than to the rules encoded in the large table at the
     * end of the XML 1.0 specification.
     *
     * @exception DomException INVALID_CHARACTER_ERR if the name isn't
     *	legal as an XML name.
     */
    static public void verifyXmlName (String name)
    {
	char c;
	int len = name.length ();

	if (len == 0)
	    throw new DomEx (DomEx.NAMESPACE_ERR, name, null, 0);

	// NOTE:  these aren't really the XML rules, but they're
	// a close approximation that's simple to implement.
	c = name.charAt (0);
	if (!Character.isUnicodeIdentifierStart (c)
		&& c != ':' && c != '_')
	    throw new DomEx (DomEx.INVALID_CHARACTER_ERR,
		name, null, c);
	for (int i = 1; i < len; i++) {
	    c = name.charAt (i);
	    if (!Character.isUnicodeIdentifierPart (c)
		     && c != ':'&& c != '_' && c != '.' && c != '-'
		    && !isExtender (c))
		throw new DomEx (DomEx.INVALID_CHARACTER_ERR,
		    name, null, c);
	}
    }

    static private boolean isExtender (char c)
    {
	// [88] Extender ::= ...
	return c == 0x00b7 || c == 0x02d0 || c == 0x02d1 || c == 0x0387
		|| c == 0x0640 || c == 0x0e46 || c == 0x0ec6 || c == 0x3005
		|| (c >= 0x3031 && c <= 0x3035)
		|| (c >= 0x309d && c <= 0x309e)
		|| (c >= 0x30fc && c <= 0x30fe);
    }

    // package private
    static void verifyNamespaceName (String name)
    {
	int index = name.indexOf (':');

	if (index < 0) {
	    verifyXmlName (name);
	    return;
	}
	if (name.lastIndexOf (':') != index)
	    throw new DomEx (DomEx.NAMESPACE_ERR, name, null, 0);
	verifyXmlName (name.substring (0, index));
	verifyXmlName (name.substring (index + 1));
    }

    // package private
    static void verifyXmlCharacters (String value)
    {
	int len = value.length ();

	for (int i = 0; i < len; i++) {
	    char c = value.charAt (i);

	    // assume surrogate pairing checks out OK, for simplicity
	    if (c >= 0x0020 && c <= 0xFFFD)
		continue;
	    if (c == '\n' || c == '\t' || c == '\r')
		continue;

	    throw new DomEx (DomEx.INVALID_CHARACTER_ERR, value, null, c);
	}
    }

    // package private
    static void verifyXmlCharacters (char buf [], int off, int len)
    {
	for (int i = 0; i < len; i++) {
	    char c = buf [off + i];

	    // assume surrogate pairing checks out OK, for simplicity
	    if (c >= 0x0020 && c <= 0xFFFD)
		continue;
	    if (c == '\n' || c == '\t' || c == '\r')
		continue;

	    throw new DomEx (DomEx.INVALID_CHARACTER_ERR,
		new String (buf, off, len), null, c);
	}
    }


    /**
     * Controls whether certain expensive checks, duplicating those that
     * conformant XML parsers must perform, are made.
     */
    final public void setCheckingCharacters (boolean value)
    {
	checkingCharacters = value;
    }


    /**
     * Returns true if certain expensive checks are performed.
     * Those checks are intended to reject illegal names, and characters
     * that are illegal as XML characters.
     */
    final public boolean isCheckingCharacters ()
    {
	return checkingCharacters;
    }


    /**
     * <b>DOM L1</b>
     * Returns a newly created element with the specified name.
     */
    public Element createElement (String name)
    {
	Element		element;

	if (checkingCharacters)
	    verifyXmlName (name);
	if (name.startsWith ("xml:"))
	    element = createElementNS (null, name);
	else
	    element = new DomElement (this, null, name);
	defaultAttributes (element, name);
	return element;
    }


    /**
     * <b>DOM L2</b>
     * Returns a newly created element with the specified name
     * and namespace information.
     */
    public Element createElementNS (String namespaceURI, String name)
    {
	if (checkingCharacters)
	    verifyNamespaceName (name);

	if ("".equals (namespaceURI))
	    namespaceURI = null;
	if (name.startsWith ("xml:")) {
	    if (namespaceURI != null
		    && !xmlNamespace.equals (namespaceURI))
		throw new DomEx (DomEx.NAMESPACE_ERR,
		    "xml namespace is always " + xmlNamespace, this, 0);
	    namespaceURI = xmlNamespace;
	} else if (name.startsWith ("xmlns:"))
	    throw new DomEx (DomEx.NAMESPACE_ERR,
		"xmlns is reserved", this, 0);
	else if (namespaceURI == null && name.indexOf (':') != -1)
	    throw new DomEx (DomEx.NAMESPACE_ERR,
		"prefixed name needs a URI", this, 0);

	Element	element = new DomElement (this, namespaceURI, name);
	defaultAttributes (element, name);
	return element;
    }

    private void defaultAttributes (Element element, String name)
    {
	DomDoctype		doctype = (DomDoctype) getDoctype ();
	DomDoctype.ElementInfo	info;

	if (doctype == null)
	    return;

	// default any attributes that need it
	info = doctype.getElementInfo (name);
	for (Enumeration e = info.keys (); e.hasMoreElements (); /* NOP */) {
	    String	attr = (String) e.nextElement ();
	    DomAttr	node = (DomAttr) createAttribute (attr);

	    node.setValue ((String) info.get (attr));
	    node.setSpecified (false);
	    element.setAttributeNode (node);
	}
    }


    /**
     * <b>DOM L1</b>
     * Returns a newly created document fragment.
     */
    public DocumentFragment createDocumentFragment ()
    {
	return new DomFragment (this);
    }


    /**
     * <b>DOM L1</b>
     * Returns a newly created text node with the specified value.
     */
    public Text createTextNode (String value)
    {
	if (checkingCharacters)
	    verifyXmlCharacters (value);
	return new DomText (this, value);
    }

    /**
     * Returns a newly created text node with the specified value.
     */
    public Text createTextNode (char buf [], int off, int len)
    {
	if (checkingCharacters)
	    verifyXmlCharacters (buf, off, len);
	return new DomText (this, buf, off, len);
    }


    /**
     * <b>DOM L1</b>
     * Returns a newly created comment node with the specified value.
     */
    public Comment createComment (String value)
    {
	if (checkingCharacters)
	    verifyXmlCharacters (value);
	return new DomComment (this, value);
    }


    /**
     * <b>DOM L1</b>
     * Returns a newly created CDATA section node with the specified value.
     */
    public CDATASection createCDATASection (String value)
    {
	if (checkingCharacters)
	    verifyXmlCharacters (value);
	return new DomCDATA (this, value);
    }


    /**
     * Returns a newly created CDATA section node with the specified value.
     */
    public CDATASection createCDATASection (char buf [], int off, int len)
    {
	if (checkingCharacters)
	    verifyXmlCharacters (buf, off, len);
	return new DomCDATA (this, buf, off, len);
    }


    /**
     * <b>DOM L1</b>
     * Returns a newly created processing instruction.
     */
    public ProcessingInstruction createProcessingInstruction (
	String target,
	String data
    )
    {
	if (checkingCharacters) {
	    verifyXmlName (target);
	    verifyXmlCharacters (data);
	    if ("xml".equalsIgnoreCase (target))
		throw new DomEx (DomEx.SYNTAX_ERR,
		    "illegal PI target name", this, 0);
	}
	return new DomPI (this, target, data);
    }


    /**
     * <b>DOM L1</b>
     * Returns a newly created attribute with the specified name.
     */
    public Attr createAttribute (String name)
    {
	if (checkingCharacters)
	    verifyXmlName (name);
	if (name.startsWith ("xml:") || name.startsWith ("xmlns:"))
	    return createAttributeNS (null, name);
	else
	    return new DomAttr (this, null, name);
    }


    /**
     * <b>DOM L2</b>
     * Returns a newly created attribute with the specified name
     * and namespace information.
     */
    public Attr createAttributeNS (String namespaceURI, String name)
    {
	if (checkingCharacters)
	    verifyNamespaceName (name);

	if ("".equals (namespaceURI))
	    namespaceURI = null;
	if (name.startsWith ("xml:")) {
	    if (namespaceURI == null)
		namespaceURI = xmlNamespace;
	    else if (!xmlNamespace.equals (namespaceURI))
		throw new DomEx (DomEx.NAMESPACE_ERR,
		    "xml namespace is always " + xmlNamespace, this, 0);
	    namespaceURI = xmlNamespace;

	} else if (name.startsWith ("xmlns:") || name.equals ("xmlns")) {
	    if (!xmlnsURI.equals (namespaceURI))
		throw new DomEx (DomEx.NAMESPACE_ERR,
		    "xmlns is reserved", this, 0);
	    namespaceURI = xmlnsURI;

	} else if (namespaceURI == null && name.indexOf (':') != -1)
	    throw new DomEx (DomEx.NAMESPACE_ERR,
		"prefixed name needs a URI: " + name, this, 0);
	
	return new DomAttr (this, namespaceURI, name);
    }


    /**
     * <b>DOM L1</b>
     * Returns a newly created reference to the specified entity.
     * The caller should populate this with the appropriate children
     * and then mark it as readonly.
     *
     * @see DomNode#makeReadonly
     */
    public EntityReference createEntityReference (String name)
    {
	DomEntityReference retval;

	if (checkingCharacters)
	    verifyXmlName (name);
	retval = new DomEntityReference (this, name);
	//
	// If we have such an entity, it's allowed that one arrange that
	// the children of this reference be "the same as" (in an undefined
	// sense of "same", clearly not identity) the children of the entity.
	// That can be immediate or deferred.  It's also allowed that nothing
	// be done -- we take that option here.
	//
	retval.makeReadonly ();
	return retval;
    }


    /**
     * <b>DOM L2</b>
     * Makes a copy of the specified node, with all nodes "owned" by
     * this document and with children optionally copied.  This type
     * of standard utility has become, well, a standard utility.
     *
     * <p> Note that EntityReference nodes created through this method (either
     * directly, or recursively) never have children, and that there is no
     * portable way to associate them with such children.
     *
     * <p> Note also that there is no requirement that the specified node
     * be associated with a different document.  This differs from the
     * <em>cloneNode</em> operation in that the node itself is not given
     * an opportunity to participate, so that any information managed
     * by node subclasses will be lost.
     */
    public Node importNode (Node copiedNode, boolean deep)
    {
	switch (copiedNode.getNodeType ()) {
	    case TEXT_NODE:
		return createTextNode (copiedNode.getNodeValue ());
	    case CDATA_SECTION_NODE:
		return createCDATASection (copiedNode.getNodeValue ());
	    case COMMENT_NODE:
		return createComment (copiedNode.getNodeValue ());
	    case PROCESSING_INSTRUCTION_NODE:
		return createProcessingInstruction (
		    copiedNode.getNodeName (),
		    copiedNode.getNodeValue ());
	    case NOTATION_NODE:
		{
		    // NOTE:  There's no standard way to create
		    // these, or add them to a doctype.  Useless.
		    Notation node = (Notation) copiedNode;
		    return new DomNotation (this, node.getNodeName (),
			    node.getPublicId (), node.getSystemId ());
		}
	    case ENTITY_NODE:
		{
		    // NOTE:  There's no standard way to create
		    // these, or add them to a doctype.  Useless.
		    Entity node = (Entity) copiedNode;

			// FIXME if "deep", can/should copy children!

		    return new DomEntity (this, node.getNodeName (),
			    node.getPublicId (), node.getSystemId (),
			    node.getNotationName ());
		}
	    case ENTITY_REFERENCE_NODE:
		return createEntityReference (copiedNode.getNodeName ());
	    case DOCUMENT_FRAGMENT_NODE:
		{
		    DocumentFragment node = createDocumentFragment ();
		    if (deep) {
			NodeList kids = copiedNode.getChildNodes ();
			int len = kids.getLength ();
			for (int i = 0; i < len; i++)
			    node.appendChild (
				importNode (kids.item (i), deep));
		    }
		    return node;
		}
	    case ATTRIBUTE_NODE:
		{
		    DomAttr	retval;
		    String	name = copiedNode.getNodeName ();
		    String	ns = copiedNode.getNamespaceURI ();
		    NodeList	kids = copiedNode.getChildNodes ();
		    int		len = kids.getLength ();

		    if (ns != null)
			retval = (DomAttr) createAttributeNS (ns, name);
		    else
			retval = (DomAttr) createAttribute (name);

		    // this is _always_ done regardless of "deep" setting
		    for (int i = 0; i < len; i++)
			retval.appendChild (importNode (kids.item (i), false));
		    return retval;
		}
	    case ELEMENT_NODE:
		{
		    DomElement	retval;
		    String	name = copiedNode.getNodeName ();
		    String	ns = copiedNode.getNamespaceURI ();
		    NamedNodeMap attrs = copiedNode.getAttributes ();
		    int		len = attrs.getLength ();

		    if (ns != null)
			retval = (DomElement) createElementNS (ns, name);
		    else
			retval = (DomElement) createElement (name);
		    for (int i = 0; i < len; i++) {
			Attr	attr = (Attr) attrs.item (i);
			Attr	dflt;

			// maybe update defaulted attributes
			dflt = retval.getAttributeNode (attr.getNodeName ());
			if (dflt != null) {
			    String	newval = attr.getNodeValue ();
			    if (!dflt.getNodeValue ().equals (newval)
				    || attr.getSpecified () == true)
				dflt.setNodeValue (newval);
			    continue;
			}

			retval.setAttributeNode ((Attr)
				    importNode (attr, false));
		    }

		    if (!deep)
			return retval;

		    NodeList	kids = copiedNode.getChildNodes ();

		    len = kids.getLength ();
		    for (int i = 0; i < len; i++)
			retval.appendChild (importNode (kids.item (i), true));
		    
		    return retval;
		}

	    // can't import document or doctype nodes
	    case DOCUMENT_NODE:
	    case DOCUMENT_TYPE_NODE:
		// FALLTHROUGH

	    // can't import unrecognized or nonstandard nodes
	    default:
		throw new DomEx (DomEx.NOT_SUPPORTED_ERR, null, copiedNode, 0);
	}

	// FIXME cleanup a bit -- for deep copies, copy those
	// children in one place, here (code sharing is healthy)
    }


    /**
     * <b>DOM L2 (Traversal)</b>
     * Returns a newly created node iterator.  Don't forget to detach
     * this iterator when you're done using it!
     *
     * @see DomIterator
     */
    public NodeIterator createNodeIterator (
	Node		root,
	int		whatToShow,
	NodeFilter	filter,
	boolean		expandEntities
    )
    {
	return new DomIterator (root, whatToShow, filter, expandEntities);
    }



    public TreeWalker createTreeWalker (
	Node		root,
	int		whatToShow,
	NodeFilter	filter,
	boolean		entityReferenceExpansion
    )
    {
nyi (); // FIXME createTreeWalker
	return null;
    }

    // DOM Level 3 methods

    public String getInputEncoding ()
      {
        // TODO
        return null;
      }

    public String getXmlEncoding ()
      {
        // TODO
        return null;
      }

    public boolean getXmlStandalone ()
      {
        // TODO
        return false;
      }

    public void setXmlStandalone (boolean xmlStandalone)
      {
        // TODO
      }

    public String getXmlVersion ()
      {
        // TODO
        return null;
      }

    public void setXmlVersion (String xmlVersion)
      {
        // TODO
      }

    public boolean getStrictErrorChecking ()
      {
        // TODO
        return false;
      }

    public void setStrictErrorChecking (boolean strictErrorChecking)
      {
        // TODO
      }

    public String getDocumentURI ()
      {
        // TODO
        return null;
      }

    public void setDocumentURI (String documentURI)
      {
        // TODO
      }

    public Node adoptNode (Node source)
      {
        // TODO
        return null;
      }

    public DOMConfiguration getDomConfig ()
      {
        // TODO
        return null;
      }

    public void normalizeDocument ()
      {
        // TODO
      }

    public Node renameNode (Node n, String namespaceURI, String qualifiedName)
      {
        // TODO
        return null;
      }

}
