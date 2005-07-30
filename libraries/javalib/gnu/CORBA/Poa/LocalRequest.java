/* LocalRequest.java --
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
exception statement from your version. */


package gnu.CORBA.Poa;

import gnu.CORBA.CDR.cdrBufOutput;
import gnu.CORBA.GIOP.MessageHeader;
import gnu.CORBA.Unexpected;
import gnu.CORBA.gnuAny;
import gnu.CORBA.gnuRequest;
import gnu.CORBA.streamReadyHolder;
import gnu.CORBA.streamRequest;

import org.omg.CORBA.ARG_OUT;
import org.omg.CORBA.BAD_INV_ORDER;
import org.omg.CORBA.BAD_OPERATION;
import org.omg.CORBA.Bounds;
import org.omg.CORBA.NamedValue;
import org.omg.CORBA.ORB;
import org.omg.CORBA.UnknownUserException;
import org.omg.CORBA.portable.ApplicationException;
import org.omg.CORBA.portable.InvokeHandler;
import org.omg.CORBA.portable.OutputStream;
import org.omg.CORBA.portable.ResponseHandler;
import org.omg.PortableServer.CurrentOperations;
import org.omg.PortableServer.CurrentPackage.NoContext;
import org.omg.PortableServer.DynamicImplementation;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.Servant;
import org.omg.PortableServer.ServantLocatorPackage.CookieHolder;
import org.omg.PortableServer.portable.Delegate;

import java.io.IOException;

/**
 * Directs the invocation to the locally available servant.
 * The POA servant does not longer implement the CORBA object and
 * cannot be substituted directly.
 *
 * @author Audrius Meskauskas, Lithuania (AudriusA@Bioinformatics.org)
 */
