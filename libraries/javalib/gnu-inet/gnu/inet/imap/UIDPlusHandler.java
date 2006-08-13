/*
 * UIDPlusHandler.java
 * Copyright (C) 2005 The Free Software Foundation
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

package gnu.inet.imap;

/**
 * Callback interface for receiving APPENDUID and COPYUID responses.
 * See RFC 2359 for details.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public interface UIDPlusHandler
{

  /**
   * Notification of an APPENDUID response.
   * Called on a successful APPEND to a server that supports the UIDPLUS
   * extension.
   * @param uidvalidity the UIDVALIDITY of the destination mailbox
   * @param uid the UID assigned to the appended message.
   */
  void appenduid(long uidvalidity, long uid);

  /**
   * Notification of a COPYUID response.
   * Called on a successful COPY to a server that supports the UIDPLUS
   * extension.
   * If more than one message is copied, this method will be called multiple
   * times, once for each message copied.
   * @param uidvalidity the UIDVALIDITY of the destination mailbox
   * @param oldUID the UID of the message in the source mailbox
   * @param newUID the UID of the corresponding message in the target
   * mailbox
   */
  void copyuid(long uidvalidity, long oldUID, long newUID);
  
}

