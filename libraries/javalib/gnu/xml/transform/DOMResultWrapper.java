/*
 * DOMResultWrapper.java
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

package gnu.xml.transform;

import java.io.IOException;
import java.io.OutputStream;
import javax.xml.transform.Result;
import javax.xml.transform.dom.DOMResult;
import javax.xml.transform.sax.SAXResult;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Node;

/**
 * A DOM result that wraps an underlying result.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class DOMResultWrapper
  extends DOMResult
{

  final Result result;

  DOMResultWrapper(Result result)
  {
    this.result = result;
  }
  
  public Node getNode()
  {
    if (result instanceof DOMResult)
      {
        return ((DOMResult) result).getNode();
      }
    return null;
  }

  public Node getNextSibling()
  {
    if (result instanceof DOMResult)
      {
        return ((DOMResult) result).getNextSibling();
      }
    return null;
  }

  public String getSystemId()
  {
    return result.getSystemId();
  }

  public void setSystemId(String systemId)
  {
    result.setSystemId(systemId);
  }

  public void setNode(Node node)
  {
    if (result instanceof DOMResult)
      {
        ((DOMResult) result).setNode(node);
      }
    else if (result instanceof StreamResult)
      {
        try
          {
            StreamResult sr = (StreamResult) result;
            OutputStream out = sr.getOutputStream();
            DOMSerializer.serialize(node, out);
            out.close();
          }
        catch (IOException e)
          {
            // TODO
            e.printStackTrace();
          }
      }
    else if (result instanceof SAXResult)
      {
        // TODO
      }
  }

  public void setNextSibling(Node node)
  {
    // Will never be called by transform
  }
  
}
