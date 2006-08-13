/*
 * Modification.java
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

import java.util.Set;

/**
 * An individual modification of an object's attributes.
 * 
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public final class Modification
  extends AttributeValues
{

  /**
   * Add the specified values to the attribute, creating the attribute if
   * necessary.
   */
  public static final int ADD = 0;

  /**
   * Delete the specified values from the given attribute, removing the
   * entire attribute if no values are listed, or if all current values of
   * the attribute are listed.
   */ 
  public static final int DELETE = 1;

  /**
   * Replace all existing values of the given attribute with the new values,
   * creating the attribute if it did not exist. A replace with no value
   * deletes the entire attribute if it exists, and is ignored otherwise.
   */
  public static final int REPLACE = 2;

  /**
   * The operation specified by this modification.
   * One of: ADD, DELETE, or REPLACE
   */
  protected final int operation;

  /**
   * Constructor.
   * @param operation the operation
   * @param type the attribute type
   * @param values the values to assign
   */
  public Modification(int operation, String type, Set values)
  {
    super(type, values);
    if (operation < ADD || operation > REPLACE)
      {
        throw new IllegalArgumentException("unknown operation");
      }
    this.operation = operation;
  }

  /**
   * @see #operation
   */
  public int getOperation()
  {
    return operation;
  }

}

