/*
 * GnomeNodeIterator.java
 * Copyright (C) 2004 The Free Software Foundation
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
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */
package gnu.xml.libxmlj.dom;

import org.w3c.dom.DOMException;
import org.w3c.dom.Node;
import org.w3c.dom.traversal.NodeFilter;
import org.w3c.dom.traversal.NodeIterator;
import org.w3c.dom.traversal.TreeWalker;

class GnomeNodeIterator
implements NodeIterator, TreeWalker
{

  Node root;
  Node current;
  int whatToShow;
  NodeFilter filter;
  boolean entityReferenceExpansion;
  boolean walk;

  GnomeNodeIterator (Node root, int whatToShow, NodeFilter filter,
                     boolean entityReferenceExpansion, boolean walk)
  {
    if (root == null)
      {
        throw new GnomeDOMException (DOMException.NOT_SUPPORTED_ERR, null);
      }
    this.root = root;
    this.whatToShow = whatToShow;
    this.filter = filter;
    this.entityReferenceExpansion = entityReferenceExpansion;
    this.walk = walk;
    current = root;
  }

  public Node getRoot ()
  {
    return root;
  }

  public int getWhatToShow ()
  {
    return whatToShow;
  }

  public NodeFilter getFilter ()
  {
    return filter;
  }

  public boolean getExpandEntityReferences ()
  {
    return entityReferenceExpansion;
  }

  public Node nextNode ()
    throws DOMException
  {
    if (root == null)
      {
        throw new GnomeDOMException (DOMException.INVALID_STATE_ERR, null);
      }
    Node ret;
    do
      {
        if (current.equals (root))
          {
            ret = root.getFirstChild ();
          }
        else if (walk)
          {
            ret = current.getFirstChild ();
            if (ret == null)
              {
                ret = current.getNextSibling ();
              }
            if (ret == null)
              {
                Node tmp = current;
                ret = tmp.getParentNode ();
                while (!ret.equals (root) && tmp.equals (ret.getLastChild ()))
                  {
                    tmp = ret;
                    ret = tmp.getParentNode ();
                  }
                if (ret.equals (root))
                  {
                    ret = null;
                  }
                else
                  {
                    ret = ret.getNextSibling ();
                  }
              }
          }
        else
          {
            ret = current.getNextSibling ();
          }
      }
    while (!accept (ret));
    current = (ret == null) ? current : ret;
    return ret;
  }

  public Node previousNode ()
    throws DOMException
  {
    if (root == null)
      {
        throw new GnomeDOMException (DOMException.INVALID_STATE_ERR, null);
      }
    Node ret;
    do
      {
        if (current.equals (root))
          {
            ret = current.getLastChild ();
          }
        else if (walk)
          {
            ret = current.getLastChild ();
            if (ret == null)
              {
                ret = current.getPreviousSibling ();
              }
            if (ret == null)
              {
                Node tmp = current;
                ret = tmp.getParentNode ();
                while (!ret.equals (root) && tmp.equals (ret.getFirstChild ()))
                  {
                    tmp = ret;
                    ret = tmp.getParentNode ();
                  }
                if (ret.equals (root))
                  {
                    ret = null;
                  }
                else
                  {
                    ret = ret.getPreviousSibling ();
                  }
              }
          }
        else
          {
            ret = current.getPreviousSibling ();
          }
      }
    while (!accept (ret));
    current = (ret == null) ? current : ret;
    return ret;
  }

  public Node getCurrentNode ()
  {
    return current;
  }

  public void setCurrentNode (Node current)
    throws DOMException
  {
    if (current == null)
      {
        throw new GnomeDOMException (DOMException.NOT_SUPPORTED_ERR, null);
      }
    this.current = current;
  }

  public Node parentNode ()
  {
    Node ret = current.getParentNode ();
    if (!accept (ret))
      {
        ret = null;
      }
    current = (ret == null) ? current : ret;
    return ret;
  }

  public Node firstChild ()
  {
    Node ret = current.getFirstChild ();
    while (!accept (ret))
      {
        ret = ret.getNextSibling ();
      }
    current = (ret == null) ? current : ret;
    return ret;
  }

  public Node lastChild ()
  {
    Node ret = current.getLastChild ();
    while (!accept (ret))
      {
        ret = ret.getPreviousSibling ();
      }
    current = (ret == null) ? current : ret;
    return ret;
  }

  public Node previousSibling ()
  {
    Node ret = current.getPreviousSibling ();
    while (!accept (ret))
      {
        ret = ret.getPreviousSibling ();
      }
    current = (ret == null) ? current : ret;
    return ret;
  }

  public Node nextSibling ()
  {
    Node ret = current.getNextSibling ();
    while (!accept (ret))
      {
        ret = ret.getNextSibling ();
      }
    current = (ret == null) ? current : ret;
    return ret;
  }

  public void detach ()
  {
    root = null;
  }

  boolean accept (Node node)
    {
      if (node == null)
        {
          return true;
        }
      boolean ret;
      switch (node.getNodeType ())
        {
        case Node.ATTRIBUTE_NODE:
          ret = (whatToShow & NodeFilter.SHOW_ATTRIBUTE) != 0;
          break;
        case Node.CDATA_SECTION_NODE:
          ret = (whatToShow & NodeFilter.SHOW_CDATA_SECTION) != 0;
          break;
        case Node.COMMENT_NODE:
          ret = (whatToShow & NodeFilter.SHOW_COMMENT) != 0;
          break;
        case Node.DOCUMENT_NODE:
          ret = (whatToShow & NodeFilter.SHOW_DOCUMENT) != 0;
          break;
        case Node.DOCUMENT_FRAGMENT_NODE:
          ret = (whatToShow & NodeFilter.SHOW_DOCUMENT_FRAGMENT) != 0;
          break;
        case Node.DOCUMENT_TYPE_NODE:
          ret = (whatToShow & NodeFilter.SHOW_DOCUMENT_TYPE) != 0;
          break;
        case Node.ELEMENT_NODE:
          ret = (whatToShow & NodeFilter.SHOW_ELEMENT) != 0;
          break;
        case Node.ENTITY_NODE:
          ret = (whatToShow & NodeFilter.SHOW_ENTITY) != 0;
          break;
        case Node.ENTITY_REFERENCE_NODE:
          ret = (whatToShow & NodeFilter.SHOW_ENTITY_REFERENCE) != 0;
          ret = ret && entityReferenceExpansion;
          break;
        case Node.NOTATION_NODE:
          ret = (whatToShow & NodeFilter.SHOW_NOTATION) != 0;
          break;
        case Node.PROCESSING_INSTRUCTION_NODE:
          ret = (whatToShow & NodeFilter.SHOW_PROCESSING_INSTRUCTION) != 0;
          break;
        case Node.TEXT_NODE:
          ret = (whatToShow & NodeFilter.SHOW_TEXT) != 0;
          break;
        default:
          ret = true;
        }
      if (ret && filter != null)
        {
          ret = (filter.acceptNode (node) == NodeFilter.FILTER_ACCEPT);
        }
      return ret;
    }

}