public class LocalRequest
  extends gnuRequest
  implements ResponseHandler, CurrentOperations
{
  /**
   * Used by servant locator, if involved.
   */
  CookieHolder cookie;

  /**
   * The object Id.
   */
  final byte[] Id;

  /**
   * The message header (singleton is sufficient).
   */
  private static final MessageHeader header = new MessageHeader();

  /**
   * True if the stream was obtained by invoking {@link #createExceptionReply()},
   * false otherwise.
   */
  boolean exceptionReply;

  /**
   * The buffer to write into.
   */
  cdrBufOutput buffer;

  /**
   * The responsible POA.
   */
  final gnuPOA poa;

  /**
   * The servant delegate to obtain the handler.
   */
  gnuServantObject object;

  /**
   * Used (reused) with dynamic implementation.
   */
  LocalServerRequest serverRequest;

  /**
   * Create an instance of the local request.
   */
  public LocalRequest(gnuServantObject local_object, gnuPOA a_poa, byte[] an_id)
  {
    Id = an_id;
    poa = a_poa;

    // Instantiate the cookie holder only if required.
    if (poa.servant_locator != null)
      cookie = new CookieHolder();
    object = local_object;
    prepareStream();
  }

  /**
   * Make an invocation and return a stream from where the results
   * can be read and throw ApplicationException, where applicable.
   */
  org.omg.CORBA.portable.InputStream s_invoke(InvokeHandler handler)
                                       throws ApplicationException
  {
    try
      {
        poa.m_orb.currents.put(Thread.currentThread(), this);

        org.omg.CORBA.portable.InputStream input = v_invoke(handler);

        if (!exceptionReply)
          return input;
        else
          {
            input.mark(500);

            String id = input.read_string();
            try
              {
                input.reset();
              }
            catch (IOException ex)
              {
                InternalError ierr = new InternalError();
                ierr.initCause(ex);
                throw ierr;
              }
            throw new ApplicationException(id, input);
          }
      }
    finally
      {
        poa.m_orb.currents.remove(Thread.currentThread());
      }
  }

  /**
   * Make an invocation and return a stream from where the results
   * can be read.
   *
   * @param the invoke handler (can be null, then it is obtained self
   * dependently).
   */
  public org.omg.CORBA.portable.InputStream v_invoke(InvokeHandler handler)
  {
    if (handler == null)
      handler = object.getHandler(operation(), cookie, false);

    cdrBufOutput request_part = new cdrBufOutput();

    request_part.setOrb(orb());

    if (m_args != null && m_args.count() > 0)
      {
        write_parameters(header, request_part);

        if (m_parameter_buffer != null)
          throw new BAD_INV_ORDER("Please either add parameters or " +
                                  "write them into stream, but not both " +
                                  "at once."
                                 );
      }

    if (m_parameter_buffer != null)
      {
        write_parameter_buffer(header, request_part);
      }

    Servant servant;

    if (handler instanceof Servant)
      servant = (Servant) handler;
    else
      throw new BAD_OPERATION("Unexpected handler type " + handler);

    org.omg.CORBA.portable.InputStream input =
      request_part.create_input_stream();

    // Ensure the servant (handler) has a delegate set.
    servantDelegate sd = null;

    Delegate d = null;

    try
      {
        d = servant._get_delegate();
      }
    catch (Exception ex)
      {
        // In some cases exception is thrown if the delegate is not set.
      }
    if (d instanceof servantDelegate)
      {
        // If the delegate is already set, try to reuse the existing
        // instance.
        sd = (servantDelegate) d;
        if (sd.object != object)
          sd = new servantDelegate(servant, poa, Id);
      }
    else
      sd = new servantDelegate(servant, poa, Id);
    servant._set_delegate(sd);

    try
      {
        ORB o = orb();
        if (o instanceof ORB_1_4)
          {
            ((ORB_1_4) o).currents.put(Thread.currentThread(), this);
          }

        handler._invoke(m_operation, input, this);
      }
    finally
      {
        ORB o = orb();
        if (o instanceof ORB_1_4)
          ((ORB_1_4) o).currents.remove(Thread.currentThread());
      }

    if (poa.servant_locator != null)
      poa.servant_locator.postinvoke(object.Id, poa, operation(), cookie.value,
                                     object.getServant()
                                    );

    return buffer.create_input_stream();
  }

  /**
   * Make an invocation and store the result in the fields of this
   * Request. Used with DII only.
   */
  public void invoke()
  {
    InvokeHandler handler = object.getHandler(operation(), cookie, false);

    if (handler instanceof dynImpHandler)
      {
        DynamicImplementation dyn = ((dynImpHandler) handler).servant;
        if (serverRequest == null)
          serverRequest = new LocalServerRequest(this);
        try
          {
            poa.m_orb.currents.put(Thread.currentThread(), this);
            dyn.invoke(serverRequest);
          }
        finally
          {
            poa.m_orb.currents.remove(Thread.currentThread());
          }
      }
    else
      {
        org.omg.CORBA.portable.InputStream input = v_invoke(handler);

        if (!exceptionReply)
          {
            NamedValue arg;

            // Read return value, if set.
            if (m_result != null)
              {
                m_result.value().read_value(input, m_result.value().type());
              }

            // Read returned parameters, if set.
            if (m_args != null)
              for (int i = 0; i < m_args.count(); i++)
                {
                  try
                    {
                      arg = m_args.item(i);

                      // Both ARG_INOUT and ARG_OUT have this binary flag set.
                      if ((arg.flags() & ARG_OUT.value) != 0)
                        {
                          arg.value().read_value(input, arg.value().type());
                        }
                    }
                  catch (Bounds ex)
                    {
                      Unexpected.error(ex);
                    }
                }
          }
        else // User exception reply
          {
            // Prepare an Any that will hold the exception.
            gnuAny exc = new gnuAny();

            exc.insert_Streamable(new streamReadyHolder(input));

            UnknownUserException unuex = new UnknownUserException(exc);
            m_environment.exception(unuex);
          }
      }
  }

  /**
   * Get an output stream for providing details about the exception.
   * Before returning the stream, the handler automatically writes
   * the message header and the reply about exception header,
   * but not the message header.
   *
   * @return the stream to write exception details into.
   */
  public OutputStream createExceptionReply()
  {
    exceptionReply = true;
    prepareStream();
    return buffer;
  }

  /**
   * Get an output stream for writing a regular reply (not an exception).
   *
   * Before returning the stream, the handler automatically writes
   * the regular reply header, but not the message header.
   *
   * @return the output stream for writing a regular reply.
   */
  public OutputStream createReply()
  {
    exceptionReply = false;
    prepareStream();
    return buffer;
  }

  /**
   * Get the buffer, normally containing the written reply.
   * The reply includes the reply header (or the exception header)
   * but does not include the message header.
   *
   * The stream buffer can also be empty if no data have been written
   * into streams, returned by {@link #createReply()} or
   * {@link #createExceptionReply()}.
   *
   * @return the CDR output stream, containing the written output.
   */
  cdrBufOutput getBuffer()
  {
    return buffer;
  }

  /**
   * True if the stream was obtained by invoking
   * {@link #createExceptionReply()}, false otherwise
   * (usually no-exception reply).
   */
  boolean isExceptionReply()
  {
    return exceptionReply;
  }

  /**
   * Compute the header offset, set the correct version number and codeset.
   */
  private void prepareStream()
  {
    buffer = new cdrBufOutput();
    buffer.setOrb(orb());
  }

  /**
   * Get the parameter stream, where the invocation arguments should
   * be written if they are written into the stream directly.
   */
  public streamRequest getParameterStream()
  {
    m_parameter_buffer = new streamRequest();
    m_parameter_buffer.request = this;
    m_parameter_buffer.setOrb(poa.orb());
    return m_parameter_buffer;
  }

  public byte[] get_object_id()
                       throws NoContext
  {
    return Id;
  }

  public POA get_POA()
              throws NoContext
  {
    return poa;
  }
}