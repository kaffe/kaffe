/*
 * Copyright (C) 2001 Andrew Selkirk
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
package javax.xml.transform.dom;

// Imports
import org.w3c.dom.Node;
import javax.xml.transform.Result;

/**
 * DOM Result
 * @author	Andrew Selkirk
 * @version	1.0
 */
public class DOMResult
  implements Result
{

  //-------------------------------------------------------------
  // Variables --------------------------------------------------
  //-------------------------------------------------------------

  public static final String FEATURE =
    "http://javax.xml.transform.dom.DOMResult/feature";

  private Node node;
  private Node nextSibling;
  private String systemId;


  //-------------------------------------------------------------
  // Initialization ---------------------------------------------
  //-------------------------------------------------------------

  public DOMResult()
  {
    this(null, null, null);
  } // DOMResult()

  public DOMResult(Node node)
  {
    this(node, null, null);
  } // DOMResult()

  /**
   * @since 1.3
   */
  public DOMResult(Node node, Node nextSibling)
  {
    this(node, nextSibling, null);
  } // DOMResult()
  
  public DOMResult(Node node, String systemID)
  {
    this(node, null, systemID);
  } // DOMResult()
  
  /**
   * @since 1.3
   */
  public DOMResult(Node node, Node nextSibling, String systemID)
  {
    this.node = node;
    this.nextSibling = nextSibling;
    this.systemId = systemID;
  } // DOMResult()


  //-------------------------------------------------------------
  // Methods ----------------------------------------------------
  //-------------------------------------------------------------

  public void setNode(Node node)
  {
    this.node = node;
  } // setNode()

  public Node getNode()
  {
    return node;
  } // getNode()

  /**
   * @since 1.3
   */
  public void setNextSibling(Node nextSibling)
  {
    this.nextSibling = nextSibling;
  }

  /**
   * @since 1.3
   */
  public Node getNextSibling()
  {
    return nextSibling;
  }

  public void setSystemId(String systemID)
  {
    this.systemId = systemID;
  } // systemID()

  public String getSystemId()
  {
    return systemId;
  } // getSystemId()


} // DOMResult

