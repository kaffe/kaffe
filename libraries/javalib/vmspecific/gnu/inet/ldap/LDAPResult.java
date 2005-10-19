/*
 * LDAPResult.java
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

/**
 * An LDAP result structure.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class LDAPResult
{

  public static final int SUCCESS = 0;
  public static final int OPERATIONS_ERROR = 1;
  public static final int PROTOCOL_ERROR = 2;
  public static final int TIME_LIMIT_EXCEEDED = 3;
  public static final int SIZE_LIMIT_EXCEEDED = 4;
  public static final int COMPARE_FALSE = 5;
  public static final int COMPARE_TRUE = 6;
  public static final int AUTH_METHOD_NOT_SUPPORTED = 7;
  public static final int STRONG_AUTH_REQUIRED = 8;
  public static final int REFERRAL = 10;
  public static final int ADMIN_LIMIT_EXCEEDED = 11;
  public static final int UNAVAILABLE_CRITICAL_EXTENSION = 12;
  public static final int CONFIDENTIALITY_REQUIRED = 13;
  public static final int SASL_BIND_IN_PROGRESS = 14;
  public static final int NO_SUCH_ATTRIBUTE = 16;
  public static final int UNDEFINED_ATTRIBUTE_TYPE = 17;
  public static final int INAPPROPRIATE_MATCHING = 18;
  public static final int CONSTRAINT_VIOLATION = 19;
  public static final int ATTRIBUTE_OR_VALUE_EXISTS = 20;
  public static final int INVALID_ATTRIBUTE_SYNTAX = 21;
  public static final int NO_SUCH_OBJECT = 32;
  public static final int ALIAS_PROBLEM = 33;
  public static final int INVALID_DN_SYNTAX = 34;
  public static final int ALIAS_DEREFERENCING_PROBLEM = 36;
  public static final int INAPPROPRIATE_AUTHENTICATION = 48;
  public static final int INVALID_CREDENTIALS = 49;
  public static final int INSUFFICIENT_ACCESS_RIGHTS = 50;
  public static final int BUSY = 51;
  public static final int UNAVAILABLE = 52;
  public static final int UNWILLING_TO_PERFORM = 53;
  public static final int LOOP_DETECT = 54;
  public static final int NAMING_VIOLATION = 64;
  public static final int OBJECT_CLASS_VIOLATION = 65;
  public static final int NOT_ALLOWED_ON_NON_LEAF = 66;
  public static final int NOT_ALLOWED_ON_RDN = 67;
  public static final int ENTRY_ALREADY_EXISTS = 68;
  public static final int OBJECT_CLASS_MODS_PROHIBITED = 69;
  public static final int AFFECTS_MULTIPLE_DSAS = 71;
  public static final int OTHER = 80;

  /**
   * The result code associated with this result.
   */
  public final int status;

  /**
   * The name of the matching entry.
   */
  public final String matchedDN;

  /**
   * An associated error message.
   */
  public final String errorMessage;

  /**
   * A list of LDAP URLs to refer to if the status is REFERRAL.
   */
  public final String[] referrals;

  protected LDAPResult(int status, String matchedDN, String errorMessage,
                       String[] referrals)
  {
    this.status = status;
    this.matchedDN = matchedDN;
    this.errorMessage = errorMessage;
    this.referrals = referrals;
  }

  /**
   * Debugging.
   */
  public String toString()
  {
    StringBuffer buffer = new StringBuffer(getClass().getName());
    buffer.append('[');
    buffer.append("status=");
    buffer.append(status);
    buffer.append(",matchedDN=");
    buffer.append(matchedDN);
    if (errorMessage != null)
      {
        buffer.append(",errorMessage=");
        buffer.append(errorMessage);
      }
    if (referrals != null)
      {
        buffer.append(",referrals=");
        buffer.append(referrals.toString());
      }
    buffer.append(']');
    return buffer.toString();
  }
  
}

