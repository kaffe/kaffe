/*
 * TransformerImpl.java
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

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Properties;
import javax.xml.transform.ErrorListener;
import javax.xml.transform.Result;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.URIResolver;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.dom.DOMResult;
import javax.xml.transform.sax.SAXResult;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Document;
import org.w3c.dom.DocumentType;
import org.w3c.dom.DOMImplementation;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.ext.LexicalHandler;
import gnu.xml.xpath.Expr;
import gnu.xml.xpath.Root;

class TransformerImpl
  extends Transformer
{

	final TransformerFactoryImpl factory;
  final Stylesheet stylesheet;
  URIResolver uriResolver;
  ErrorListener errorListener;
  Properties outputProperties;

  TransformerImpl(TransformerFactoryImpl factory,
                  Stylesheet stylesheet)
    throws TransformerConfigurationException
  {
    this.factory = factory;
    uriResolver = factory.userResolver;
    errorListener = factory.userListener;
    this.stylesheet = stylesheet;
    if (stylesheet != null)
      {
        // Set up parameter context for this transformer
        stylesheet.bindings.push(false);
      }
  }

  public void transform(Source xmlSource, Result outputTarget)
    throws TransformerException
  {
    DOMSource source;
    synchronized (factory.resolver)
      {
        factory.resolver.setUserResolver(uriResolver);
        factory.resolver.setUserListener(errorListener);
        source = factory.resolver.resolveDOM(xmlSource, null, null);
      }
    Node context = source.getNode();
    Node parent = null, nextSibling = null;
    if (outputTarget instanceof DOMResult)
      {
        DOMResult dr = (DOMResult) outputTarget;
        parent = dr.getNode();
        nextSibling = dr.getNextSibling();
      }
    boolean created = false;
    if (parent == null)
      {
        Document doc = (context instanceof Document) ? (Document) context :
          context.getOwnerDocument();
        parent = doc.createDocumentFragment();
        created = true;
      }
    int outputMethod = Stylesheet.OUTPUT_XML;
    String encoding = null;
    if (stylesheet != null)
      {
        // XSLT transformation
        stylesheet.applyTemplates(new Root(), null,
                                  context, 1, 1,
                                  parent, nextSibling);
        outputMethod = stylesheet.outputMethod;
        encoding = stylesheet.outputEncoding;
        // TODO stylesheet.outputIndent
        String publicId = stylesheet.outputPublicId;
        if (publicId.length() == 0)
          {
            publicId = null;
          }
        String systemId = stylesheet.outputSystemId;
        if (systemId.length() == 0)
          {
            systemId = null;
          }
        
        if (created)
          {
            Node root = parent.getFirstChild();
            while (root != null && root.getNodeType() != Node.ELEMENT_NODE)
              {
                root = root.getNextSibling();
              }
            if (root != null)
              {
                // Now that we know the name of the root element we can create
                // the document
                Document doc = (context instanceof Document) ?
                  (Document) context :
                  context.getOwnerDocument();
                DOMImplementation impl = doc.getImplementation();
                DocumentType doctype = (publicId != null || systemId != null) ?
                  impl.createDocumentType(root.getNodeName(),
                                          publicId,
                                          systemId) :
                  null;
                Document newDoc = impl.createDocument(root.getNamespaceURI(),
                                                      root.getNodeName(),
                                                      doctype);
                Node newRoot = newDoc.getDocumentElement();
                copyAttributes(newDoc, root, newRoot);
                copyChildren(newDoc, root, newRoot);
                parent = newDoc;
              }
          }
      }
    else
      {
        // Identity transform
        Node clone = context.cloneNode(true);
        if (nextSibling != null)
          {
            parent.insertBefore(clone, nextSibling);
          }
        else
          {
            parent.appendChild(clone);
          }
      }
    if (outputTarget instanceof DOMResult)
      {
        if (created)
          {
            ((DOMResult) outputTarget).setNode(parent);
          }
      }
    else if (outputTarget instanceof StreamResult)
      {
        StreamResult sr = (StreamResult) outputTarget;
        try
          {
            OutputStream out = sr.getOutputStream();
            if (out == null)
              {
                String systemId = sr.getSystemId();
                try
                  {
                    URL url = new URL(systemId);
                    URLConnection connection = url.openConnection();
                    connection.setDoOutput(true);
                    out = connection.getOutputStream();
                  }
                catch (MalformedURLException e)
                  {
                    out = new FileOutputStream(systemId);
                  }
              }
            StreamSerializer serializer = new StreamSerializer(encoding);
            serializer.serialize(parent, out, outputMethod);
            out.close();
          }
        catch (IOException e)
          {
            if (errorListener != null)
              {
                errorListener.error(new TransformerException(e));
              }
            else
              {
                e.printStackTrace(System.err);
              }
          }
      }
    else if (outputTarget instanceof SAXResult)
      {
        SAXResult sr = (SAXResult) outputTarget;
        try
          {
            ContentHandler ch = sr.getHandler();
            LexicalHandler lh = sr.getLexicalHandler();
            if (lh == null && ch instanceof LexicalHandler)
              {
                lh = (LexicalHandler) ch;
              }
            SAXSerializer serializer = new SAXSerializer();
            serializer.serialize(parent, ch, lh);
          }
        catch (SAXException e)
          {
            if (errorListener != null)
              {
                errorListener.error(new TransformerException(e));
              }
            else
              {
                e.printStackTrace(System.err);
              }
          }
      }
  }

  void copyAttributes(Document dstDoc, Node src, Node dst)
  {
    NamedNodeMap srcAttrs = src.getAttributes();
    NamedNodeMap dstAttrs = dst.getAttributes();
    if (srcAttrs != null && dstAttrs != null)
      {
        int len = srcAttrs.getLength();
        for (int i = 0; i < len; i++)
          {
            Node node = srcAttrs.item(i);
            node = dstDoc.adoptNode(node);
            dstAttrs.setNamedItemNS(node);
          }
      }
  }

  void copyChildren(Document dstDoc, Node src, Node dst)
  {
    Node srcChild = src.getFirstChild();
    while (srcChild != null)
      {
        Node dstChild = dstDoc.adoptNode(srcChild);
        dst.appendChild(dstChild);
        srcChild = srcChild.getNextSibling();
      }
  }

  public void setParameter(String name, Object value)
  {
    if (stylesheet != null)
      {
        stylesheet.bindings.set(name, value, false);
      }
  }

  public Object getParameter(String name)
  {
    if (stylesheet != null)
      {
        return stylesheet.bindings.get(name, null);
      }
    return null;
  }

  public void clearParameters()
  {
    if (stylesheet != null)
      {
        stylesheet.bindings.pop(false);
        stylesheet.bindings.push(false);
      }
  }

  public void setURIResolver(URIResolver resolver)
  {
    uriResolver = resolver;
  }

  public URIResolver getURIResolver()
  {
    return uriResolver;
  }

  public void setOutputProperties(Properties oformat)
    throws IllegalArgumentException
  {
    outputProperties.putAll(oformat);
  }

  public Properties getOutputProperties()
  {
    return (Properties) outputProperties.clone();
  }

  public void setOutputProperty(String name, String value)
    throws IllegalArgumentException
  {
    outputProperties.put(name, value);
  }

  public String getOutputProperty(String name)
    throws IllegalArgumentException
  {
    return outputProperties.getProperty(name);
  }

  public void setErrorListener(ErrorListener listener)
  {
    errorListener = listener;
  }

  public ErrorListener getErrorListener()
  {
    return errorListener;
  }

}
