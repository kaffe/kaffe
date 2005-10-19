/*
 * ResultHandler.java
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

import java.util.List;
import java.util.Map;

/**
 * Callback handler for receiving notification of search results.
 * The application must pass an implementation of this interface into the
 * <code>LDAPConnection.search</code> method. Search responses received
 * during the execution of the method result in calls to the methods defined
 * in this interface.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public interface ResultHandler
{

  /**
   * Receive an LDAP SearchResultEntry response.
   * The attributes map provides a mapping of attribute names to values. In
   * the case where <code>typesOnly</code> was <code>true</code>, the value
   * for each attribute will be null. Otherwise it will be a Set of
   * attribute values, which may be of the following types:
   * <ul>
   * <li>java.lang.String</li>
   * <li>java.lang.Integer</li>
   * <li>java.lang.Double</li>
   * <li>java.lang.Boolean</li>
   * <li>byte[]</li>
   * <ul>
   * @param name the object name DN
   * @param attributes a map of attribute names to values
   */
  void searchResultEntry(String name, Map attributes);

  /**
   * Receive an LDAP SearchResultReference response.
   * The argument to this function is a sequence of LDAP URLs, one for each
   * entry not explored by the server during the search.
   * @param urls the list of LDAP URLs
   */
  void searchResultReference(List urls);
  
}

