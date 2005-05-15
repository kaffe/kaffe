/* FunctionalORB.java --
   Copyright (C) 2005 Free Software Foundation, Inc.

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
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

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
exception statement from your version. */


package gnu.CORBA;

import gnu.CORBA.CDR.cdrBufInput;
import gnu.CORBA.CDR.cdrBufOutput;
import gnu.CORBA.GIOP.ErrorMessage;
import gnu.CORBA.GIOP.MessageHeader;
import gnu.CORBA.GIOP.ReplyHeader;
import gnu.CORBA.GIOP.RequestHeader;
import gnu.CORBA.NamingService.NamingServiceTransient;

import org.omg.CORBA.BAD_INV_ORDER;
import org.omg.CORBA.BAD_OPERATION;
import org.omg.CORBA.BAD_PARAM;
import org.omg.CORBA.CompletionStatus;
import org.omg.CORBA.MARSHAL;
import org.omg.CORBA.OBJECT_NOT_EXIST;
import org.omg.CORBA.ORBPackage.InvalidName;
import org.omg.CORBA.SystemException;
import org.omg.CORBA.UNKNOWN;
import org.omg.CORBA.portable.Delegate;
import org.omg.CORBA.portable.InvokeHandler;
import org.omg.CORBA.portable.ObjectImpl;
import org.omg.CosNaming.NamingContextExt;
import org.omg.CosNaming.NamingContextExtHelper;

import java.applet.Applet;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

import java.util.Enumeration;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;
import java.util.StringTokenizer;
import java.util.TreeMap;

/**
 * The ORB implementation, capable to handle remote invocations on the
 * registered object.
 *
 * @author Audrius Meskauskas (AudriusA@Bioinformatics.org)
 */
