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

import java.util.Iterator;

import org.w3c.dom.Attr;
import org.w3c.dom.CDATASection;
import org.w3c.dom.Comment;
import org.w3c.dom.Document;
import org.w3c.dom.DocumentFragment;
import org.w3c.dom.DocumentType;
import org.w3c.dom.DOMConfiguration;
import org.w3c.dom.DOMImplementation;
import org.w3c.dom.DOMException;
import org.w3c.dom.Element;
import org.w3c.dom.Entity;
import org.w3c.dom.EntityReference;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.Notation;
import org.w3c.dom.ProcessingInstruction;
import org.w3c.dom.Text;
import org.w3c.dom.traversal.DocumentTraversal;
import org.w3c.dom.traversal.NodeFilter;
import org.w3c.dom.traversal.NodeIterator;
import org.w3c.dom.traversal.TreeWalker;
import org.w3c.dom.xpath.XPathEvaluator;
import org.w3c.dom.xpath.XPathException;
import org.w3c.dom.xpath.XPathExpression;
import org.w3c.dom.xpath.XPathNSResolver;


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
public class DomDocument
  extends DomNode
  implements Document, DocumentTraversal, XPathEvaluator
{

  private final DOMImplementation implementation;
  private boolean checkingCharacters = true;
  
  // package private
  final static String xmlNamespace = "http://www.w3.org/XML/1998/namespace";
  final static String xmlnsURI = "http://www.w3.org/2000/xmlns/";

  String inputEncoding;
  String encoding;
  String version;
  boolean standalone;
  String systemId;
  
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
  public DomDocument()
  {
    this(new DomImpl());
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
  protected DomDocument(DOMImplementation impl)
  {
	super(DOCUMENT_NODE, null);
    implementation = impl;
  }

  /**
   * <b>DOM L1</b>
   * Returns the constant "#document".
   */
  final public String getNodeName()
  {
    return "#document";
  }

  /**
   * <b>DOM L1</b>
   * Returns the document's root element, or null.
   */
  final public Element getDocumentElement()
  {
	for (DomNode ctx = first; ctx != null; ctx = ctx.next)
      {
        if (ctx.nodeType == ELEMENT_NODE)
          {
            return (Element) ctx;
          }
	}
    return null;
  }

  /**
   * <b>DOM L1</b>
   * Returns the document's DocumentType, or null.
   */
  final public DocumentType getDoctype()
  {
	for (DomNode ctx = first; ctx != null; ctx = ctx.next)
      {
	    if (ctx.nodeType == DOCUMENT_TYPE_NODE)
          {
            return (DocumentType) ctx;
          }
      }
	return null;
  }

  /**
   * <b>DOM L1</b>
   * Returns the document's DOMImplementation.
   */
  final public DOMImplementation getImplementation()
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
  public Element getElementById(String id)
  {
	DomDoctype doctype = (DomDoctype) getDoctype();

	if (doctype == null || !doctype.hasIds()
		|| id == null || id.length() == 0)
      {
	    return null;
      }
	
	// yes, this is linear in size of document.
	// it'd be easy enough to maintain a hashtable.
	Node current = getDocumentElement();
	Node temp;

	if (current == null)
      {
	    return null;
      }
	while (current != this)
      {
	    // done?
	    if (current.getNodeType() == ELEMENT_NODE)
          {
            Element element = (Element) current;
            DomDoctype.ElementInfo	info;

            info = doctype.getElementInfo(current.getNodeName());
            if (id.equals(element.getAttribute(info.getIdAttr())))
              {
                return element;
              }
          }

	    // descend?
	    if (current.hasChildNodes())
          {
            current = current.getFirstChild();
            continue;
          }
	    
	    // lateral?
	    temp = current.getNextSibling();
	    if (temp != null)
          {
            current = temp;
            continue;
          }
	    
	    // back up ... 
	    do
          {
            temp = current.getParentNode();
            if (temp == null)
              {
                return null;
              }
            current = temp;
            temp = current.getNextSibling();
          }
        while (temp == null);
	    current = temp;
	}
	return null;
  }

  private void checkNewChild(Node newChild)
  {
	if (newChild.getNodeType() == ELEMENT_NODE
		&& getDocumentElement() != null)
      {
	    throw new DomEx (DomEx.HIERARCHY_REQUEST_ERR, null, newChild, 0);
      }
	if (newChild.getNodeType() == DOCUMENT_TYPE_NODE
		&& getDoctype() != null)
      {
	    throw new DomEx (DomEx.HIERARCHY_REQUEST_ERR, null, newChild, 0);
      }
  }

  /**
   * <b>DOM L1</b>
   * Appends the specified node to this node's list of children,
   * enforcing the constraints that there be only one root element
   * and one document type child.
   */
  public Node appendChild(Node newChild)
  {
    checkNewChild(newChild);
    return super.appendChild(newChild);
  }

  /**
   * <b>DOM L1</b>
   * Inserts the specified node in this node's list of children,
   * enforcing the constraints that there be only one root element
   * and one document type child.
   */
  public Node insertBefore (Node newChild, Node refChild)
  {
	checkNewChild(newChild);
	return super.insertBefore(newChild, refChild);
  }

  /**
   * <b>DOM L1</b>
   * Replaces the specified node in this node's list of children,
   * enforcing the constraints that there be only one root element
   * and one document type child.
   */
  public Node replaceChild(Node newChild, Node refChild)
  {
	if (!(newChild.getNodeType() == ELEMENT_NODE
		    && refChild.getNodeType() != ELEMENT_NODE)
		&& !(newChild.getNodeType() == DOCUMENT_TYPE_NODE
		    && refChild.getNodeType() != ELEMENT_NODE))
      {
	    checkNewChild(newChild);
      }
	return super.replaceChild(newChild, refChild);
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
  public static void verifyXmlName(String name)
  {
    char c;
    int len = name.length();

	if (len == 0)
      {
	    throw new DomEx (DomEx.NAMESPACE_ERR, name, null, 0);
      }

	// NOTE:  these aren't really the XML rules, but they're
	// a close approximation that's simple to implement.
	c = name.charAt(0);
	if (!Character.isUnicodeIdentifierStart(c)
		&& c != ':' && c != '_')
      {
	    throw new DomEx(DomEx.INVALID_CHARACTER_ERR, name, null, c);
      }
	for (int i = 1; i < len; i++)
      {
	    c = name.charAt(i);
	    if (!Character.isUnicodeIdentifierPart(c)
		     && c != ':'&& c != '_' && c != '.' && c != '-'
             && !isExtender (c))
          {
            throw new DomEx(DomEx.INVALID_CHARACTER_ERR, name, null, c);
          }
      }
  }

  private static boolean isExtender(char c)
  {
	// [88] Extender ::= ...
    return c == 0x00b7 || c == 0x02d0 || c == 0x02d1 || c == 0x0387
      || c == 0x0640 || c == 0x0e46 || c == 0x0ec6 || c == 0x3005
      || (c >= 0x3031 && c <= 0x3035)
      || (c >= 0x309d && c <= 0x309e)
      || (c >= 0x30fc && c <= 0x30fe);
  }

  // package private
  static void verifyNamespaceName(String name)
  {
	int index = name.indexOf(':');

	if (index < 0)
      {
	    verifyXmlName(name);
	    return;
      }
	if (name.lastIndexOf(':') != index)
      {
	    throw new DomEx(DomEx.NAMESPACE_ERR, name, null, 0);
      }
	verifyXmlName(name.substring(0, index));
	verifyXmlName(name.substring(index + 1));
  }

  // package private
  static void verifyXmlCharacters(String value)
  {
	int len = value.length();

	for (int i = 0; i < len; i++)
      {
	    char c = value.charAt(i);

	    // assume surrogate pairing checks out OK, for simplicity
	    if (c >= 0x0020 && c <= 0xFFFD)
          {
            continue;
          }
	    if (c == '\n' || c == '\t' || c == '\r')
          {
            continue;
          }

	    throw new DomEx(DomEx.INVALID_CHARACTER_ERR, value, null, c);
      }
  }

  // package private
  static void verifyXmlCharacters(char[] buf, int off, int len)
  {
	for (int i = 0; i < len; i++)
      {
	    char c = buf[off + i];

	    // assume surrogate pairing checks out OK, for simplicity
	    if (c >= 0x0020 && c <= 0xFFFD)
          {
            continue;
          }
	    if (c == '\n' || c == '\t' || c == '\r')
          {
            continue;
          }

	    throw new DomEx(DomEx.INVALID_CHARACTER_ERR,
                        new String(buf, off, len), null, c);
      }
  }

  /**
   * <b>DOM L1</b>
   * Returns a newly created element with the specified name.
   */
  public Element createElement(String name)
  {
	Element element;

	if (checkingCharacters)
      {
	    verifyXmlName(name);
      }
	if (name.startsWith("xml:"))
      {
	    element = createElementNS(null, name);
      }
	else
      {
	    element = new DomElement(this, null, name);
      }
	defaultAttributes(element, name);
    return element;
  }

  /**
   * <b>DOM L2</b>
   * Returns a newly created element with the specified name
   * and namespace information.
   */
  public Element createElementNS(String namespaceURI, String name)
  {
	if (checkingCharacters)
      {
	    verifyNamespaceName(name);
      }

	if ("".equals(namespaceURI))
      {
	    namespaceURI = null;
      }
	if (name.startsWith("xml:"))
      {
	    if (namespaceURI != null
            && !xmlNamespace.equals(namespaceURI))
          {
            throw new DomEx(DomEx.NAMESPACE_ERR,
                            "xml namespace is always " + xmlNamespace,
                            this, 0);
          }
	    namespaceURI = xmlNamespace;
      }
	else if (name.startsWith("xmlns:"))
      {
	    throw new DomEx(DomEx.NAMESPACE_ERR,
                        "xmlns is reserved", this, 0);
      }
	else if (namespaceURI == null && name.indexOf(':') != -1)
      {
	    throw new DomEx(DomEx.NAMESPACE_ERR,
                        "prefixed name needs a URI", this, 0);
      }

	Element	element = new DomElement(this, namespaceURI, name);
	defaultAttributes(element, name);
	return element;
  }

  private void defaultAttributes(Element element, String name)
  {
	DomDoctype doctype = (DomDoctype) getDoctype();
	DomDoctype.ElementInfo info;

	if (doctype == null)
      {
	    return;
      }

	// default any attributes that need it
	info = doctype.getElementInfo(name);
	for (Iterator i = info.keySet().iterator(); i.hasNext(); )
      {
	    String	attr = (String) i.next();
	    DomAttr	node = (DomAttr) createAttribute(attr);

	    node.setValue((String) info.get(attr));
	    node.setSpecified(false);
	    element.setAttributeNode(node);
      }
  }

  /**
   * <b>DOM L1</b>
   * Returns a newly created document fragment.
   */
  public DocumentFragment createDocumentFragment()
  {
    return new DomFragment(this);
  }

  /**
   * <b>DOM L1</b>
   * Returns a newly created text node with the specified value.
   */
  public Text createTextNode(String value)
  {
	if (checkingCharacters)
      {
	    verifyXmlCharacters(value);
      }
	return new DomText(this, value);
  }

  /**
   * Returns a newly created text node with the specified value.
   */
  public Text createTextNode(char[] buf, int off, int len)
  {
	if (checkingCharacters)
      {
	    verifyXmlCharacters(buf, off, len);
      }
	return new DomText(this, buf, off, len);
  }

  /**
   * <b>DOM L1</b>
   * Returns a newly created comment node with the specified value.
   */
  public Comment createComment(String value)
  {
	if (checkingCharacters)
      {
	    verifyXmlCharacters(value);
      }
	return new DomComment(this, value);
  }

  /**
   * <b>DOM L1</b>
   * Returns a newly created CDATA section node with the specified value.
   */
  public CDATASection createCDATASection(String value)
  {
	if (checkingCharacters)
      {
	    verifyXmlCharacters(value);
      }
	return new DomCDATA(this, value);
  }

  /**
   * Returns a newly created CDATA section node with the specified value.
   */
  public CDATASection createCDATASection(char[] buf, int off, int len)
  {
	if (checkingCharacters)
      {
	    verifyXmlCharacters(buf, off, len);
      }
	return new DomCDATA(this, buf, off, len);
  }

  /**
   * <b>DOM L1</b>
   * Returns a newly created processing instruction.
   */
  public ProcessingInstruction createProcessingInstruction(String target,
                                                           String data)
  {
	if (checkingCharacters)
      {
	    verifyXmlName(target);
	    verifyXmlCharacters(data);
	    if ("xml".equalsIgnoreCase(target))
          {
            throw new DomEx(DomEx.SYNTAX_ERR,
                            "illegal PI target name", this, 0);
          }
      }
	return new DomPI(this, target, data);
  }

  /**
   * <b>DOM L1</b>
   * Returns a newly created attribute with the specified name.
   */
  public Attr createAttribute(String name)
  {
	if (checkingCharacters)
      {
	    verifyXmlName(name);
      }
	if (name.startsWith("xml:") || name.startsWith("xmlns:"))
      {
	    return createAttributeNS(null, name);
      }
	else
      {
	    return new DomAttr(this, null, name);
      }
  }

  /**
   * <b>DOM L2</b>
   * Returns a newly created attribute with the specified name
   * and namespace information.
   */
  public Attr createAttributeNS(String namespaceURI, String name)
  {
	if (checkingCharacters)
      {
	    verifyNamespaceName(name);
      }

	if ("".equals(namespaceURI))
      {
	    namespaceURI = null;
      }
	if (name.startsWith ("xml:"))
      {
	    if (namespaceURI == null)
          {
            namespaceURI = xmlNamespace;
          }
	    else if (!xmlNamespace.equals(namespaceURI))
          {
            throw new DomEx(DomEx.NAMESPACE_ERR,
                            "xml namespace is always " + xmlNamespace,
                            this, 0);
          }
	    namespaceURI = xmlNamespace;
      }
    else if (name.startsWith("xmlns:") || name.equals("xmlns"))
      {
	    if (!xmlnsURI.equals(namespaceURI))
          {
            throw new DomEx(DomEx.NAMESPACE_ERR,
                            "xmlns is reserved", this, 0);
          }
        namespaceURI = xmlnsURI;
      }
    else if (namespaceURI == null && name.indexOf(':') != -1)
      {
	    throw new DomEx(DomEx.NAMESPACE_ERR,
                        "prefixed name needs a URI: " + name, this, 0);
      }
    return new DomAttr(this, namespaceURI, name);
  }

  /**
   * <b>DOM L1</b>
   * Returns a newly created reference to the specified entity.
   * The caller should populate this with the appropriate children
   * and then mark it as readonly.
   *
   * @see DomNode#makeReadonly
   */
  public EntityReference createEntityReference(String name)
  {
	DomEntityReference retval;

	if (checkingCharacters)
      {
	    verifyXmlName(name);
      }
	retval = new DomEntityReference(this, name);
	//
	// If we have such an entity, it's allowed that one arrange that
	// the children of this reference be "the same as" (in an undefined
	// sense of "same", clearly not identity) the children of the entity.
	// That can be immediate or deferred.  It's also allowed that nothing
	// be done -- we take that option here.
	//
	retval.makeReadonly();
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
  public Node importNode(Node copiedNode, boolean deep)
  {
    String name, ns;
	switch (copiedNode.getNodeType())
      {
      case TEXT_NODE:
        return createTextNode(copiedNode.getNodeValue());
      case CDATA_SECTION_NODE:
        return createCDATASection(copiedNode.getNodeValue());
      case COMMENT_NODE:
        return createComment(copiedNode.getNodeValue());
      case PROCESSING_INSTRUCTION_NODE:
        return createProcessingInstruction(copiedNode.getNodeName(),
                                           copiedNode.getNodeValue());
      case NOTATION_NODE:
        // NOTE:  There's no standard way to create
        // these, or add them to a doctype.  Useless.
        Notation notation = (Notation) copiedNode;
        return new DomNotation(this, notation.getNodeName(),
                               notation.getPublicId(),
                               notation.getSystemId());
      case ENTITY_NODE:
        // NOTE:  There's no standard way to create
        // these, or add them to a doctype.  Useless.
        Entity entity = (Entity) copiedNode;
        
        // FIXME if "deep", can/should copy children!
        
        return new DomEntity(this, entity.getNodeName(),
                             entity.getPublicId(),
                             entity.getSystemId(),
                             entity.getNotationName());
      case ENTITY_REFERENCE_NODE:
        return createEntityReference(copiedNode.getNodeName());
      case DOCUMENT_FRAGMENT_NODE:
        DomFragment fragment = new DomFragment(this);
        if (deep)
          {
            for (DomNode ctx = fragment.first; ctx != null; ctx = ctx.next)
              {
                fragment.appendChild(importNode(ctx, deep));
              }
          }
        return fragment;
      case ATTRIBUTE_NODE:
        DomAttr	attr;
        name = copiedNode.getNodeName();
        ns = copiedNode.getNamespaceURI();

        if (ns != null)
          {
			attr = (DomAttr) createAttributeNS(ns, name);
          }
        else
          {
			attr = (DomAttr) createAttribute(name);
          }

        // this is _always_ done regardless of "deep" setting
        for (Node ctx = copiedNode.getFirstChild(); ctx != null;
             ctx = ctx.getNextSibling())
          {
			attr.appendChild(importNode(ctx, false));
          }
        return attr;
      case ELEMENT_NODE:
        DomElement element;
        name = copiedNode.getNodeName();
        ns = copiedNode.getNamespaceURI();
        NamedNodeMap attrs = copiedNode.getAttributes();
        int len = attrs.getLength();
        
        if (ns != null)
          {
            element = (DomElement) createElementNS(ns, name);
          }
        else
          {
            element = (DomElement) createElement(name);
          }
        for (int i = 0; i < len; i++)
          {
            Attr a = (Attr) attrs.item(i);
            Attr dflt;
            
            // maybe update defaulted attributes
            dflt = element.getAttributeNode(a.getNodeName());
            if (dflt != null)
              {
                String newval = a.getNodeValue();
                if (!dflt.getNodeValue().equals(newval)
                    || a.getSpecified () == true)
                  {
                    dflt.setNodeValue (newval);
                  }
                continue;
              }
            
            element.setAttributeNode((Attr) importNode(a, false));
          }
        
        if (!deep)
          {
            return element;
          }
        
        for (Node ctx = copiedNode.getFirstChild(); ctx != null;
             ctx = ctx.getNextSibling())
          {
            element.appendChild(importNode(ctx, true));
          }
        
        return element;
        
        // can't import document or doctype nodes
      case DOCUMENT_NODE:
      case DOCUMENT_TYPE_NODE:
        // FALLTHROUGH
        
        // can't import unrecognized or nonstandard nodes
      default:
        throw new DomEx(DomEx.NOT_SUPPORTED_ERR, null, copiedNode, 0);
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
  public NodeIterator createNodeIterator(Node root,
                                         int whatToShow,
                                         NodeFilter filter,
                                         boolean expandEntities)
  {
	return new DomIterator(root, whatToShow, filter, expandEntities);
  }

  public TreeWalker createTreeWalker(Node root,
                                     int whatToShow,
                                     NodeFilter filter,
                                     boolean entityReferenceExpansion)
  {
    nyi(); // FIXME createTreeWalker
	return null;
  }

  // DOM Level 3 methods
  
  /**
   * DOM L3
   */
  public String getInputEncoding()
  {
    return inputEncoding;
  }

  public void setInputEncoding(String inputEncoding)
  {
    this.inputEncoding = inputEncoding;
  }
  
  /**
   * DOM L3
   */
  public String getXmlEncoding()
  {
    return encoding;
  }
  
  public void setXmlEncoding(String encoding)
  {
    this.encoding = encoding;
  }
  
  public boolean getXmlStandalone()
  {
    return standalone;
  }

  public void setXmlStandalone(boolean xmlStandalone)
  {
    standalone = xmlStandalone;
  }

  public String getXmlVersion()
  {
    return version;
  }

  public void setXmlVersion(String xmlVersion)
  {
    version = xmlVersion;
  }

  public boolean getStrictErrorChecking()
  {
    return checkingCharacters;
  }

  public void setStrictErrorChecking(boolean strictErrorChecking)
  {
    checkingCharacters = strictErrorChecking;
  }

  public String getDocumentURI()
  {
    return systemId;
  }

  public void setDocumentURI(String documentURI)
  {
    systemId = documentURI;
  }

  public Node adoptNode(Node source)
  {
    if (source instanceof DomNode)
      {
        DomNode node = (DomNode) source;
        if (node.parent != null)
          {
            node = (DomNode) node.cloneNode(true);
          }
        node.setOwner(this);
        return node;
      }
    return null;
  }

  public DOMConfiguration getDomConfig()
  {
    // TODO
    nyi();
    return null;
  }

  public void normalizeDocument()
  {
    // TODO
    nyi();
  }
  
  public Node renameNode(Node n, String namespaceURI, String qualifiedName)
  {
    // TODO
    nyi();
    return null;
  }

  // -- XPathEvaluator --
  
  public XPathExpression createExpression(String expression,
                                          XPathNSResolver resolver)
    throws XPathException, DOMException
  {
    return new DomXPathExpression(this, expression, resolver);
  }
  
  public XPathNSResolver createNSResolver(Node nodeResolver)
  {
    return new DomXPathNSResolver(nodeResolver);
  }
    
  public Object evaluate(String expression,
                         Node contextNode,
                         XPathNSResolver resolver,
                         short type,
                         Object result)
    throws XPathException, DOMException
  {
    XPathExpression xpe =
      new DomXPathExpression(this, expression, resolver);
    return xpe.evaluate(contextNode, type, result);
  }

}

