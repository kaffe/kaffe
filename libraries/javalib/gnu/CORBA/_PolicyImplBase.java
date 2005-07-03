/* _PolicyImplBase.java --
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


package gnu.CORBA;

import org.omg.CORBA.BAD_OPERATION;
import org.omg.CORBA.CompletionStatus;
import org.omg.CORBA.PolicyHelper;
import org.omg.CORBA.PolicyOperations;
import org.omg.CORBA.portable.InputStream;
import org.omg.CORBA.portable.InvokeHandler;
import org.omg.CORBA.portable.ObjectImpl;
import org.omg.CORBA.portable.OutputStream;
import org.omg.CORBA.portable.ResponseHandler;

/**
 * The server side implementatin base for the {@link Policy}.
 *
 * @specnote The java 1.4 API does not define the server side policy
 * implementation base, but it defines the policy client side stub.
 * As these two classes always work together, and even no separate testing is
 * possible, the required implementation base is provided in gnu.CORBA
 * namespace. Sun will probably include they base in the future java APIs.
 *
 * @author Audrius Meskauskas, Lithuania (AudriusA@Bioinformatics.org)
 */
public abstract class _PolicyImplBase
  extends ObjectImpl
  implements PolicyOperations, InvokeHandler
{
  /**
   * Use serialVersionUID for interoperability.
   */
  private static final long serialVersionUID = 1;

  /**
   * The binding interator repository ids.
   */
  private static String[] ids = { PolicyHelper.id() };

  /**
   * Return the list of repository ids.
   */
  public String[] _ids()
  {
    return ids;
  }

  /**
   * Call the required method.
   */
  public OutputStream _invoke(String method, InputStream input,
                              ResponseHandler rh
                             )
  {
    OutputStream output = null;

    if (method.equals("destroy"))
      {
        // The "destroy" has been invoked.
        destroy();
        output = rh.createReply();
      }
    else if (method.equals("copy"))
      {
        // The "copy" has been invoked.
        org.omg.CORBA.Object returns = copy();
        output = rh.createReply();
        output.write_Object(returns);
      }
    else if (method.equals("policy_type"))
      {
        // The "policy_type" has been invoked.
        int returns = policy_type();
        output = rh.createReply();
        output.write_long(returns);
      }
    else
      throw new BAD_OPERATION(method, 0, CompletionStatus.COMPLETED_MAYBE);

    return output;
  }
}