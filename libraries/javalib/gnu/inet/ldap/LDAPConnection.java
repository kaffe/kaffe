/*
 * LDAPConnection.java
 * Copyright (C) 2004 The Free Software Foundation
 * 
 * This file is part of GNU inetlib, a library.
 * 
 * GNU inetlib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU inetlib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
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

package gnu.inet.ldap;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.net.InetSocketAddress;
import java.net.ProtocolException;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.ArrayList;
import javax.naming.ldap.Control;

/**
 * An LDAPv3 client.
 * This client is still experimental, please contact
 * <a href='mailto:dog@gnu.org'>Chris Burdess</a> if you want to help out
 * with it.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class LDAPConnection
{

  /**
   * The default LDAP port.
   */
  public static final int DEFAULT_PORT = 389;

  public static final int SCOPE_BASE_OBJECT = 0;
  public static final int SCOPE_SINGLE_LEVEL = 1;
  public static final int SCOPE_WHOLE_SUBTREE = 2;

  /**
   * Do not dereference aliases in searching or in locating the base object
   * of the search.
   */
  public static final int DEREF_NEVER = 0;

  /**
   * Dereference aliases in subordinates of the base object in searching,
   * but not in locating the base object of the search.
   */
  public static final int DEREF_IN_SEARCHING = 1;

  /**
   * Dereference aliases in locating the base object of the search, but not
   * when searching subordinates of the base object.
   */
  public static final int DEREF_FINDING_BASE_OBJ = 2;

  /**
   * Dereference aliases both in searching and in locating the base object
   * of the search.
   */
  public static final int DEREF_ALWAYS = 3;

  private static final int SUCCESS = 0;
  private static final int SASL_BIND_IN_PROGRESS = 14;

  private static final int MESSAGE = 0x30;
  private static final int BIND_REQUEST = 0x60;
  private static final int BIND_RESPONSE = 0x61;
  private static final int UNBIND_REQUEST = 0x62;
  private static final int SEARCH_REQUEST = 0x63;
  private static final int SEARCH_RESULT = 0x64;
  private static final int SEARCH_RESULT_DONE = 0x65;
  private static final int MODIFY_REQUEST = 0x66;
  private static final int MODIFY_RESPONSE = 0x67;
  private static final int ADD_REQUEST = 0x68;
  private static final int ADD_RESPONSE = 0x69;
  private static final int DELETE_REQUEST = 0x6a;
  private static final int DELETE_RESPONSE = 0x6b;
  private static final int MODIFY_DN_REQUEST = 0x6c;
  private static final int MODIFY_DN_RESPONSE = 0x6d;
  private static final int SEARCH_REFERENCE = 0x73;

  protected String host;
  protected int port;
  protected int version; // 2 or 3
  
  private Socket socket;
  private InputStream in;
  private OutputStream out;
  
  private int messageId;
  private Map asyncResponses;

  /**
   * Creates a new LDAP connection to the specified host, using the default
   * LDAP port.
   * @param host the host
   */
  public LDAPConnection(String host)
    throws IOException
  {
    this(host, DEFAULT_PORT, 0, 0);
  }

  /**
   * Creates a new LDAP connection to the specified host and port.
   * @param host the host
   * @param port the port
   */
  public LDAPConnection(String host, int port)
    throws IOException
  {
    this(host, port, 0, 0);
  }

  /**
   * Creates a new LDAP connection to the specified host, port, and timeouts.
   * @param host the host
   * @param port the port
   * @param connectionTimeout the connection timeout in ms
   * @param timeout the socket I/O timeout in ms
   */
  public LDAPConnection(String host, int port,
                        int connectionTimeout, int timeout)
    throws IOException
  {
    this.host = host;
    if (port < 0)
      {
        port = DEFAULT_PORT;
      }
    this.port = port;
    messageId = 0;
    asyncResponses = new HashMap();
    version = 3;

    // Connect
    socket = new Socket();
    SocketAddress address = new InetSocketAddress(host, port);
    if (connectionTimeout > 0)
      {
        socket.connect(address, connectionTimeout);
      }
    else
      {
        socket.connect(address);
      }
    in = new BufferedInputStream(socket.getInputStream());
    out = new BufferedOutputStream(socket.getOutputStream());
  }

  /**
   * Sets the version of LDAP to use.
   * This implementation supports versions 2 and 3.
   * @param version the LDAP version
   */
  public void setVersion(int version)
  {
    if (version < 2 || version > 3)
      {
        throw new IllegalArgumentException(Integer.toString(version));
      }
    this.version = version;
  }

  /**
   * Initiates a bind operation to authenticate the client to the server.
   * @param name the LDAP DN to authenticate to, or <code>null</code> for
   * anonymous binding
   * @param mechanism the SASL mechanism to use, or <code>null</code> for
   * simple authentication
   * @param credentials the security credentials to use
   * @return the LDAP result
   */
  public LDAPResult bind(String name, String mechanism,
                         byte[] credentials, Control[] controls)
    throws IOException
  {
    int id = messageId++;
    boolean utf8 = (version == 3);
    BEREncoder bind = new BEREncoder(utf8);
    if (mechanism == null)
      {
        bind.append(version);
        bind.append(name);
        if (credentials != null)
          {
            bind.append(credentials);
          }
      }
    else
      {
        bind.append(version);
        bind.append(name);
        // SASL credentials
        BEREncoder saslCredentials = new BEREncoder(utf8);
        saslCredentials.append(mechanism);
        if (credentials != null)
          {
            saslCredentials.append(credentials);
          }
        bind.append(saslCredentials.toByteArray(), BERConstants.SEQUENCE);
      }
    // Request controls
    BEREncoder ctls = new BEREncoder(utf8);
    if (controls != null)
      {
        for (int i = 0; i < controls.length; i++)
          {
            ctls.append(controlSequence(controls[i], utf8),
                        BERConstants.SEQUENCE);
          }
      }
    bind.append(ctls.toByteArray(), BERConstants.CONTEXT);
    // Write request
    write(id, BIND_REQUEST, bind.toByteArray());
    // Read response
    BERDecoder response = read(id);
    BERDecoder resultSequence = response.parseSequence(BIND_RESPONSE);
    LDAPResult result = parseResult(resultSequence);
    if (resultSequence.available())
      {
        byte[] serverCreds = resultSequence.parseOctetString();
        // TODO
      }
    // TODO response controls
    return result;
  }

  /**
   * Issues an unbind request. This indicates to the server that the client
   * has no more requests to issue and will terminate the connection. After
   * invoking this method, no further methods may be invoked.
   */
  public void unbind()
    throws IOException
  {
    int id = messageId++;
    boolean utf8 = (version == 3);
    BEREncoder unbind = new BEREncoder(utf8);
    unbind.appendNull();
    write(id, UNBIND_REQUEST, unbind.toByteArray());
    // Close socket
    socket.close();
  }

  /**
   * Issues a search request.
   * @param name the LDAP DN that is the base object entry relative to which
   * the search is to be performed
   * @param scope the search scope, one of the SCOPE_* constants
   * @param derefAliases whether to dereference aliases, one of the DEREF_*
   * constants
   * @param sizeLimit the maximum number of entries to return, or 0 for no
   * restriction
   * @param timeLimit the maximum time in seconds permitted for the search,
   * or 0 for no restriction
   * @param typesOnly whether to return only attribute types(true) or both
   * attribute types and values(false)
   * @param filter the search filter in RFC2254 format
   * @param attributes the IDs of the attributes to return
   * @param controls the request controls
   * @param handler the result handler to receive notification of results
   * @return the LDAP result
   */
  public LDAPResult search(String name, int scope, int derefAliases,
                           int sizeLimit, int timeLimit,
                           boolean typesOnly, String filter,
                           String[] attributes, Control[] controls,
                           ResultHandler handler)
    throws IOException
  {
    if (filter == null || filter.length() == 0)
      {
        filter = "(objectClass=*)";
      }
    int id = messageId++;
    boolean utf8 = (version == 3);
    BEREncoder search = new BEREncoder(utf8);
    search.append(name);
    search.append(scope, BERConstants.ENUMERATED);
    search.append(derefAliases, BERConstants.ENUMERATED);
    search.append(sizeLimit);
    search.append(timeLimit);
    search.append(typesOnly);
    search.appendFilter(filter);
    BEREncoder attributeSequence = new BEREncoder(utf8);
    if (attributes != null)
      {
        for (int i = 0; i < attributes.length; i++)
          {
            attributeSequence.append(attributes[i]);
          }
      }
    search.append(attributeSequence.toByteArray(), BERConstants.SEQUENCE);
    // Request controls
    BEREncoder ctls = new BEREncoder(utf8);
    if (controls != null)
      {
        for (int i = 0; i < controls.length; i++)
          {
            ctls.append(controlSequence(controls[i], utf8),
                        BERConstants.SEQUENCE);
          }
      }
    search.append(ctls.toByteArray(), BERConstants.SEQUENCE);
    // Write request
    write(id, SEARCH_REQUEST, search.toByteArray());
    do
      {
        BERDecoder response = read(id);
        int code = response.parseType();
        switch (code)
          {
          case SEARCH_RESULT:
            BERDecoder entry = response.parseSequence(code);
            String objectName = entry.parseString();
            BERDecoder attributeSeq = entry.parseSequence(0x30);
            Map attrs = new TreeMap();
            while (attributeSeq.available())
              {
                BERDecoder attribute = attributeSeq.parseSequence(0x30);
                String type = attribute.parseString();
                BERDecoder values = attribute.parseSet(0x31);
                List acc = new ArrayList();
                while (values.available())
                  {
                    int valueType = values.parseType();
                    switch (valueType)
                      {
                      case BERConstants.BOOLEAN:
                        acc.add(Boolean.valueOf(values.parseBoolean()));
                        break;
                      case BERConstants.INTEGER:
                      case BERConstants.ENUMERATED:
                        acc.add(new Integer(values.parseInt()));
                        break;
                        // TODO float
                      case BERConstants.UTF8_STRING:
                        acc.add(values.parseString());
                        break;
                      case BERConstants.OCTET_STRING:
                        acc.add(values.parseOctetString());
                        break;
                      }
                  }
                attrs.put(type, acc);
              }
            handler.searchResultEntry(objectName, attrs);
            break;
          case SEARCH_REFERENCE:
            List acc = new ArrayList();
            BERDecoder urls = response.parseSequence(code);
            while (urls.available())
              {
                acc.add(urls.parseString());
              }
            handler.searchResultReference(acc);
            break;
          case SEARCH_RESULT_DONE:
            return parseResult(response.parseSequence(code));
          default:
            throw new ProtocolException("Unexpected response: " + code);
          }
      }
    while (true);
  }

  /**
   * Issues a modify request.
   * @param name the LDAP DN of the object to be modified(alias
   * dereferencing will not be performed)
   * @param modifications a sequence of modifications to be executed
   * to be executed
   * @see Modification
   */
  public LDAPResult modify(String name, final Modification[] modifications)
    throws IOException
  {
    int id = messageId++;
    boolean utf8 = (version == 3);
    BEREncoder modify = new BEREncoder(utf8);
    modify.append(name);
    BEREncoder modSeq = new BEREncoder(utf8);
    for (int i = 0; i < modifications.length; i++)
      {
        BEREncoder mod = new BEREncoder(utf8);
        mod.append(modifications[i].operation);
        BEREncoder typeAndValues = new BEREncoder(utf8);
        typeAndValues.append(modifications[i].type);
        BEREncoder values = new BEREncoder(utf8);
        appendValues(values, modifications[i].values);
        typeAndValues.append(values.toByteArray(), BERConstants.SET);
        mod.append(typeAndValues.toByteArray(), BERConstants.SEQUENCE);
        modSeq.append(mod.toByteArray(), BERConstants.SEQUENCE);
      }
    modify.append(modSeq.toByteArray(), BERConstants.SEQUENCE);
    // Write request
    write(id, MODIFY_REQUEST, modify.toByteArray());
    // Read response
    BERDecoder response = read(id);
    BERDecoder resultSequence = response.parseSequence(MODIFY_RESPONSE);
    LDAPResult result = parseResult(resultSequence);
    return result;
  }

  /**
   * Requests the addition of a new entry into the directory.
   * @param name the LDAP DN of the new entry
   * @param attributes a sequence of attributes to assign to the new entry
   */
  public LDAPResult add(String name, AttributeValues[] attributes)
    throws IOException
  {
    int id = messageId++;
    boolean utf8 = (version == 3);
    BEREncoder add = new BEREncoder(utf8);
    add.append(name);
    BEREncoder attrSeq = new BEREncoder(utf8);
    for (int i = 0; i < attributes.length; i++)
      {
        BEREncoder attr = new BEREncoder(utf8);
        attr.append(attributes[i].type);
        BEREncoder values = new BEREncoder(utf8);
        appendValues(values, attributes[i].values);
        attr.append(values.toByteArray(), BERConstants.SET);
        attrSeq.append(attr.toByteArray(), BERConstants.SEQUENCE);
      }
    add.append(attrSeq.toByteArray(), BERConstants.SEQUENCE);
    // Write request
    write(id, ADD_REQUEST, add.toByteArray());
    // Read response
    BERDecoder response = read(id);
    BERDecoder resultSequence = response.parseSequence(ADD_RESPONSE);
    LDAPResult result = parseResult(resultSequence);
    return result;
  }

  /**
   * Requests the removal of an entry from the directory.
   * @param name the LDAP DN of the entry to remove
   */
  public LDAPResult delete(String name)
    throws IOException
  {
    int id = messageId++;
    boolean utf8 = (version == 3);
    BEREncoder del = new BEREncoder(utf8);
    del.append(name);
    // Write request
    write(id, DELETE_REQUEST, del.toByteArray());
    // Read response
    BERDecoder response = read(id);
    int code = response.parseType();
    if (code != DELETE_RESPONSE)
      {
        throw new ProtocolException("Unexpected response type: " +
                                    code);
      }
    BERDecoder resultSequence = response.parseSequence();
    LDAPResult result = parseResult(resultSequence);
    return result;
  }

  /**
   * Changes the leftmost(least significant) component of the name of an
   * entry in the directory, or move a subtree of entries to a new location
   * in the directory.
   * @param name the LDAP DN of the entry to be changed
   * @param newRDN the RDN that will form the leftmost component of the new
   * name of the entry
   * @param deleteOldRDN if false, the old RDN values will be retained as
   * attributes of the entry, otherwise they are deleted from the entry
   * @param newSuperior if non-null, the DN of the entry to become the
   * immediate superior of the existing entry
   */
  public LDAPResult modifyDN(String name, String newRDN,
                             boolean deleteOldRDN, String newSuperior)
    throws IOException
  {
    int id = messageId++;
    boolean utf8 = (version == 3);
    BEREncoder modifyDN = new BEREncoder(utf8);
    modifyDN.append(name);
    modifyDN.append(newRDN);
    modifyDN.append(deleteOldRDN);
    if (newSuperior != null)
      {
        modifyDN.append(newSuperior);
      }
    // Write request
    write(id, MODIFY_DN_REQUEST, modifyDN.toByteArray());
    // Read response
    BERDecoder response = read(id);
    BERDecoder resultSequence = response.parseSequence(MODIFY_DN_RESPONSE);
    LDAPResult result = parseResult(resultSequence);
    return result;
  }

  /* TODO Compare Operation */

  /* TODO Abandon Operation */

  /* TODO Extended Operation */



  /**
   * Appends the specified set of values to the given encoder.
   */
  void appendValues(BEREncoder encoder, Set values)
    throws BERException
  {
    if (values != null)
      {
        for (Iterator i = values.iterator(); i.hasNext(); )
          {
            Object value = i.next();
            if (value == null)
              {
                encoder.appendNull();
              }
            else if (value instanceof String)
              {
                encoder.append((String) value);
              }
            else if (value instanceof Integer)
              {
                encoder.append(((Integer) value).intValue());
              }
            else if (value instanceof Boolean)
              {
                encoder.append(((Boolean) value).booleanValue());
              }
            else if (value instanceof byte[])
              {
                encoder.append((byte[]) value);
              }
            // TODO float
            else
              {
                throw new ClassCastException(value.getClass().getName());
              }
          }
      }
  }

  /**
   * Encode a control.
   */
  byte[] controlSequence(final Control control, boolean utf8)
    throws IOException
  {
    BEREncoder encoder = new BEREncoder(utf8);
    encoder.append(control.getID());
    if (control.isCritical())
      {
        encoder.append(true);
      }
    return encoder.toByteArray();
  }

  /**
   * Parse a response into an LDAP result object.
   */
  LDAPResult parseResult(BERDecoder response)
    throws IOException
  {
    int status = response.parseInt();
    String matchingDN = response.parseString();
    String errorMessage = response.parseString();
    String[] referrals = null;
    if (response.available())
      {
        int type = response.parseType();
        if (type == BERConstants.SEQUENCE)
          {
            ArrayList list = new ArrayList();
            BERDecoder sequence = response.parseSequence();
            type = sequence.parseType();
            while (type != -1)
              {
                list.add(sequence.parseString());
              }
            referrals = new String[list.size()];
            list.toArray(referrals);
          }
      }
    return new LDAPResult(status, matchingDN, errorMessage, referrals);
  }

  /**
   * Write a request.
   * @param id the message ID
   * @param code the operation code
   * @param request the request body
   */
  void write(int id, int code, byte[] request)
    throws IOException
  {
    boolean utf8 = (version == 3);
    BEREncoder envelope = new BEREncoder(utf8);
    envelope.append(id);
    envelope.append(request, code);
    BEREncoder message = new BEREncoder(utf8);
    message.append(envelope.toByteArray(), MESSAGE);
    byte[] toSend = message.toByteArray();
    // Write to socket
    out.write(toSend);
    out.flush();
  }

  /**
   * Read a response associated with the given message ID.
   * @param id the message ID
   * @return a BERDecoder for the content of the message
   */
  BERDecoder read(int id)
    throws IOException
  {
    // Check for an already received async response
    Integer key = new Integer(id);
    List responses = (List) asyncResponses.get(key);
    if (responses != null)
      {
        BERDecoder response = (BERDecoder) responses.remove(0);
        if (responses.size() == 0)
          {
            asyncResponses.remove(key);
          }
        return response;
      }
    do
      {
        // Read LDAP message
        byte[] bytes = readMessage();
        boolean utf8 = (version == 3);
        BERDecoder message = new BERDecoder(bytes, utf8);
        message = message.parseSequence(MESSAGE);
        // Check message ID
        int msgId = message.parseInt();
        if (msgId == id)
          {
            return message;
          }
        else
          {
            // Store this message for later processing
            key = new Integer(msgId);
            responses = (List) asyncResponses.get(key);
            if (responses == null)
              {
                responses = new ArrayList();
                asyncResponses.put(key, responses);
              }
            responses.add(message);
          }
      }
    while (true);
  }

  /**
   * Read an LDAP message.
   */
  byte[] readMessage()
    throws IOException
  {
    // Peek at the length part of the BER encoding to determine the length
    // of the message
    // TODO normalize this with functionality in BERDecoder
    byte[] header = new byte[6];
    int offset = 0;
    header[offset++] = (byte) readByte(); // type
    int len = readByte(); // length 0
    header[offset++] = (byte) len;
    if ((len & 0x80) != 0)
      {
        int lsize = len - 0x80;
        if (lsize > 4)
          {
            throw new BERException("Data too long: " + lsize);
          }
        len = 0;
        for (int i = 0; i < lsize; i++)
          {
            int c = readByte();
            header[offset++] = (byte) c;
            len = (len << 8) + c;
          }
      }
    // Allocate message array
    byte[] message = new byte[offset + len];
    System.arraycopy(header, 0, message, 0, offset);
    if (len == 0)
      {
        return message;
      }
    header = null;
    // Read message content
    do
      {
        int l = in.read(message, offset, len);
        if (l == -1)
          {
            throw new IOException("EOF");
          }
        offset += l;
        len -= l;
      }
    while (len > 0);
    return message;
  }

  /**
   * Read a single byte.
   */
  int readByte()
    throws IOException
  {
    int ret = in.read();
    if (ret == -1)
      {
        throw new IOException("EOF");
      }
    return ret & 0xff;
  }
  
}

