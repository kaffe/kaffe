/*
 * AttributeValues.java
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
 * An attribute type and set of values to associate with it.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class AttributeValues
{

  /**
   * The attribute type.
   */
  protected final String type;

  /**
   * The values to assign.
   */
  protected final Set values;

  /**
   * Constructor.
   * @param type the attribute type
   * @param values the values to assign
   */
  public AttributeValues (String type, Set values)
  {
    if (type == null)
      {
        throw new NullPointerException ("type");
      }
    this.type = type;
    this.values = values;
  }
  
  /**
   * @see #type
   */
  public String getType ()
  {
    return type;
  }

  /**
   * @see #values
   */
  public Set getValues ()
  {
    return values;
  }
  
}
