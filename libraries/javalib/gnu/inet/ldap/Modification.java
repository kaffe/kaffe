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
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
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
  public Modification (int operation, String type, Set values)
  {
    super (type, values);
    if (operation < ADD || operation > REPLACE)
      {
        throw new IllegalArgumentException ("unknown operation");
      }
    this.operation = operation;
  }

  /**
   * @see #operation
   */
  public int getOperation ()
  {
    return operation;
  }

}
