/* XMLParser.java -- 
   Copyright (C) 2005  Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version.

Partly derived from code which carried the following notice:

  Copyright (c) 1997, 1998 by Microstar Software Ltd.

  AElfred is free for both commercial and non-commercial use and
  redistribution, provided that Microstar's copyright and disclaimer are
  retained intact.  You are free to modify AElfred for your own use and
  to redistribute AElfred with your modifications, provided that the
  modifications are clearly documented.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  merchantability or fitness for a particular purpose.  Please use it AT
  YOUR OWN RISK.
*/

package gnu.xml.stream;

import java.io.BufferedInputStream;
import java.io.EOFException;
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Map;
import java.util.NoSuchElementException;

import javax.xml.XMLConstants;
import javax.xml.namespace.NamespaceContext;
import javax.xml.namespace.QName;
import javax.xml.stream.Location;
import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLReporter;
import javax.xml.stream.XMLResolver;
import javax.xml.stream.XMLStreamConstants;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;

/**
 * An XML parser.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class XMLParser
  implements XMLStreamReader, NamespaceContext, Location
{

  private static final int INIT = 0;
  private static final int PROLOG = 1;
  private static final int CONTENT = 2;
  private static final int EMPTY_ELEMENT = 3;
  private static final int MISC = 4;

  private final static int LIT_ENTITY_REF = 2;
  private final static int LIT_NORMALIZE = 4;
  private final static int LIT_ATTRIBUTE = 8;
  private final static int LIT_DISABLE_PE = 16;
  private final static int LIT_DISABLE_CREF = 32;
  private final static int LIT_DISABLE_EREF = 64;
  private final static int LIT_PUBID = 256;

  final static int ATTRIBUTE_DEFAULT_UNDECLARED = 30;
  final static int ATTRIBUTE_DEFAULT_SPECIFIED = 31;
  final static int ATTRIBUTE_DEFAULT_IMPLIED = 32;
  final static int ATTRIBUTE_DEFAULT_REQUIRED = 33;
  final static int ATTRIBUTE_DEFAULT_FIXED = 34;

  private Input input;
  private LinkedList inputStack = new LinkedList();
  
  private int state = INIT;
  private int event;
  private boolean lookahead;
  private LinkedList stack = new LinkedList();
  private LinkedList namespaces = new LinkedList();
  private ArrayList attrs = new ArrayList();
  private StringBuffer buf = new StringBuffer();
  private StringBuffer nmtokenBuf = new StringBuffer();
  private char[] tmpBuf = new char[1024];

  private String piTarget, piData;

  private String xmlVersion;
  private String xmlEncoding;
  private Boolean xmlStandalone;
  private boolean xml11;

  Doctype doctype;
  private boolean expandPE, peIsError;

  private boolean stringInterning = true;
  private boolean coalescing = false;
  private boolean replaceERefs = true;
  private boolean externalEntities = false;
  private boolean supportDTD = true;
  private boolean namespaceAware = true;
  private boolean xIncludeAware = true;

  private XMLReporter reporter;
  private XMLResolver resolver;

  private static final String TEST_START_ELEMENT = "<";
  private static final String TEST_END_ELEMENT = "</";
  private static final String TEST_COMMENT = "<!--";
  private static final String TEST_PI = "<?";
  private static final String TEST_REFERENCE = "&";
  private static final String TEST_CDATA = "<![CDATA[";
  private static final String TEST_XML_DECL = "<?xml ";
  private static final String TEST_DOCTYPE_DECL = "<!DOCTYPE";
  private static final String TEST_ELEMENT_DECL = "<!ELEMENT";
  private static final String TEST_ATTLIST_DECL = "<!ATTLIST";
  private static final String TEST_ENTITY_DECL = "<!ENTITY";
  private static final String TEST_NOTATION_DECL = "<!NOTATION";
  private static final String TEST_KET = ">";
  private static final String TEST_END_COMMENT = "--";
  private static final String TEST_END_PI = "?>";
  private static final String TEST_END_CDATA = "]]>";

  public XMLParser(InputStream in, String systemId)
  {
    pushInput(new Input(in, null, systemId));
  }

  public XMLParser(Reader reader, String systemId)
  {
    pushInput(new Input(null, reader, null, systemId));
  }

  public void setStringInterning(boolean stringInterning)
  {
    this.stringInterning = stringInterning;
  }

  public void setValidating(boolean validating)
  {
    // TODO
  }

  public void setNamespaceAware(boolean namespaceAware)
  {
    this.namespaceAware = namespaceAware;
  }

  public void setCoalescing(boolean coalescing)
  {
    this.coalescing = coalescing;
  }

  public void setReplacingEntityReferences(boolean flag)
  {
    replaceERefs = flag;
  }

  public void setExternalEntities(boolean flag)
  {
    externalEntities = flag;
  }

  public void setSupportDTD(boolean flag)
  {
    supportDTD = flag;
  }

  public void setXIncludeAware(boolean flag)
  {
    xIncludeAware = flag;
  }

  public void setReporter(XMLReporter reporter)
  {
    this.reporter = reporter;
  }

  public void setResolver(XMLResolver resolver)
  {
    this.resolver = resolver;
  }

  // -- NamespaceContext --

  public String getNamespaceURI(String prefix)
  {
    for (Iterator i = namespaces.iterator(); i.hasNext(); )
      {
        LinkedHashMap ctx = (LinkedHashMap) i.next();
        String namespaceURI = (String) ctx.get(prefix);
        if (namespaceURI != null)
          return namespaceURI;
      }
    return null;
  }

  public String getPrefix(String namespaceURI)
  {
    for (Iterator i = namespaces.iterator(); i.hasNext(); )
      {
        LinkedHashMap ctx = (LinkedHashMap) i.next();
        if (ctx.containsValue(namespaceURI))
          {
            for (Iterator j = ctx.entrySet().iterator(); j.hasNext(); )
              {
                Map.Entry entry = (Map.Entry) i.next();
                String uri = (String) entry.getValue();
                if (uri.equals(namespaceURI))
                  return (String) entry.getKey();
              }
          }
      }
    return null;
  }

  public Iterator getPrefixes(String namespaceURI)
  {
    LinkedList acc = new LinkedList();
    for (Iterator i = namespaces.iterator(); i.hasNext(); )
      {
        LinkedHashMap ctx = (LinkedHashMap) i.next();
        if (ctx.containsValue(namespaceURI))
          {
            for (Iterator j = ctx.entrySet().iterator(); j.hasNext(); )
              {
                Map.Entry entry = (Map.Entry) i.next();
                String uri = (String) entry.getValue();
                if (uri.equals(namespaceURI))
                  acc.add(entry.getKey());
              }
          }
      }
    return acc.iterator();
  }

  // -- Location --

  public int getCharacterOffset()
  {
    return input.offset;
  }

  public int getColumnNumber()
  {
    return input.column;
  }

  public int getLineNumber()
  {
    return input.line;
  }

  public String getLocationURI()
  {
    return input.systemId;
  }

  // -- XMLStreamReader --

  public void close()
    throws XMLStreamException
  {
    stack = null;
    namespaces = null;
    buf = null;
    attrs = null;
    doctype = null;
  }

  public NamespaceContext getNamespaceContext()
  {
    return this;
  }

  public int getAttributeCount()
  {
    return attrs.size();
  }

  public String getAttributeName(int index)
  {
    Attribute a = (Attribute) attrs.get(index);
    return a.localName;
  }

  public String getAttributeNamespace(int index)
  {
    String prefix = getAttributePrefix(index);
    return getNamespaceURI(prefix);
  }

  public String getAttributePrefix(int index)
  {
    Attribute a = (Attribute) attrs.get(index);
    return a.prefix;
  }

  public QName getAttributeQName(int index)
  {
    Attribute a = (Attribute) attrs.get(index);
    String namespaceURI = getNamespaceURI(a.prefix);
    return new QName(namespaceURI, a.localName, a.prefix);
  }

  public String getAttributeType(int index)
  {
    Attribute a = (Attribute) attrs.get(index);
    return a.type;
  }

  private String getAttributeType(String elementName, String attName)
  {
    if (doctype != null)
      {
        LinkedHashMap attlist =
          (LinkedHashMap) doctype.attlists.get(elementName);
        if (attlist != null)
          {
            AttributeDecl att = (AttributeDecl) attlist.get(attName);
            if (att != null)
              return att.type;
          }
      }
    return null;
  }

  public String getAttributeValue(int index)
  {
    Attribute a = (Attribute) attrs.get(index);
    return a.value;
  }

  public String getAttributeValue(String namespaceURI, String localName)
  {
    for (Iterator i = attrs.iterator(); i.hasNext(); )
      {
        Attribute a = (Attribute) i.next();
        if (a.localName.equals(localName))
          {
            String uri = getNamespaceURI(a.prefix);
            if ((uri == null && namespaceURI == null) ||
                (uri != null && uri.equals(namespaceURI)))
              return a.value;
          }
      }
    return null;
  }

  boolean isAttributeDeclared(int index)
  {
    if (doctype == null)
      return false;
    Attribute a = (Attribute) attrs.get(index);
    String qn = ("".equals(a.prefix)) ? a.localName :
      a.prefix + ":" + a.localName;
    String elementName = buf.toString();
    LinkedHashMap attlist = (LinkedHashMap) doctype.attlists.get(elementName);
    if (attlist == null)
      return false;
    return attlist.containsKey(qn);
  }
  
  public String getCharacterEncodingScheme()
  {
    return xmlEncoding;
  }

  public String getElementText()
    throws XMLStreamException
  {
    if (event != XMLStreamConstants.START_ELEMENT)
      throw new XMLStreamException("current event must be START_ELEMENT");
    StringBuffer elementText = new StringBuffer();
    int depth = stack.size();
    while (event != XMLStreamConstants.END_ELEMENT || stack.size() > depth)
      {
        switch (next())
          {
          case XMLStreamConstants.CHARACTERS:
          case XMLStreamConstants.SPACE:
            elementText.append(buf.toString());
          }
      }
    return elementText.toString();
  }

  public String getEncoding()
  {
    return (input.forceReader) ? null : input.inputEncoding;
  }

  public int getEventType()
  {
    return event;
  }

  public String getLocalName()
  {
    switch (event)
      {
      case XMLStreamConstants.START_ELEMENT:
      case XMLStreamConstants.END_ELEMENT:
        String qName = buf.toString();
        int ci = qName.indexOf(':');
        return (ci == -1) ? qName : qName.substring(ci + 1);
      default:
        return null;
      }
  }

  public Location getLocation()
  {
    return this;
  }

  public QName getName()
  {
    switch (event)
      {
      case XMLStreamConstants.START_ELEMENT:
      case XMLStreamConstants.END_ELEMENT:
        String qName = buf.toString();
        int ci = qName.indexOf(':');
        String localName = (ci == -1) ? qName : qName.substring(ci + 1);
        String prefix = (ci == -1) ?
          (namespaceAware ? XMLConstants.DEFAULT_NS_PREFIX : null) :
          qName.substring(0, ci);
        String namespaceURI = getNamespaceURI(prefix);
        return new QName(namespaceURI, localName, prefix);
      default:
        return null;
      }
  }

  public int getNamespaceCount()
  {
    if (!namespaceAware)
      return 0;
    switch (event)
      {
      case XMLStreamConstants.START_ELEMENT:
      case XMLStreamConstants.END_ELEMENT:
        LinkedHashMap ctx = (LinkedHashMap) namespaces.getFirst();
        return ctx.size();
      default:
        return 0;
      }
  }

  public String getNamespacePrefix(int index)
  {
    LinkedHashMap ctx = (LinkedHashMap) namespaces.getFirst();
    int count = 0;
    for (Iterator i = ctx.keySet().iterator(); i.hasNext(); )
      {
        String prefix = (String) i.next();
        if (count++ == index)
          return prefix;
      }
    return null;
  }

  public String getNamespaceURI()
  {
    switch (event)
      {
      case XMLStreamConstants.START_ELEMENT:
      case XMLStreamConstants.END_ELEMENT:
        String qName = buf.toString();
        int ci = qName.indexOf(':');
        if (ci == -1)
          return null;
        String prefix = qName.substring(ci + 1);
        return getNamespaceURI(prefix);
      default:
        return null;
      }
  }

  public String getNamespaceURI(int index)
  {
    LinkedHashMap ctx = (LinkedHashMap) namespaces.getFirst();
    int count = 0;
    for (Iterator i = ctx.values().iterator(); i.hasNext(); )
      {
        String uri = (String) i.next();
        if (count++ == index)
          return uri;
      }
    return null;
  }

  public String getPIData()
  {
    return piData;
  }

  public String getPITarget()
  {
    return piTarget;
  }

  public String getPrefix()
  {
    switch (event)
      {
      case XMLStreamConstants.START_ELEMENT:
      case XMLStreamConstants.END_ELEMENT:
        String qName = buf.toString();
        int ci = qName.indexOf(':');
        return (ci == -1) ?
          (namespaceAware ? XMLConstants.DEFAULT_NS_PREFIX : null) :
          qName.substring(0, ci);
      default:
        return null;
      }
  }

  public Object getProperty(String name)
    throws IllegalArgumentException
  {
    if (name == null)
      throw new IllegalArgumentException("name is null");
    if (XMLInputFactory.ALLOCATOR.equals(name))
      return null;
    if (XMLInputFactory.IS_COALESCING.equals(name))
      return coalescing ? Boolean.TRUE : Boolean.FALSE;
    if (XMLInputFactory.IS_NAMESPACE_AWARE.equals(name))
      return namespaceAware ? Boolean.TRUE : Boolean.FALSE;
    if (XMLInputFactory.IS_REPLACING_ENTITY_REFERENCES.equals(name))
      return replaceERefs ? Boolean.TRUE : Boolean.FALSE;
    if (XMLInputFactory.IS_SUPPORTING_EXTERNAL_ENTITIES.equals(name))
      return externalEntities ? Boolean.TRUE : Boolean.FALSE;
    if (XMLInputFactory.IS_VALIDATING.equals(name))
      return Boolean.FALSE;
    if (XMLInputFactory.REPORTER.equals(name))
      return reporter;
    if (XMLInputFactory.RESOLVER.equals(name))
      return resolver;
    if (XMLInputFactory.SUPPORT_DTD.equals(name))
      return supportDTD ? Boolean.TRUE : Boolean.FALSE;
    // TODO stringInterning
    // TODO xIncludeAware
    return null;
  }

  public String getText()
  {
    return buf.toString();
  }

  public char[] getTextCharacters()
  {
    return buf.toString().toCharArray();
  }

  public int getTextCharacters(int sourceStart, char[] target,
                               int targetStart, int length)
    throws XMLStreamException
  {
    length = Math.min(sourceStart + buf.length(), length);
    int sourceEnd = sourceStart + length;
    buf.getChars(sourceStart, sourceEnd, target, targetStart);
    return length;
  }

  public int getTextLength()
  {
    return buf.length();
  }

  public int getTextStart()
  {
    return 0;
  }

  public String getVersion()
  {
    return xmlVersion;
  }

  public boolean hasName()
  {
    switch (event)
      {
      case XMLStreamConstants.START_ELEMENT:
      case XMLStreamConstants.END_ELEMENT:
        return true;
      default:
        return false;
      }
  }

  public boolean hasText()
  {
    switch (event)
      {
      case XMLStreamConstants.CHARACTERS:
      case XMLStreamConstants.SPACE:
        return true;
      default:
        return false;
      }
  }

  public boolean isAttributeSpecified(int index)
  {
    Attribute a = (Attribute) attrs.get(index);
    return a.specified;
  }

  public boolean isCharacters()
  {
    return (event == XMLStreamConstants.CHARACTERS);
  }

  public boolean isEndElement()
  {
    return (event == XMLStreamConstants.END_ELEMENT);
  }

  public boolean isStandalone()
  {
    return Boolean.TRUE.equals(xmlStandalone);
  }

  public boolean isStartElement()
  {
    return (event == XMLStreamConstants.START_ELEMENT);
  }

  public boolean isWhiteSpace()
  {
    return (event == XMLStreamConstants.SPACE);
  }

  public int nextTag()
    throws XMLStreamException
  {
    do
      {
        switch (next())
          {
          case XMLStreamConstants.START_ELEMENT:
          case XMLStreamConstants.END_ELEMENT:
          case XMLStreamConstants.CHARACTERS:
          case XMLStreamConstants.SPACE:
          case XMLStreamConstants.COMMENT:
          case XMLStreamConstants.PROCESSING_INSTRUCTION:
            break;
          default:
            throw new XMLStreamException("Unexpected event type: " + event);
          }
      }
    while (event != XMLStreamConstants.START_ELEMENT &&
           event != XMLStreamConstants.END_ELEMENT);
    return event;
  }

  public void require(int type, String namespaceURI, String localName)
    throws XMLStreamException
  {
    if (event != type)
      throw new XMLStreamException("Current event type is " + event);
    if (event == XMLStreamConstants.START_ELEMENT ||
        event == XMLStreamConstants.END_ELEMENT)
      {
        String ln = getLocalName();
        if (!ln.equals(localName))
          throw new XMLStreamException("Current local-name is " + ln);
        String uri = getNamespaceURI();
        if ((uri == null && namespaceURI != null) ||
            (uri != null && !uri.equals(namespaceURI)))
          throw new XMLStreamException("Current namespace URI is " + uri);
      }
  }

  public boolean standaloneSet()
  {
    return (xmlStandalone != null);
  }

  public boolean hasNext()
    throws XMLStreamException
  {
    if (!lookahead)
      {
        next();
        lookahead = true;
      }
    return event != -1;
  }
  
  public int next()
    throws XMLStreamException
  {
    if (lookahead)
      {
        lookahead = false;
        return event;
      }
    try
      {
        switch (state)
          {
          case CONTENT:
            if (tryRead(TEST_END_ELEMENT))
              {
                readEndElement();
                if (stack.isEmpty())
                  state = MISC;
                event = XMLStreamConstants.END_ELEMENT;
              }
            else if (tryRead(TEST_COMMENT))
              {
                readComment();
                event = XMLStreamConstants.COMMENT;
              }
            else if (tryRead(TEST_PI))
              {
                readPI();
                event = XMLStreamConstants.PROCESSING_INSTRUCTION;
              }
            else if (tryRead(TEST_START_ELEMENT))
              {
                state = readStartElement();
                event = XMLStreamConstants.START_ELEMENT;
              }
            else if (tryRead(TEST_REFERENCE))
              {
                readReference();
                event = XMLStreamConstants.ENTITY_REFERENCE;
              }
            else if (tryRead(TEST_CDATA))
              {
                readCDSect();
                event = XMLStreamConstants.CDATA;
              }
            else
              event = readCharData();
            break;
          case EMPTY_ELEMENT:
            state = stack.isEmpty() ? MISC : CONTENT;
            event = XMLStreamConstants.END_ELEMENT;
            // Pop namespace context
            namespaces.removeFirst();
            break;
          case INIT: // XMLDecl?
            input.init();
            if (tryRead(TEST_XML_DECL))
              readXMLDecl();
            event = XMLStreamConstants.START_DOCUMENT;
            state = PROLOG;
            break;
          case PROLOG: // Misc* (doctypedecl Misc*)?
            skipWhitespace();
            if (doctype == null && tryRead(TEST_DOCTYPE_DECL))
              {
                readDoctypeDecl();
                event = XMLStreamConstants.DTD;
              }
            else if (tryRead(TEST_START_ELEMENT))
              {
                state = readStartElement();
                event = XMLStreamConstants.START_ELEMENT;
              }
            else if (tryRead(TEST_COMMENT))
              {
                readComment();
                event = XMLStreamConstants.COMMENT;
              }
            else if (tryRead(TEST_PI))
              {
                readPI();
                event = XMLStreamConstants.PROCESSING_INSTRUCTION;
              }
            else
              error("no root element");
            break;
          case MISC: // Comment | PI | S
            skipWhitespace();
            if (tryRead(TEST_COMMENT))
              {
                readComment();
                event = XMLStreamConstants.COMMENT;
              }
            else if (tryRead(TEST_PI))
              {
                readPI();
                event = XMLStreamConstants.PROCESSING_INSTRUCTION;
              }
            else
              {
                if (event == XMLStreamConstants.END_DOCUMENT)
                  throw new NoSuchElementException();
                event = XMLStreamConstants.END_DOCUMENT;
              }
            break;
          default:
            event = -1;
          }
        return event;
      }
    catch (IOException e)
      {
        XMLStreamException e2 = new XMLStreamException();
        e2.initCause(e);
        throw e2;
      }
  }

  // private

  private void mark(int limit)
    throws IOException
  {
    input.mark(limit);
  }

  private void reset()
    throws IOException
  {
    input.reset();
    //System.out.print("\n(reset):");
  }

  private int read()
    throws IOException
  {
    int ret = input.read();
    //System.out.print((char) ret);
    return ret;
  }

  private int read(char[] b, int off, int len)
    throws IOException
  {
    int ret = input.read(b, off, len);
    //System.out.print(new String(b, off, len));
    return ret;
  }
  
  /**
   * Parsed character read.
   */
  private char readCh()
    throws IOException, XMLStreamException
  {
    int c = read();
    if (c == -1)
      {
        if (inputStack.size() > 1)
          {
            popInput();
            return readCh();
          }
        return '\uFFFF';
      }
    char ret = (char) c;
    if (expandPE && c == '%')
      {
        if (peIsError)
          error("PE reference within decl in internal subset.");
        parsePEReference();
        return readCh();
      }
    return ret;
  }

  private void require(char delim)
    throws IOException, XMLStreamException
  {
    mark(1);
    char c = readCh();
    if (delim != c)
      {
        reset();
        error("required character (got U+"+Integer.toHexString(c)+")", new Character(delim));
      }
  }

  private void require(String delim)
    throws IOException, XMLStreamException
  {
    char[] chars = delim.toCharArray();
    int len = chars.length;
    mark(len);
    int l2 = read(tmpBuf, 0, len);
    if (l2 != len)
      {
        reset();
        error("EOF before required string", delim);
      }
    else
      {
        for (int i = 0; i < chars.length; i++)
          {
            if (chars[i] != tmpBuf[i])
              {
                reset();
                error("required string", delim);
              }
          }
      }
  }

  /**
   * Try to read a single character.
   */
  private boolean tryRead(char delim)
    throws IOException, XMLStreamException
  {
    mark(1);
    char c = readCh();
    if (delim != c)
      {
        reset();
        return false;
      }
    return true;
  }

  /**
   * Tries to read the specified characters.
   * If successful, the stream is positioned after the last character,
   * otherwise it is reset.
   */
  private boolean tryRead(String test)
    throws IOException
  {
    char[] chars = test.toCharArray();
    int len = chars.length;
    mark(len);
    int l2 = read(tmpBuf, 0, len);
    if (l2 < len)
      {
        reset();
        return false;
      }
    for (int i = 0; i < len; i++)
      {
        if (chars[i] != tmpBuf[i])
          {
            reset();
            return false;
          }
      }
    return true;
  }

  private void readUntil(String delim)
    throws IOException, XMLStreamException
  {
    int startLine = input.line;
    try
      {
        while (!tryRead(delim))
          {
            char c = readCh();
            buf.append(c);
          }
      }
    catch (EOFException e)
      {
        error("end of input while looking for delimiter "+
              "(started on line " + startLine + ')', delim);
      }
  }

  private boolean tryWhitespace()
    throws IOException, XMLStreamException
  {
    boolean white;
    boolean ret = false;
    do
      {
        mark(1);
        char c = readCh();
        white = (c == ' ' || c == '\t' || c == '\n' || c == '\r');
        if (white)
          ret = true;
        else if (c == -1)
          throw new EOFException();
      }
    while (white);
    reset();
    return ret;
  }

  /**
   * Skip over any whitespace characters.
   */
  private void skipWhitespace()
    throws IOException, XMLStreamException
  {
    boolean white;
    do
      {
        mark(1);
        char c = readCh();
        white = (c == ' ' || c == '\t' || c == '\n' || c == '\r');
      }
    while (white);
    reset();
  }

  private void requireWhitespace()
    throws IOException, XMLStreamException
  {
    if (!tryWhitespace())
      error("whitespace required");
  }

  /**
   * Push the specified text input source.
   */
  private void pushInput(String text)
    throws IOException, XMLStreamException
  {
    pushInput(new Input(null, new StringReader(text), null, null,
                        input.inputEncoding));
  }

  /**
   * Push the specified external input source.
   */
  private void pushInput(ExternalIds ids)
    throws IOException, XMLStreamException
  {
    if (!externalEntities)
      error("parser is configured not to support external entities");
    InputStream in = null;
    String url = absolutize(input.systemId, ids.systemId);
    // TODO try to resolve via public ID
    if (in == null && url != null && resolver != null)
      in = resolver.resolve(url);
    if (in == null)
      in = resolve(url);
    if (in == null)
      error("unable to resolve external entity",
            (ids.systemId != null) ? ids.systemId : ids.publicId);
    pushInput(new Input(in, ids.publicId, url, input.inputEncoding));
  }

  private void pushInput(Input input)
  {
    inputStack.addLast(input);
    this.input = input;
    //System.out.print("\n(+input:"+input.systemId+")"); 
  }

  static String absolutize(String base, String href)
  {
    if (href == null)
      return null;
    if (base == null)
      base = "";
    else
      {
        int i = base.lastIndexOf('/');
        if (i != -1)
          base = base.substring(0, i + 1);
        else
          base = "";
      }
    if ("".equals(base))
      {
        // assume file URL relative to current directory
        base = System.getProperty("user.dir");
        if (base.charAt(0) == '/')
          base = base.substring(1);
        base = "file:///" + base.replace(File.separatorChar, '/');
        if (!base.endsWith("/"))
          base += "/";
      }
    while (href.startsWith(".."))
      {
        int i = base.lastIndexOf('/', base.length() - 2);
        if (i != -1)
          base = base.substring(0, i + 1);
        href = href.substring(2);
        if (href.startsWith("/"))
          href = href.substring(1);
      }
    return base + href;
  }

  private InputStream resolve(String url)
    throws IOException
  {
    try
      {
        return new URL(url).openStream();
      }
    catch (MalformedURLException e)
      {
        return null;
      }
  }

  private void popInput()
  {
    inputStack.removeLast();
    input = (Input) inputStack.getLast();
    //System.out.print("\n(-input:"+input.systemId+")"); 
  }

  /**
   * Parse the XML declaration.
   */
  private void readXMLDecl()
    throws IOException, XMLStreamException
  {
    final int flags = LIT_DISABLE_CREF | LIT_DISABLE_PE | LIT_DISABLE_EREF;
    
    require("version");
    readEq();
    xmlVersion = readLiteral(flags);
    if ("1.0".equals(xmlVersion))
      {
        xml11 = false;
      }
    else if ("1.1".equals(xmlVersion))
      {
        xml11 = true;
      }
    else
      throw new XMLStreamException("illegal XML version: " + xmlVersion);
    
    boolean white = tryWhitespace();
    
    if (tryRead("encoding"))
      {
        if (!white)
          error("whitespace required before 'encoding='");
        readEq();
        xmlEncoding = readLiteral(flags);
        if (!input.forceReader)
          input.setInputEncoding(xmlEncoding);
        white = tryWhitespace();
      }
    
    if (tryRead("standalone"))
      {
        if (!white)
          error("whitespace required before 'standalone='");
        readEq();
        String standalone = readLiteral(flags);
        if ("yes".equals(standalone))
          xmlStandalone = Boolean.TRUE;
        else if ("no".equals(standalone))
          xmlStandalone = Boolean.FALSE;
        else
          error("standalone flag must be 'yes' or 'no'", standalone);
      }

    skipWhitespace();
    require("?>");
  }

  /**
   * Parse the DOCTYPE declaration.
   */
  private void readDoctypeDecl()
    throws IOException, XMLStreamException
  {
    if (!supportDTD)
      error("parser was configured not to support DTDs");
    requireWhitespace();
    String rootName = readNmtoken(true);
    skipWhitespace();
    ExternalIds ids = readExternalIds(false, true);
    doctype =
      this.new Doctype(rootName, ids.publicId, ids.systemId);
    
    // Parse internal subset first
    skipWhitespace();
    if (tryRead('['))
      {
        while (true)
          {
            expandPE = true;
            skipWhitespace();
            expandPE = false;
            if (tryRead(']'))
              break;
            else
              {
                peIsError = expandPE = true;
                readMarkupdecl();
                peIsError = expandPE = false;
              }
          }
      }
    skipWhitespace();
    require('>');

    // Parse external subset
    if (ids.systemId != null)
      {
        pushInput(">");
        pushInput(ids);
        // loop until we get back to ">"
        while (true)
          {
            expandPE = true;
            skipWhitespace();
            expandPE = false;
            if (tryRead('>'))
              break;
            else
              {
                peIsError = expandPE = true;
                readMarkupdecl();
                peIsError = expandPE = false;
              }
          }
        if (inputStack.size() != 2)
          error("external subset has unmatched '>'");
      }

    // Make rootName available for reading
    buf.setLength(0);
    buf.append(rootName);
  }

  private void readMarkupdecl()
    throws IOException, XMLStreamException
  {
    boolean saved = expandPE;
    mark(1);
    require('<');
    reset();
    expandPE = false;
    if (tryRead(TEST_ELEMENT_DECL))
      {
        expandPE = saved;
        readElementDecl();
      }
    else if (tryRead(TEST_ATTLIST_DECL))
      {
        expandPE = saved;
        readAttlistDecl();
      }
    else if (tryRead(TEST_ENTITY_DECL))
      {
        expandPE = saved;
        readEntityDecl();
      }
    else if (tryRead(TEST_NOTATION_DECL))
      {
        expandPE = saved;
        readNotationDecl();
      }
    else if (tryRead(TEST_PI))
      {
        expandPE = saved;
        readPI();
      }
    else if (tryRead(TEST_COMMENT))
      {
        expandPE = saved;
        readComment();
      }
    else if (tryRead("<!["))
      {
        // conditional section
        expandPE = saved;
        if (inputStack.size() < 2)
          error("conditional sections illegal in internal subset");
        skipWhitespace();
        if (tryRead("INCLUDE"))
          {
            skipWhitespace();
            require('[');
            skipWhitespace();
            while (!tryRead("]]>"))
              {
                readMarkupdecl();
                skipWhitespace();
              }
          }
        else if (tryRead("IGNORE"))
          {
            skipWhitespace();
            require('[');
            expandPE = false;
            for (int nesting = 1; nesting > 0; )
              {
                char c = readCh();
                switch (c)
                  {
                  case '<':
                    if (tryRead("!["))
                      nesting++;
                    break;
                  case ']':
                    if (tryRead("]>"))
                      nesting--;
                  }
              }
            expandPE = true;
          }
        else
          error("conditional section must begin with INCLUDE or IGNORE");
      }
    else
      error("expected markup declaration");
  }

  private void readElementDecl()
    throws IOException, XMLStreamException
  {
    requireWhitespace();
    String name = readNmtoken(true);
    requireWhitespace();
    readContentspec(name);
    skipWhitespace();
    require('>');
  }

  private void readContentspec(String elementName)
    throws IOException, XMLStreamException
  {
    if (tryRead("EMPTY"))
      doctype.elements.put(elementName, "EMPTY");
    else if (tryRead("ANY"))
      doctype.elements.put(elementName, "ANY");
    else
      {
        StringBuffer acc = new StringBuffer();
        require('(');
        acc.append('(');
        skipWhitespace();
        if (tryRead("#PCDATA"))
          {
            // mixed content
            acc.append("#PCDATA");
            skipWhitespace();
            if (tryRead(')'))
              {
                acc.append(")*");
                tryRead('*');
              }
            else
              {
                while (!tryRead(")"))
                  {
                    require('|');
                    acc.append('|');
                    skipWhitespace();
                    acc.append(readNmtoken(true));
                    skipWhitespace();
                  }
                require('*');
                acc.append(")*");
              }
          }
        else
          readElements(acc);
        doctype.elements.put(elementName, acc.toString());
      }
  }

  private void readElements(StringBuffer acc)
    throws IOException, XMLStreamException
  {
    char separator;
    
    // Parse first content particle
    skipWhitespace();
    readContentParticle(acc);
    // End or separator
    skipWhitespace();
    char c = readCh();
    switch (c)
      {
      case ')':
        acc.append(')');
        mark(1);
        c = readCh();
        switch (c)
          {
          case '*':
          case '+':
          case '?':
            acc.append(c);
            break;
          default:
            reset();
          }
        return; // done
      case ',':
      case '|':
        separator = c;
        acc.append(c);
        break;
      default:
        error("bad separator in content model", new Character(c));
        return;
      }
    // Parse subsequent content particles
    while (true)
      {
        skipWhitespace();
        readContentParticle(acc);
        skipWhitespace();
        c = readCh();
        if (c == ')')
          {
            acc.append(')');
            break;
          }
        else if (c != separator)
          {
            error("bad separator in content model", new Character(c));
            return;
          }
        else
          acc.append(c);
      }
    // Check for occurrence indicator
    mark(1);
    c = readCh();
    switch (c)
      {
      case '?':
      case '*':
      case '+':
        acc.append(c);
        return;
      default:
        reset();
        return;
      }
  }

  private void readContentParticle(StringBuffer acc)
    throws IOException, XMLStreamException
  {
    if (tryRead('('))
      {
        acc.append('(');
        readElements(acc);
      }
    else
      {
        acc.append(readNmtoken(true));
        mark(1);
        char c = readCh();
        switch (c)
          {
          case '?':
          case '*':
          case '+':
            acc.append(c);
            break;
          default:
            reset();
          }
      }
  }

  /**
   * Parse an attribute-list definition.
   */
  private void readAttlistDecl()
    throws IOException, XMLStreamException
  {
    requireWhitespace();
    String elementName = readNmtoken(true);
    boolean white = tryWhitespace();
    while (!tryRead('>'))
      {
        if (!white)
          error("whitespace required before attribute definition");
        readAttDef(elementName);
        white = tryWhitespace();
      }
  }

  /**
   * Parse a single attribute definition.
   */
  private void readAttDef(String elementName)
    throws IOException, XMLStreamException
  {
    String name = readNmtoken(true);
    requireWhitespace();
    StringBuffer acc = new StringBuffer();
    String type = readAttType(acc);
    String enumer = null;
    if ("ENUMERATION".equals(type) || "NOTATION".equals(type))
      enumer = acc.toString();
    requireWhitespace();
    readDefault(elementName, name, type, enumer);
  }

  /**
   * Parse an attribute type.
   */
  private String readAttType(StringBuffer acc)
    throws IOException, XMLStreamException
  {
    if (tryRead('('))
      {
        readEnumeration(false, acc);
        return "ENUMERATION";
      }
    else
      {
        String typeString = readNmtoken(true);
        if ("NOTATION".equals(typeString))
          {
            readNotationType(acc);
            return typeString;
          }
        else if ("CDATA".equals(typeString) ||
                 "ID".equals(typeString) ||
                 "IDREF".equals(typeString) ||
                 "IDREFS".equals(typeString) ||
                 "ENTITY".equals(typeString) ||
                 "ENTITIES".equals(typeString) ||
                 "NMTOKEN".equals(typeString) ||
                 "NMTOKENS".equals(typeString))
          return typeString;
        else
          {
            error("illegal attribute type", typeString);
            return null;
          }
      }
  }

  /**
   * Parse an enumeration.
   */
  private void readEnumeration(boolean isNames, StringBuffer acc)
    throws IOException, XMLStreamException
  {
    acc.append('(');
    // first token
    skipWhitespace();
    acc.append(readNmtoken(isNames));
    // subsequent tokens
    skipWhitespace();
    while (!tryRead(')'))
      {
        require('|');
        acc.append('|');
        skipWhitespace();
        acc.append(readNmtoken(isNames));
        skipWhitespace();
      }
    acc.append(')');
  }

  /**
   * Parse a notation type for an attribute.
   */
  private void readNotationType(StringBuffer acc)
    throws IOException, XMLStreamException
  {
    requireWhitespace();
    require('(');
    readEnumeration(true, acc);
  }

  /**
   * Parse the default value for an attribute.
   */
  private void readDefault(String elementName, String name,
                           String type, String enumeration)
    throws IOException, XMLStreamException
  {
    int valueType = ATTRIBUTE_DEFAULT_SPECIFIED;
    int flags = LIT_ATTRIBUTE;
    String value = null, defaultType = null;
    boolean saved = expandPE;
    
    if (!"CDATA".equals(type))
      flags |= LIT_NORMALIZE;

    expandPE = false;
    if (tryRead('#'))
      {
        if (tryRead("FIXED"))
          {
            defaultType = "#FIXED";
            valueType = ATTRIBUTE_DEFAULT_FIXED;
            requireWhitespace();
            value = readLiteral(flags);
          }
        else if (tryRead("REQUIRED"))
          {
            defaultType = "#REQUIRED";
            valueType = ATTRIBUTE_DEFAULT_REQUIRED;
          }
        else if (tryRead("IMPLIED"))
          {
            defaultType = "#IMPLIED";
            valueType = ATTRIBUTE_DEFAULT_IMPLIED;
          }
        else
          error("illegal keyword for attribute default value");
      }
    else
      value = readLiteral(flags);
    expandPE = saved;
    /*if ("ENUMERATION".equals(type))
      type = enumer;
    else if ("NOTATION".equals(type))
      type = "NOTATION " + enumer;*/
    // Register attribute def
    LinkedHashMap attlist =
      (LinkedHashMap) doctype.attlists.get(elementName);
    if (attlist == null)
      {
        attlist = new LinkedHashMap();
        doctype.attlists.put(elementName, attlist);
      }
    if (attlist.containsKey(name))
      {
        return;
      }
    AttributeDecl attribute =
      new AttributeDecl(type, value, valueType, enumeration);
    attlist.put(name, attribute);
  }

  private void readEntityDecl()
    throws IOException, XMLStreamException
  {
    int flags = 0;
    // Check if parameter entity
    boolean peFlag = false;
    expandPE = false;
    requireWhitespace();
    if (tryRead('%'))
      {
        peFlag = true;
        requireWhitespace();
      }
    expandPE = true;
    // Read entity name
    String name = readNmtoken(true);
    if (name.indexOf(':') != -1)
      error("Illegal character ':' in entity name", name);
    if (peFlag)
      name = "%" + name;
    requireWhitespace();
    mark(1);
    char c = readCh();
    reset();
    if (c == '"' || c == '\'')
      {
        // Internal entity replacement text
        String value = readLiteral(flags);
        doctype.entities.put(name, value);
      }
    else
      {
        ExternalIds ids = readExternalIds(false, false);
        // Check for NDATA
        boolean white = tryWhitespace();
        if (!peFlag && tryRead("NDATA"))
          {
            if (!white)
              error("whitespace required before NDATA");
            requireWhitespace();
            ids.notationName = readNmtoken(true);
          }
        doctype.entities.put(name, ids);
      }
    // finish
    skipWhitespace();
    require('>');
  }

  private void readNotationDecl()
    throws IOException, XMLStreamException
  {
    requireWhitespace();
    String notationName = readNmtoken(true);
    if (notationName.indexOf(':') != -1)
      error("Illegal character ':' in notation name", notationName);
    requireWhitespace();
    ExternalIds ids = readExternalIds(true, false);
    ids.notationName = notationName;
    doctype.notations.put(notationName, ids);
    skipWhitespace();
    require('>');
  }

  /**
   * Returns a tuple {publicId, syatemId}.
   */
  private ExternalIds readExternalIds(boolean inNotation, boolean isSubset)
    throws IOException, XMLStreamException
  {
    char c;
    int flags = LIT_DISABLE_CREF | LIT_DISABLE_PE | LIT_DISABLE_EREF;
    ExternalIds ids = new ExternalIds();
    
    if (tryRead("PUBLIC"))
      {
        requireWhitespace();
        ids.publicId = readLiteral(LIT_NORMALIZE | LIT_PUBID | flags);
        if (inNotation)
          {
            skipWhitespace();
            mark(1);
            c = readCh();
            reset();
            if (c == '"' || c == '\'')
              ids.systemId = readLiteral(flags);
          }
        else
          {
            requireWhitespace();
            ids.systemId = readLiteral(flags);
          }

        for (int i = 0; i < ids.publicId.length(); i++)
          {
            c = ids.publicId.charAt(i);
            if (c >= 'a' && c <= 'z')
              continue;
            if (c >= 'A' && c <= 'Z')
              continue;
            if (" \r\n0123456789-' ()+,./:=?;!*#@$_%".indexOf(c) != -1)
              continue;
            error("illegal PUBLIC id character",
                  "U+" + Integer.toHexString(c));
          }
      }
    else if (tryRead("SYSTEM"))
      {
        requireWhitespace();
        ids.systemId = readLiteral(flags);
      }
    else if (!isSubset)
      {
        error("missing SYSTEM or PUBLIC keyword");
      }
    if (ids.systemId != null)
      {
        if (ids.systemId.indexOf('#') != -1)
          error("SYSTEM id has a URI fragment", ids.systemId);
      }
    return ids;
  }

  /**
   * Parse the start of an element.
   * @return the state of the parser afterwards (EMPTY_ELEMENT or CONTENT)
   */
  private int readStartElement()
    throws IOException, XMLStreamException
  {
    //System.err.println("readStartElement");
    // Read element name
    String elementName = readNmtoken(true);
    attrs.clear();
    // Push namespace context
    if (namespaceAware)
      namespaces.addFirst(new LinkedHashMap());
    // Read element content
    boolean white = tryWhitespace();
    mark(1);
    char c = readCh();
    while (c != '/' && c != '>')
      {
        // Read attribute
        reset();
        if (!white)
          error("need whitespace between attributes");
        readAttribute(elementName);
        white = tryWhitespace();
        mark(1);
        c = readCh();
      }
    // supply defaulted attributes
    if (doctype != null)
      {
        LinkedHashMap attlist =
          (LinkedHashMap) doctype.attlists.get(elementName);
        if (attlist != null)
          {
            for (Iterator i = attlist.entrySet().iterator(); i.hasNext(); )
              {
                Map.Entry entry = (Map.Entry) i.next();
                String attName = (String) entry.getKey();
                if (namespaceAware && attName.equals("xmlns"))
                  {
                    LinkedHashMap ctx =
                      (LinkedHashMap) namespaces.getFirst();
                    if (ctx.containsKey(XMLConstants.DEFAULT_NS_PREFIX))
                      continue; // namespace was specified
                  }
                else if (namespaceAware && attName.startsWith("xmlns:"))
                  {
                    LinkedHashMap ctx =
                      (LinkedHashMap) namespaces.getFirst();
                    if (ctx.containsKey(attName.substring(6)))
                      continue; // namespace was specified
                  }
                else
                  {
                    for (Iterator j = attrs.iterator(); j.hasNext(); )
                      {
                        Attribute a = (Attribute) j.next();
                        if (attName.equals(a.name))
                          continue; // attribute was specified
                      }
                  }
                AttributeDecl decl = (AttributeDecl) entry.getValue();
                if (decl.value == null)
                  continue;
                Attribute attr =
                  new Attribute(attName, decl.type, false, decl.value);
                if (namespaceAware)
                  {
                    if (!addNamespace(attr))
                      attrs.add(attr);
                  }
                else
                  attrs.add(attr);
              }
          }
      }
    // make element name available for read
    buf.setLength(0);
    buf.append(elementName);
    switch (c)
      {
      case '>':
        // push element onto stack
        stack.addLast(elementName);
        return CONTENT;
      case '/':
        require('>');
        return EMPTY_ELEMENT;
      }
    return -1; // to satisfy compiler
  }

  /**
   * Parse an attribute.
   */
  private void readAttribute(String elementName)
    throws IOException, XMLStreamException
  {
    //System.err.println("readAttribute");
    // Read attribute name
    String attributeName = readNmtoken(true);
    String type = getAttributeType(elementName, attributeName);
    readEq();
    // Read literal
    final int flags = LIT_ATTRIBUTE |  LIT_ENTITY_REF;
    String value = (type == null || "CDATA".equals(type)) ?
      readLiteral(flags) : readLiteral(flags | LIT_NORMALIZE);
    // add attribute event
    Attribute attr = this.new Attribute(attributeName, type, true, value);
    if (namespaceAware && attributeName.equals("xmlns"))
      {
        LinkedHashMap ctx = (LinkedHashMap) namespaces.getFirst();
        if (ctx.containsKey(XMLConstants.DEFAULT_NS_PREFIX))
          error("duplicate default namespace");
      }
    else if (namespaceAware && attributeName.startsWith("xmlns:"))
      {
        LinkedHashMap ctx = (LinkedHashMap) namespaces.getFirst();
        if (ctx.containsKey(attributeName.substring(6)))
          error("duplicate namespace", attributeName.substring(6));
      }
    else
      {
        if (attrs.contains(attr))
          error("duplicate attribute", attributeName);
      }
    if (namespaceAware)
      {
        if (!addNamespace(attr))
          attrs.add(attr);
      }
    else
      attrs.add(attr);
  }

  private boolean addNamespace(Attribute attr)
    throws XMLStreamException
  {
    if ("xmlns".equals(attr.name))
      {
        LinkedHashMap ctx = (LinkedHashMap) namespaces.getFirst();
        if (ctx.get(XMLConstants.DEFAULT_NS_PREFIX) != null)
          error("Duplicate default namespace declaration");
        ctx.put(XMLConstants.DEFAULT_NS_PREFIX, attr.value);
        return true;
      }
    else if ("xmlns".equals(attr.prefix))
      {
        LinkedHashMap ctx = (LinkedHashMap) namespaces.getFirst();
        if (ctx.get(attr.localName) != null)
          error("Duplicate namespace declaration for prefix",
                attr.localName);
        ctx.put(attr.localName, attr.value);
        return true;
      }
    return false;
  }

  /**
   * Parse a closing tag.
   */
  private void readEndElement()
    throws IOException, XMLStreamException
  {
    //System.err.println("readEndElement");
    // pop element off stack
    String expected = (String) stack.removeLast();
    require(expected);
    skipWhitespace();
    require('>');
    // Pop namespace context
    if (namespaceAware)
      namespaces.removeFirst();
    // Make element name available
    buf.setLength(0);
    buf.append(expected);
  }

  /**
   * Parse a comment.
   */
  private void readComment()
    throws IOException, XMLStreamException
  {
    boolean saved = expandPE;
    expandPE = false;
    buf.setLength(0);
    readUntil(TEST_END_COMMENT);
    require('>');
    expandPE = saved;
  }

  /**
   * Parse a processing instruction.
   */
  private void readPI()
    throws IOException, XMLStreamException
  {
    boolean saved = expandPE;
    expandPE = false;
    piTarget = readNmtoken(true);
    if (piTarget.indexOf(':') != -1)
      error("Illegal character in PI target", new Character(':'));
    if ("xml".equalsIgnoreCase(piTarget))
      error("Illegal PI target", piTarget);
    if (tryRead(TEST_END_PI))
      piData = null;
    else
      {
        if (!tryWhitespace())
          error("whitespace required between PI target and data");
        buf.setLength(0);
        readUntil(TEST_END_PI);
        piData = buf.toString();
      }
    expandPE = saved;
  }

  /**
   * Parse an entity reference.
   */
  private void readReference()
    throws IOException, XMLStreamException
  {
    buf.setLength(0);
    String entityName = readNmtoken(true);
    require(';');
    buf.setLength(0);
    buf.append(entityName);
  }

  /**
   * Read an CDATA section.
   */
  private void readCDSect()
    throws IOException, XMLStreamException
  {
    buf.setLength(0);
    readUntil(TEST_END_CDATA);
  }

  /**
   * Read character data.
   * @return the type of text read (CHARACTERS or SPACE)
   */
  private int readCharData()
    throws IOException, XMLStreamException
  {
    //System.out.println("readCharData");
    boolean white = true;
    buf.setLength(0);
    boolean done = false;
    while (!done)
      {
        // Block read
        mark(tmpBuf.length);
        int len = read(tmpBuf, 0, tmpBuf.length);
        if (len == -1)
          throw new EOFException();
        for (int i = 0; i < len && !done; i++)
          {
            char c = tmpBuf[i];
            switch (c)
              {
              case ' ':
              case '\t':
              case '\n':
              case '\r':
                buf.append(c);
                break; // whitespace
              case '&':
                reset();
                read(tmpBuf, 0, i);
                // character reference?
                mark(3);
                c = readCh();
                c = readCh();
                if (c == '#')
                  {
                    mark(1);
                    c = readCh();
                    boolean hex = (c == 'x');
                    if (!hex)
                      reset();
                    char[] ch = readCharacterRef(hex ? 16 : 10);
                    buf.append(ch, 0, ch.length);
                    for (int j = 0; j < ch.length; j++)
                      {
                        switch (ch[j])
                          {
                          case ' ':
                          case '\t':
                          case '\n':
                          case '\r':
                            break; // whitespace
                          default:
                            white = false;
                          }
                      }
                    // continue processing
                    i = -1;
                    mark(tmpBuf.length);
                    len = read(tmpBuf, 0, tmpBuf.length);
                    if (len == -1)
                      throw new EOFException();
                  }
                else
                  {
                    // entity reference
                    reset();
                    if (replaceERefs)
                      {
                        // inline reference with replacement text
                        c = readCh();
                        String entityName = readNmtoken(true);
                        require(';');
                        expandEntity(entityName);
                      }
                    else
                      {
                        // report reference as separate event
                        done = true;
                      }
                  }
                break; // end of text sequence
              case '<':
                reset();
                read(tmpBuf, 0, i);
                done = true;
                break; // end of text sequence
              default:
                if ((c < 0x0020 || c > 0xfffd) ||
                    (xml11 && (c >= 0x007f) && (c <= 0x009f) && (c != 0x0085)))
                  {
                    error("illegal XML character",
                          "U+" + Integer.toHexString(c));
                  }
                white = false;
                buf.append(c);
              }
          }
        // if text buffer >= 2MB, return it as a chunk
        // to avoid excessive memory use
        if (!coalescing && buf.length() >= 2097152)
          done = true;
      }
    return white ? XMLStreamConstants.SPACE : XMLStreamConstants.CHARACTERS;
  }

  /**
   * Expands the specified entity.
   */
  private void expandEntity(String name)
    throws IOException, XMLStreamException
  {
    if (doctype != null)
      {
        Object value = doctype.entities.get(name);
        if (value != null)
          {
            if (value instanceof String)
              pushInput((String) value);
            else
              pushInput((ExternalIds) value);
            return;
          }
      }
    error("Reference to undeclared entity", name);
  }

  /**
   * Read an equals sign.
   */
  private void readEq()
    throws IOException, XMLStreamException
  { 
    skipWhitespace();
    require('=');
    skipWhitespace();
  }

  private String readLiteral(int flags)
    throws IOException, XMLStreamException
  {
    boolean saved = expandPE;
    char delim = readCh();
    if (delim != '\'' && delim != '"')
      error("expected '\"' or \"'\"", new Character(delim));
    buf.setLength(0);
    if ((flags & LIT_DISABLE_PE) != 0)
      expandPE = false;
    for (char c = readCh(); c != delim; c = readCh())
      {
        switch (c)
          {
          case '\n':
          case '\r':
            if ((flags & (LIT_ATTRIBUTE | LIT_PUBID)) != 0)
              c = ' '; // normalize to space
            break;
          case '\t':
            if ((flags & LIT_ATTRIBUTE) != 0)
              c = ' '; // normalize to space
            break;
          case '&':
            mark(2);
            c = readCh();
            if (c == '#')
              {
                if ((flags & LIT_DISABLE_CREF) != 0)
                  error("literal may not contain character reference");
                mark(1);
                c = readCh();
                boolean hex = (c == 'x');
                if (!hex)
                  reset();
                char[] ref = readCharacterRef(hex ? 16 : 10);
                for (int i = 0; i < ref.length; i++)
                  buf.append(ref[i]);
                continue;
              }
            else
              {
                if ((flags & LIT_DISABLE_EREF) != 0)
                  error("literal may not contain entity reference");
                reset();
                if (replaceERefs || (flags & LIT_NORMALIZE) > 0)
                  {
                    String entityName = readNmtoken(true);
                    require(';');
                    expandEntity(entityName);
                    continue;
                  }
                else
                  error("parser is configured not to replace entity " +
                        "references");
              }
            break;
          case '<':
            if ((flags & LIT_ATTRIBUTE) != 0)
              error("attribute values may not contain '<'");
            break;
          }
        buf.append(c);
      }
    expandPE = saved;
    if ((flags & LIT_NORMALIZE) > 0)
      normalize();
    return buf.toString();
  }

  /**
   * Performs attribute-value normalization of the text buffer.
   * This discards leading and trailing whitespace, and replaces sequences
   * of whitespace with a single space.
   */
  private void normalize()
  {
    StringBuffer acc = new StringBuffer();
    int len = buf.length();
    int avState = 0;
    for (int i = 0; i < len; i++)
      {
        char c = buf.charAt(i);
        if (c == ' ')
          avState = (avState == 0) ? 0 : 1;
        else
          {
            if (avState == 1)
              acc.append(' ');
            acc.append(c);
            avState = 2;
          }
      }
    buf = acc;
  }

  /**
   * Parse and expand a parameter entity reference.
   */
  private void parsePEReference()
    throws IOException, XMLStreamException
  {
    String name = readNmtoken(true);
    require(';');
    if (doctype != null)
      {
        Object entity = doctype.entities.get("%" + name);
        if (entity != null)
          {
            if (entity instanceof String)
              pushInput((String) entity);
            else
              pushInput((ExternalIds) entity);
          }
      }
    error("reference to undeclared parameter entity", name);
  }

  private char[] readCharacterRef(int base)
    throws IOException, XMLStreamException
  {
    StringBuffer b = new StringBuffer();
    for (char c = readCh(); c != ';'; c = readCh())
      b.append(c);
    try
      {
        int ord = Integer.parseInt(b.toString(), base);
        if ((ord < 0x20 && !(ord == 0x0a || ord == 0x09 || ord == 0x0d))
            || (ord >= 0xd800 && ord <= 0xdfff)
            || ord == 0xfffe || ord == 0xffff
            || ord > 0x0010ffff)
          error("illegal XML character reference" +
                "U+" + Integer.toHexString(ord));
        return Character.toChars(ord);
      }
    catch (NumberFormatException e)
      {
        error("illegal characters in character reference", b.toString());
        return null;
      }
  }

  private String readNmtoken(boolean isName)
    throws IOException, XMLStreamException
  {
    nmtokenBuf.setLength(0);
    char c = readCh();
    if (isName)
      {
        if (!isNameStartCharacter(c))
          error("not a name start character",
                "U+" + Integer.toHexString(c));
      }
    else
      {
        if (!isNameCharacter(c))
          error("not a name character",
                "U+" + Integer.toHexString(c));
      }
    nmtokenBuf.append(c);
    do
      {
        mark(1);
        c = readCh();
        switch (c)
          {
          case '%':
          case '<':
          case '>':
          case '&':
          case ',':
          case '|':
          case '*':
          case '+':
          case '?':
          case ')':
          case '=':
          case '\'':
          case '"':
          case '[':
          case ' ':
          case '\t':
          case '\n':
          case '\r':
          case ';':
          case '/':
            reset();
            return intern(nmtokenBuf.toString());
          default:
            if (!isNameCharacter(c))
              error("not a name character",
                    "U+" + Integer.toHexString(c));
            else
              nmtokenBuf.append(c);
          }
      }
    while (true);
  }

  private boolean isNameStartCharacter(char c)
  {
    if (xml11)
      {
        if ((c < 0x0041 || c > 0x005a) &&
            (c < 0x0061 || c > 0x007a) &&
            c != ':' && c != '_' &&
            (c < 0x00c0 || c > 0x00d6) &&
            (c < 0x00d8 || c > 0x00f6) &&
            (c < 0x00f8 || c > 0x02ff) &&
            (c < 0x0370 || c > 0x037d) &&
            (c < 0x037f || c > 0x1fff) &&
            (c < 0x200c || c > 0x200d) &&
            (c < 0x2070 || c > 0x218f) &&
            (c < 0x2c00 || c > 0x2fef) &&
            (c < 0x3001 || c > 0xd7ff) &&
            (c < 0xf900 || c > 0xfdcf) &&
            (c < 0xfdf0 || c > 0xfffd) &&
            (c < 0x10000 || c > 0xeffff))
          return false;
      }
    else
      {
        int type = Character.getType(c);
        switch (type)
          {
          case Character.LOWERCASE_LETTER: // Ll
          case Character.UPPERCASE_LETTER: // Lu
          case Character.OTHER_LETTER: // Lo
          case Character.TITLECASE_LETTER: // Lt
          case Character.LETTER_NUMBER: // Nl
            if ((c > 0xf900 && c < 0xfffe) ||
                (c >= 0x20dd && c <= 0x20e0))
              {
                // Compatibility area and Unicode 2.0 exclusions
                return false;
              }
            break;
          default:
            if (c != ':' && c != '_' && (c < 0x02bb || c > 0x02c1) &&
                c != 0x0559 && c != 0x06e5 && c != 0x06e6)
              return false; 
          }
      }
    return true;
  }

  private boolean isNameCharacter(char c)
  {
    if (xml11)
      {
        if ((c < 0x0041 || c > 0x005a) &&
            (c < 0x0061 || c > 0x007a) &&
            (c < 0x0030 || c > 0x0039) &&
            c != ':' && c != '_' && c != '-' && c != '.' &&
            (c < 0x00c0 || c > 0x00d6) &&
            (c < 0x00d8 || c > 0x00f6) &&
            (c < 0x00f8 || c > 0x02ff) &&
            (c < 0x0370 || c > 0x037d) &&
            (c < 0x037f || c > 0x1fff) &&
            (c < 0x200c || c > 0x200d) &&
            (c < 0x2070 || c > 0x218f) &&
            (c < 0x2c00 || c > 0x2fef) &&
            (c < 0x3001 || c > 0xd7ff) &&
            (c < 0xf900 || c > 0xfdcf) &&
            (c < 0xfdf0 || c > 0xfffd) &&
            (c < 0x10000 || c > 0xeffff) &&
            c != 0x00b7 && 
            (c < 0x0300 || c > 0x036f) &&
            (c < 0x203f || c > 0x2040))
          return false;
      }
    else
      {
        int type = Character.getType(c);
        switch (type)
          {
          case Character.LOWERCASE_LETTER: // Ll
          case Character.UPPERCASE_LETTER: // Lu
          case Character.DECIMAL_DIGIT_NUMBER: // Nd
          case Character.OTHER_LETTER: // Lo
          case Character.TITLECASE_LETTER: // Lt
          case Character.LETTER_NUMBER: // Nl
          case Character.COMBINING_SPACING_MARK: // Mc
          case Character.ENCLOSING_MARK: // Me
          case Character.NON_SPACING_MARK: // Mn
          case Character.MODIFIER_LETTER: // Lm
            if ((c > 0xf900 && c < 0xfffe) ||
                (c >= 0x20dd && c <= 0x20e0))
              return false;
            break;
          default:
            if (c != '-' && c != '.' && c != ':' && c != '_' &&
                c != 0x0387 && (c < 0x02bb || c > 0x02c1) && 
                c != 0x0559 && c != 0x06e5 && c != 0x06e6 && c != 0x00b7)
              return false;
          }
      }
    return true;
  }

  private String intern(String text)
  {
    return stringInterning ? text.intern() : text;
  }

  private void error(String message)
    throws XMLStreamException
  {
    error(message, null);
  }
  
  private void error(String message, Object info)
    throws XMLStreamException
  {
    if (info != null)
      {
        if (info instanceof String)
          message += ": \"" + ((String) info) + "\"";
        else if (info instanceof Character)
          message += ": '" + ((Character) info) + "'";
      }
    throw new XMLStreamException(message);
  }

  public static void main(String[] args)
    throws Exception
  {
    XMLParser p = new XMLParser(new java.io.FileInputStream(args[0]), args[0]);
    try
      {
        int event;
        do
          {
            event = p.next();
            switch (event)
              {
              case XMLStreamConstants.START_DOCUMENT:
                System.out.println("START_DOCUMENT version="+p.getVersion()+
                                   " encoding="+p.getEncoding());
                break;
              case XMLStreamConstants.END_DOCUMENT:
                System.out.println("END_DOCUMENT");
                break;
              case XMLStreamConstants.START_ELEMENT:
                System.out.println("START_ELEMENT "+p.getName());
                int l = p.getNamespaceCount();
                for (int i = 0; i < l; i++)
                  System.out.println("\tnamespace "+p.getNamespacePrefix(i)+
                                     "='"+p.getNamespaceURI(i)+"'");
                l = p.getAttributeCount();
                for (int i = 0; i < l; i++)
                  System.out.println("\tattribute "+p.getAttributeQName(i)+
                                     "='"+p.getAttributeValue(i)+"'");
                break;
              case XMLStreamConstants.END_ELEMENT:
                System.out.println("END_ELEMENT "+p.getName());
                break;
              case XMLStreamConstants.CHARACTERS:
                System.out.println("CHARACTERS '"+p.getText()+"'");
                break;
              case XMLStreamConstants.CDATA:
                System.out.println("CDATA '"+p.getText()+"'");
                break;
              case XMLStreamConstants.SPACE:
                System.out.println("SPACE '"+p.getText()+"'");
                break;
              case XMLStreamConstants.DTD:
                System.out.println("DTD "+p.getText());
                break;
              case XMLStreamConstants.ENTITY_REFERENCE:
                System.out.println("ENTITY_REFERENCE "+p.getText());
                break;
              case XMLStreamConstants.COMMENT:
                System.out.println("COMMENT '"+p.getText()+"'");
                break;
              case XMLStreamConstants.PROCESSING_INSTRUCTION:
                System.out.println("PROCESSING_INSTRUCTION "+p.getPITarget()+
                                   " "+p.getPIData());
                break;
              case XMLStreamConstants.START_ENTITY:
                System.out.println("START_ENTITY");
                break;
              case XMLStreamConstants.END_ENTITY:
                System.out.println("END_ENTITY");
                break;
              default:
                System.out.println("Unknown event: "+event);
              }
          }
        while (event != -1 && event != XMLStreamConstants.END_DOCUMENT);
      }
    catch (XMLStreamException e)
      {
        Location l = p.getLocation();
        System.out.println("At line "+l.getLineNumber()+
                           ", column "+l.getColumnNumber()+
                           " of "+l.getLocationURI());
        throw e;
      }
  }

  class Attribute
  {

    final String name;
    final String type;
    final boolean specified;
    final String value;
    final String prefix;
    final String localName;

    Attribute(String name, String type, boolean specified, String value)
    {
      this.name = name;
      this.type = type;
      this.specified = specified;
      this.value = value;
      int ci = name.indexOf(':');
      if (ci == -1)
        {
          prefix = null;
          localName = name;
        }
      else
        {
          prefix = intern(name.substring(0, ci));
          localName = intern(name.substring(ci + 1));
        }
    }

    public boolean equals(Object other)
    {
      if (other instanceof Attribute)
        {
          return ((Attribute) other).name.equals(name);
        }
      return false;
    }
    
  }

  class Doctype
  {

    final String rootName;
    final String publicId;
    final String systemId;
    final LinkedHashMap elements = new LinkedHashMap();
    final LinkedHashMap attlists = new LinkedHashMap();
    final LinkedHashMap entities = new LinkedHashMap();
    final LinkedHashMap notations = new LinkedHashMap();

    Doctype(String rootName, String publicId, String systemId)
    {
      this.rootName = rootName;
      this.publicId = publicId;
      this.systemId = systemId;
    }
    
  }

  class ExternalIds
  {
    String publicId;
    String systemId;
    String notationName;
  }

  class AttributeDecl
  {
    
    final String type;
    final String value;
    final int valueType;
    final String enumeration;

    AttributeDecl(String type, String value,
                  int valueType, String enumeration)
    {
      this.type = type;
      this.value = value;
      this.valueType = valueType;
      this.enumeration = enumeration;
    }
    
  }

  static class Input
  {
    
    int line = 1, markLine;
    int column, markColumn;
    int offset, markOffset;
    String publicId, systemId;
    
    InputStream in;
    Reader reader;
    boolean forceReader;
    String inputEncoding;

    Input(InputStream in, String publicId, String systemId)
    {
      this(in, null, publicId, systemId, null);
    }
    
    Input(InputStream in, String publicId, String systemId, String inputEncoding)
    {
      this(in, null, publicId, systemId, inputEncoding);
    }
    
    Input(InputStream in, Reader reader, String publicId, String systemId)
    {
      this(in, reader, publicId, systemId, null);
    }
    
    Input(InputStream in, Reader reader, String publicId, String systemId,
          String defaultEncoding)
    {
      if (defaultEncoding == null)
        defaultEncoding = "UTF-8";
      if (in != null && !in.markSupported())
        in = new BufferedInputStream(in);
      this.in = in;
      this.publicId = publicId;
      this.systemId = systemId;
      if (reader == null)
        {
          try
            {
              reader = new XMLInputStreamReader(in, defaultEncoding);
            }
          catch (UnsupportedEncodingException e)
            {
              RuntimeException e2 =
                new RuntimeException(defaultEncoding +
                                     " charset not supported");
              e2.initCause(e);
              throw e2;
            }
        }
      else
        forceReader = true;
      reader = new CRLFReader(reader);
      this.reader = reader;
    }

    void init()
      throws IOException
    {
      if (in != null && !forceReader)
        detectEncoding();
    }

    void mark(int len)
      throws IOException
    {
      //System.out.println("mark");
      markOffset = offset;
      markLine = line;
      markColumn = column;
      reader.mark(len);
    }

    /**
     * Character read.
     */
    int read()
      throws IOException
    {
      offset++;
      int ret = reader.read();
      //System.out.println("read:"+((char) ret));
      if (ret == 0x0a)
        {
          line++;
          column = 0;
        }
      else
        column++;
      return ret;
    }

    /**
     * Block read.
     */
    int read(char[] b, int off, int len)
      throws IOException
    {
      int count = 0;
      while (count < len)
        {
          int l2 = reader.read(b, off + count, len - count);
          if (l2 == -1)
            return (count > 0) ? count : -1;
          //System.out.println("read:"+new String(b, off + count, len - count));
          offset += l2;
          count += l2;
          for (int i = 0; i < l2; i++)
            {
              char c = b[off + count + i];
              if (c == 0x0a)
                {
                  line++;
                  column = 0;
                }
              else
                column++;
            }
        }
      return count;
    }

    void reset()
      throws IOException
    {
      //System.out.println("reset");
      reader.reset();
      offset = markOffset;
      line = markLine;
      column = markColumn;
    }

    // Detection of input encoding
    
    private static final int[] SIGNATURE_UCS_4_1234 =
      new int[] { 0x00, 0x00, 0x00, 0x3c };
    private static final int[] SIGNATURE_UCS_4_4321 =
      new int[] { 0x3c, 0x00, 0x00, 0x00 };
    private static final int[] SIGNATURE_UCS_4_2143 =
      new int[] { 0x00, 0x00, 0x3c, 0x00 };
    private static final int[] SIGNATURE_UCS_4_3412 =
      new int[] { 0x00, 0x3c, 0x00, 0x00 };
    private static final int[] SIGNATURE_UCS_2_12 =
      new int[] { 0xfe, 0xff };
    private static final int[] SIGNATURE_UCS_2_21 =
      new int[] { 0xff, 0xfe };
    private static final int[] SIGNATURE_UCS_2_12_NOBOM =
      new int[] { 0x00, 0x3c, 0x00, 0x3f };
    private static final int[] SIGNATURE_UCS_2_21_NOBOM =
      new int[] { 0x3c, 0x00, 0x3f, 0x00 };
    private static final int[] SIGNATURE_UTF_8 =
      new int[] { 0x3c, 0x3f, 0x78, 0x6d };
    private static final int[] SIGNATURE_UTF_8_BOM =
      new int[] { 0xef, 0xbb, 0xbf };
    
    /**
     * Detect the input encoding.
     */
    private void detectEncoding()
      throws IOException
    {
      int[] signature = new int[4];
      in.mark(4);
      for (int i = 0; i < 4; i++)
        signature[i] = in.read();
      in.reset();

      // 4-byte encodings
      if (equals(SIGNATURE_UCS_4_1234, signature))
        setInputEncoding("UTF-32BE");
      else if (equals(SIGNATURE_UCS_4_4321, signature))
        setInputEncoding("UTF-32LE");
      else if (equals(SIGNATURE_UCS_4_2143, signature) ||
               equals(SIGNATURE_UCS_4_3412, signature))
        throw new UnsupportedEncodingException("unsupported UCS-4 byte ordering");
      // 2-byte encodings
      else if (equals(SIGNATURE_UCS_2_12, signature))
        {
          setInputEncoding("UTF-16BE");
          in.read();
          in.read();
        }
      else if (equals(SIGNATURE_UCS_2_21, signature))
        {
          setInputEncoding("UTF-16LE");
          in.read();
          in.read();
        }
      else if (equals(SIGNATURE_UCS_2_12_NOBOM, signature))
        {
          setInputEncoding("UTF-16BE");
          throw new UnsupportedEncodingException("no byte-order mark for UCS-2 entity");
        }
      else if (equals(SIGNATURE_UCS_2_21_NOBOM, signature))
        {
          setInputEncoding("UTF-16LE");
          throw new UnsupportedEncodingException("no byte-order mark for UCS-2 entity");
        }
      // ASCII-derived encodings
      else if (equals(SIGNATURE_UTF_8, signature))
        {
          // UTF-8 input encoding implied, TextDecl
        }
      else if (equals(SIGNATURE_UTF_8_BOM, signature))
        {
          setInputEncoding("UTF-8");
          in.read();
          in.read();
          in.read();
        }
    }

    private static boolean equals(int[] b1, int[] b2)
    {
      for (int i = 0; i < b1.length; i++)
        {
          if (b1[i] != b2[i])
            return false;
        }
      return true;
    }
    
    private void setInputEncoding(String encoding)
      throws UnsupportedEncodingException
    {
      inputEncoding = encoding;
      reader = new XMLInputStreamReader((XMLInputStreamReader) reader, encoding);
      reader = new CRLFReader(reader);
    }

  }

}