public class Functional_ORB
  extends Restricted_ORB
{
  /**
   * The property of port, on that this ORB is listening for requests from clients.
   * This class supports one port per ORB only.
   */
  public static final String LISTEN_ON = "gnu.classpath.CORBA.ListenOn";

  /**
   * The property, defining the IOR of the intial reference to resolve.
   */
  public static final String REFERENCE = "org.omg.CORBA.ORBInitRef";

  /**
   * The property, defining the port on that the default name service is running.
   */
  public static final String NS_PORT = "org.omg.CORBA.ORBInitialPort";

  /**
   * The property, defining the host on that the default name service is running.
   */
  public static final String NS_HOST = "org.omg.CORBA.ORBInitialHost";

  /**
   * The string, defining the naming service initial reference.
   */
  public static final String NAME_SERVICE = "NameService";

  /**
   * The address of the local host.
   */
  public final String LOCAL_HOST;

  /**
   * The map of the already conncted objects.
   */
  protected final Connected_objects connected_objects = new Connected_objects();

  /**
   * The maximal CORBA version, supported by this ORB. The default value
   * 0 means that the ORB will not check the request version while trying
   * to respond.
   */
  protected Version max_version;

  /**
   * Setting this value to false causes the ORB to shutdown after the
   * latest serving operation is complete.
   */
  protected boolean running;

  /**
   * The map of the initial references.
   */
  private Map initial_references = new TreeMap();

  /**
   * The host, on that the name service is expected to be running.
   */
  private String ns_host;

  /**
   * The port, under that the ORB is listening for remote requests.
   * In this implementation, all objects, served by this ORB, share
   * the same port, 1126.
   */
  private int Port = 1126;

  /**
   * The port, on that the name service is expected to be running.
   */
  private int ns_port = 900;

  /**
   * Create the instance of the Functional ORB.
   */
  public Functional_ORB()
  {
    try
      {
        LOCAL_HOST = ns_host = InetAddress.getLocalHost().getHostAddress();
      }
    catch (UnknownHostException ex)
      {
        throw new BAD_OPERATION("Unable to resolve the local host address.");
      }
  }

  /**
  * If the max version is assigned, the orb replies with the error
  * message if the request version is above the supported 1.2 version.
  * This behavior is recommended by OMG, but not all implementations
  * respond that error message by re-sending the request, encoded in the older
  * version.
  */
  public void setMaxVersion(Version max_supported)
  {
    max_version = max_supported;
  }

  /**
   * Get the maximal supported GIOP version or null if the version is
   * not checked.
   */
  public Version getMaxVersion()
  {
    return max_version;
  }

  /**
   * Set the port, on that the server is listening for the client requests.
   * In this implementation, the server is listening at only one port,
   * the default value being 1126.
   *
   * @param a_Port a port, on that the server is listening for requests.
   *
   * @throws BAD_INV_ORDER if the server has already been started. The port
   * can only be changed when the server is not yet started.
   */
  public void setPort(int a_Port)
  {
    if (running)
      throw new BAD_INV_ORDER("The server is running");
    this.Port = a_Port;
  }

  /**
   * Get the port, on that the server is listening for the client requests.
   * In this implementation, the server is listening at only one port,
   * the default value being 1126.
   *
   * @return the port.
   */
  public int getPort()
  {
    return Port;
  }

  /**
   * Connect the given CORBA object to this ORB. After the object is
   * connected, it starts receiving remote invocations via this ORB.
   *
   * @param object the object, must implement the {@link InvokeHandler})
   * interface.
   *
   * @throws BAD_PARAM if the object does not implement the
   * {@link InvokeHandler}).
   */
  public void connect(org.omg.CORBA.Object object)
  {
    connected_objects.add(object);

    IOR ior = createIOR(object, connected_objects.getKey(object));
    prepareObject(object, ior);
  }

  /**
   * Connect the given CORBA object to this ORB, explicitly specifying
   * the object key.
   *
   * @param object the object, must implement the {@link InvokeHandler})
   * interface.
   * @param key the object key, usually used to identify the object from
   *  remote side.
   *
   * @throws BAD_PARAM if the object does not implement the
   * {@link InvokeHandler}).
   */
  public void connect(org.omg.CORBA.Object object, byte[] key)
  {
    prepareObject(object, createIOR(object, key));
    connected_objects.add(key, object);
  }

  /**
   * Destroy this server, releasing the occupied resources.
   */
  public void destroy()
  {
    super.destroy();
  }

  /**
   * Disconnect the given CORBA object from this ORB. The object will be
   * no longer receiving the remote invocations. In response to the
   * remote invocation on this object, the ORB will send the
   * exception {@link OBJECT_NOT_EXIST}. The object, however, is not
   * destroyed and can receive the local invocations.

   * @param object the object to disconnect.
   */
  public void disconnect(org.omg.CORBA.Object object)
  {
    connected_objects.remove(object);
  }

  /**
   * Find the local object, connected to this ORB.
   *
   * @param ior the ior of the potentially local object.
   *
   * @return the local object, represented by the given IOR,
   * or null if this is not a local connected object.
   */
  public org.omg.CORBA.Object find_local_object(IOR ior)
  {
    // Must be the same host.
    if (!ior.Internet.host.equals(LOCAL_HOST))
      return null;

    // Must be the same port.
    if (ior.Internet.port != Port)
      return null;

    return find_connected_object(ior.key);
  }

  /**
   * List the initially available CORBA objects (services).
   *
   * @return a list of services.
   *
   * @see resolve_initial_references(String)
   */
  public String[] list_initial_services()
  {
    String[] refs = new String[ initial_references.size() ];
    int p = 0;

    Iterator iter = initial_references.keySet().iterator();
    while (iter.hasNext())
      refs [ p++ ] = (String) iter.next();

    return refs;
  }

  /**
   * Get the IOR reference string for the given object.
   * The string embeds information about the object
   * repository Id, its access key and the server internet
   * address and port. With this information, the object
   * can be found by another ORB, possibly located on remote
   * computer.
   *
   * @param the CORBA object
   * @return the object IOR representation.
   *
   * @throws BAD_PARAM if the object has not been previously
   * connected to this ORB.
   * @throws BAD_OPERATION in the unlikely case if the local host
   * address cannot be resolved.
   *
   * @see string_to_object(String)
   */
  public String object_to_string(org.omg.CORBA.Object forObject)
  {
    // Handle the case when the object is known, but not local.
    if (forObject instanceof ObjectImpl)
      {
        Delegate delegate = ((ObjectImpl) forObject)._get_delegate();
        if (delegate instanceof Simple_delegate)
          return ((Simple_delegate) delegate).getIor().toStringifiedReference();
      }

    // Handle the case when the object is local.
    byte[] key = connected_objects.getKey(forObject);

    if (key == null)
      throw new BAD_PARAM("The object " + forObject +
                          " has not been previously connected to this ORB"
                         );

    IOR ior = createIOR(forObject, key);

    return ior.toStringifiedReference();
  }

  /**
   * Find and return the easily accessible CORBA object, addressed
   * by name.
   *
   * @param name the object name.
   * @return the object
   *
   * @throws org.omg.CORBA.ORBPackage.InvalidName if the given name
   * is not associated with the known object.
   */
  public org.omg.CORBA.Object resolve_initial_references(String name)
                                                  throws InvalidName
  {
    org.omg.CORBA.Object object = null;
    try
      {
        object = (org.omg.CORBA.Object) initial_references.get(name);
        if (object == null && name.equals(NAME_SERVICE))
          {
            object = getDefaultNameService();
            if (object != null)
              initial_references.put(NAME_SERVICE, object);
          }
      }
    catch (Exception ex)
      {
        throw new InvalidName(name + ":" + ex.getMessage());
      }
    if (object != null)
      return object;
    else
      throw new InvalidName("Not found: '" + name + "'");
  }

  /**
   * Start the ORBs main working cycle
   * (receive invocation - invoke on the local object - send response -
   *  wait for another invocation).
   *
   * The method only returns after calling {@link #shutdown(boolean)}.
   */
  public void run()
  {
    try
      {
        ServerSocket service = new ServerSocket(Port);
        running = true;
        while (running)
          {
            serve(service);
          }
      }
    catch (IOException ex)
      {
        throw new BAD_OPERATION("Unable to open the server socket.");
      }
  }

  /**
   * Shutdown the ORB server.
   *
   * @param wait_for_completion if true, the current thread is
   * suspended untile the shutdown process is complete.
   */
  public void shutdown(boolean wait_for_completion)
  {
    super.shutdown(wait_for_completion);
  }

  /**
   * Find and return the CORBA object, addressed by the given
   * IOR string representation. The object can (an usually is)
   * located on a remote computer, possibly running a different
   * (not necessary java) CORBA implementation.
   *
   * @param ior the object IOR representation string.
   *
   * @return the found CORBA object.
   * @see object_to_string(org.omg.CORBA.Object)
   */
  public org.omg.CORBA.Object string_to_object(String an_ior)
  {
    IOR ior = IOR.parse(an_ior);
    org.omg.CORBA.Object object = find_local_object(ior);
    if (object == null)
      {
        ObjectImpl impl = stubFinder.search(this, ior);
        try
          {
            if (impl._get_delegate() == null)
              {
                impl._set_delegate(new IOR_Delegate(this, ior));
              }
          }
        catch (BAD_OPERATION ex)
          {
            // Some colaborants may throw this exception
            // in response to the attempt to get the unset delegate.
            impl._set_delegate(new IOR_Delegate(this, ior));
          }

        object = impl;
        connected_objects.add(ior.key, impl);
      }
    return object;
  }

  /**
   * Get the default naming service for the case when there no
   * NameService entries.
   */
  protected org.omg.CORBA.Object getDefaultNameService()
  {
    if (initial_references.containsKey(NAME_SERVICE))
      {
        return (org.omg.CORBA.Object) initial_references.get(NAME_SERVICE);
      }

    IOR ior = new IOR();
    ior.Id = NamingContextExtHelper.id();
    ior.Internet.host = ns_host;
    ior.Internet.port = ns_port;
    ior.key = NamingServiceTransient.getDefaultKey();

    IOR_contructed_object iorc = new IOR_contructed_object(this, ior);
    NamingContextExt namer = NamingContextExtHelper.narrow(iorc);
    initial_references.put(NAME_SERVICE, namer);
    return namer;
  }

  /**
   * Find and return the object, that must be previously connected
   * to this ORB. Return null if no such object is available.
   *
   * @param key the object key.
   *
   * @return the connected object, null if none.
   */
  protected org.omg.CORBA.Object find_connected_object(byte[] key)
  {
    return connected_objects.get(key);
  }

  /**
   * Set the ORB parameters. This method is normally called from
   * {@link #init(Applet, Properties)}.
   *
   * @param app the current applet.
   *
   * @param props application specific properties, passed as the second
   * parameter in {@link #init(Applet, Properties)}.
   * Can be <code>null</code>.
   */
  protected void set_parameters(Applet app, Properties props)
  {
    useProperties(props);

    String[][] para = app.getParameterInfo();
    if (para != null)
      {
        for (int i = 0; i < para.length; i++)
          {
            if (para [ i ] [ 0 ].equals(LISTEN_ON))
              Port = Integer.parseInt(para [ i ] [ 1 ]);

            if (para [ i ] [ 0 ].equals(REFERENCE))
              {
                StringTokenizer st = new StringTokenizer(para [ i ] [ 1 ], "=");
                initial_references.put(st.nextToken(),
                                       string_to_object(st.nextToken())
                                      );
              }

            if (para [ i ] [ 0 ].equals(NS_HOST))
              ns_host = para [ i ] [ 1 ];

            try
              {
                if (para [ i ] [ 0 ].equals(NS_PORT))
                  ns_port = Integer.parseInt(para [ i ] [ 1 ]);
              }
            catch (NumberFormatException ex)
              {
                throw new BAD_PARAM("Invalid " + NS_PORT +
                                    "property, unable to parse '" +
                                    props.getProperty(NS_PORT) + "'"
                                   );
              }
          }
      }
  }

  /**
   * Set the ORB parameters. This method is normally called from
   * {@link #init(String[], Properties)}.
   *
   * @param para the parameters, that were passed as the parameters
   * to the  <code>main(String[] args)</code> method of the current standalone
   * application.
   *
   * @param props application specific properties that were passed
   * as a second parameter in {@link init(String[], Properties)}).
   * Can be <code>null</code>.
   */
  protected void set_parameters(String[] para, Properties props)
  {
    if (para.length > 1)
      for (int i = 0; i < para.length - 1; i++)
        {
          if (para [ i ].endsWith("ListenOn"))
            Port = Integer.parseInt(para [ i + 1 ]);

          if (para [ i ].endsWith("ORBInitRef"))
            {
              StringTokenizer st = new StringTokenizer(para [ i + 1 ], "=");
              initial_references.put(st.nextToken(),
                                     string_to_object(st.nextToken())
                                    );
            }

          if (para [ i ].endsWith("ORBInitialHost"))
            ns_host = para [ i + 1 ];

          try
            {
              if (para [ i ].endsWith("ORBInitialPort"))
                ns_port = Integer.parseInt(para [ i + 1 ]);
            }
          catch (NumberFormatException ex)
            {
              throw new BAD_PARAM("Invalid " + para [ i ] +
                                  "parameter, unable to parse '" +
                                  props.getProperty(para [ i + 1 ]) + "'"
                                 );
            }
        }

    useProperties(props);
  }

  private IOR createIOR(org.omg.CORBA.Object forObject, byte[] key)
                 throws BAD_OPERATION
  {
    IOR ior = new IOR();
    ior.key = key;
    ior.Internet.port = Port;

    if (forObject instanceof ObjectImpl)
      {
        ObjectImpl imp = (ObjectImpl) forObject;
        if (imp._ids().length > 0)
          ior.Id = imp._ids() [ 0 ];
      }
    if (ior.Id == null)
      ior.Id = forObject.getClass().getName();

    try
      {
        ior.Internet.host = InetAddress.getLocalHost().getHostAddress();
        ior.Internet.port = Port;
      }
    catch (UnknownHostException ex)
      {
        throw new BAD_OPERATION("Cannot resolve the local host address");
      }
    return ior;
  }

  /**
   * Prepare object for connecting it to this ORB.
   *
   * @param object the object being connected.
   *
   * @throws BAD_PARAM if the object does not implement the
   * {@link InvokeHandler}).
   */
  private void prepareObject(org.omg.CORBA.Object object, IOR ior)
                      throws BAD_PARAM
  {
    if (!(object instanceof InvokeHandler))
      throw new BAD_PARAM(object.getClass().getName() +
                          " does not implement InvokeHandler. "
                         );

    // If no delegate is set, set the default delegate.
    if (object instanceof ObjectImpl)
      {
        ObjectImpl impl = (ObjectImpl) object;
        try
          {
            if (impl._get_delegate() == null)
              {
                impl._set_delegate(new Simple_delegate(this, ior));
              }
          }
        catch (BAD_OPERATION ex)
          {
            // Some colaborants may throw this exception.
            impl._set_delegate(new Simple_delegate(this, ior));
          }
      }
  }

  /**
   * Write the response message.
   *
   * @param net_out the stream to write response into
   * @param msh_request the request message header
   * @param rh_request the request header
   * @param handler the invocation handler that has been used to
   * invoke the operation
   * @param sysEx the system exception, thrown during the invocation,
   * null if none.
   *
   * @throws IOException
   */
  private void respond_to_client(OutputStream net_out,
                                 MessageHeader msh_request,
                                 RequestHeader rh_request,
                                 bufferedResponseHandler handler,
                                 SystemException sysEx
                                )
                          throws IOException
  {
    // Set the reply header properties.
    ReplyHeader reply = handler.reply_header;

    if (sysEx != null)
      reply.reply_status = reply.SYSTEM_EXCEPTION;
    else if (handler.isExceptionReply())
      reply.reply_status = reply.USER_EXCEPTION;
    else
      reply.reply_status = reply.NO_EXCEPTION;

    reply.request_id = rh_request.request_id;

    cdrBufOutput out = new cdrBufOutput(50 + handler.getBuffer().buffer.size());
    out.setOrb(this);

    out.setOffset(msh_request.getHeaderSize());

    reply.write(out);

    // Write the reply data from the handler.
    handler.getBuffer().buffer.writeTo(out);

    MessageHeader msh_reply = new MessageHeader();
    msh_reply.version = msh_request.version;
    msh_reply.message_type = msh_reply.REPLY;
    msh_reply.message_size = out.buffer.size();

    // Write the reply.
    msh_reply.write(net_out);
    out.buffer.writeTo(net_out);
    net_out.flush();
  }

  /**
   * Contains a single servicing step.
   *
   * @param serverSocket the ORB server socket.
   *
   * @throws MARSHAL
   * @throws IOException
   */
  private void serve(ServerSocket serverSocket)
              throws MARSHAL, IOException
  {
    Socket service = null;
    try
      {
        service = serverSocket.accept();
        service.setKeepAlive(true);

        InputStream in = service.getInputStream();

        MessageHeader msh_request = new MessageHeader();
        msh_request.read(in);

        if (max_version != null)
          if (!msh_request.version.until_inclusive(max_version.major,
                                                   max_version.minor
                                                  )
             )
            {
              OutputStream out = service.getOutputStream();
              new ErrorMessage(max_version).write(out);
              service.close();
              return;
            }

        byte[] r = new byte[ msh_request.message_size ];

        int n = 0;

        reading:
        while (n < r.length)
          {
            n = in.read(r, n, r.length - n);
          }

        if (msh_request.message_type == msh_request.REQUEST)
          {
            RequestHeader rh_request;

            cdrBufInput cin = new cdrBufInput(r);
            cin.setOrb(this);
            cin.setVersion(msh_request.version);
            cin.setOffset(msh_request.getHeaderSize());

            rh_request = msh_request.create_request_header();

            // Read header and auto set the charset.
            rh_request.read(cin);

            // in 1.2 and higher, align the current position at
            // 8 octet boundary.
            if (msh_request.version.since_inclusive(1, 2))
              cin.align(8);

            // find the target object.
            InvokeHandler target =
              (InvokeHandler) find_connected_object(rh_request.object_key);

            // Prepare the reply header. This must be done in advance,
            // as the size must be known for handler to set alignments
            // correctly.
            ReplyHeader rh_reply = msh_request.create_reply_header();

            // TODO log errors about not existing objects and methods.
            bufferedResponseHandler handler =
              new bufferedResponseHandler(this, msh_request, rh_reply);

            SystemException sysEx = null;

            try
              {
                if (target == null)
                  throw new OBJECT_NOT_EXIST();
                target._invoke(rh_request.operation, cin, handler);
              }
            catch (SystemException ex)
              {
                sysEx = ex;

                org.omg.CORBA.portable.OutputStream ech =
                  handler.createExceptionReply();
                ObjectCreator.writeSystemException(ech, ex);
              }
            catch (Exception except)
              {
                sysEx =
                  new UNKNOWN("Unknown", 2, CompletionStatus.COMPLETED_MAYBE);

                org.omg.CORBA.portable.OutputStream ech =
                  handler.createExceptionReply();

                ObjectCreator.writeSystemException(ech, sysEx);
              }

            // Write the response.
            if (rh_request.isResponseExpected())
              {
                respond_to_client(service.getOutputStream(), msh_request,
                                  rh_request, handler, sysEx
                                 );
              }
          }
        else
          ;

        // TODO log error: "Not a request message."
      }
    finally
      {
        if (service != null && !service.isClosed())
          {
            service.close();
          }
      }
  }

  private void useProperties(Properties props)
  {
    if (props != null)
      {
        if (props.containsKey(LISTEN_ON))
          Port = Integer.parseInt(props.getProperty(LISTEN_ON));

        if (props.containsKey(NS_HOST))
          ns_host = props.getProperty(NS_HOST);

        try
          {
            if (props.containsKey(NS_PORT))
              ns_port = Integer.parseInt(props.getProperty(NS_PORT));
          }
        catch (NumberFormatException ex)
          {
            throw new BAD_PARAM("Invalid " + NS_PORT +
                                "property, unable to parse '" +
                                props.getProperty(NS_PORT) + "'"
                               );
          }

        Enumeration en = props.elements();
        while (en.hasMoreElements())
          {
            String item = (String) en.nextElement();
            if (item.equals(REFERENCE))
              initial_references.put(item,
                                     string_to_object(props.getProperty(item))
                                    );
          }
      }
  }
}
