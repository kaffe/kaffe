/*
 * $Id: IMAPConstants.java,v 1.5 2004/10/30 12:14:16 robilad Exp $
 * Copyright (C) 2003,2004 The Free Software Foundation
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

package gnu.inet.imap;

/**
 * IMAP4rev1 string constants.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.5 $ $Date: 2004/10/30 12:14:16 $
 */
public interface IMAPConstants
{

  // Client commands
  public static final String CAPABILITY = "CAPABILITY".intern ();
  public static final String NOOP = "NOOP".intern ();
  public static final String AUTHENTICATE = "AUTHENTICATE".intern ();
  public static final String LOGIN = "LOGIN".intern ();
  public static final String LOGOUT = "LOGOUT".intern ();
  public static final String SELECT = "SELECT".intern ();
  public static final String EXAMINE = "EXAMINE".intern ();
  public static final String CREATE = "CREATE".intern ();
  public static final String DELETE = "DELETE".intern ();
  public static final String RENAME = "RENAME".intern ();
  public static final String SUBSCRIBE = "SUBSCRIBE".intern ();
  public static final String UNSUBSCRIBE = "UNSUBSCRIBE".intern ();
  public static final String LIST = "LIST".intern ();
  public static final String LSUB = "LSUB".intern ();
  public static final String STATUS = "STATUS".intern ();
  public static final String APPEND = "APPEND".intern ();
  public static final String CHECK = "CHECK".intern ();
  public static final String CLOSE = "CLOSE".intern ();
  public static final String EXPUNGE = "EXPUNGE".intern ();
  public static final String SEARCH = "SEARCH".intern ();
  public static final String FETCH = "FETCH".intern ();
  public static final String STORE = "STORE".intern ();
  public static final String COPY = "COPY".intern ();
  public static final String UID = "UID".intern ();
  public static final String STARTTLS = "STARTTLS".intern ();
  public static final String NAMESPACE = "NAMESPACE".intern ();
  public static final String SETACL = "SETACL".intern ();
  public static final String DELETEACL = "DELETEACL".intern ();
  public static final String GETACL = "GETACL".intern ();
  public static final String LISTRIGHTS = "LISTRIGHTS".intern ();
  public static final String MYRIGHTS = "MYRIGHTS".intern ();
  public static final String SETQUOTA = "SETQUOTA".intern ();
  public static final String GETQUOTA = "GETQUOTA".intern ();
  public static final String GETQUOTAROOT = "GETQUOTAROOT".intern ();

  // Server responses
  public static final String OK = "OK".intern ();
  public static final String NO = "NO".intern ();
  public static final String BAD = "BAD".intern ();
  public static final String PREAUTH = "PREAUTH".intern ();
  public static final String BYE = "BYE".intern ();

  // Response codes
  public static final String ALERT = "ALERT".intern ();
  public static final String NEWNAME = "NEWNAME".intern ();
  public static final String PARSE = "PARSE".intern ();
  public static final String PERMANENTFLAGS = "PERMANENTFLAGS".intern ();
  public static final String READ_ONLY = "READ-ONLY".intern ();
  public static final String READ_WRITE = "READ-WRITE".intern ();
  public static final String TRYCREATE = "TRYCREATE".intern ();
  public static final String UIDVALIDITY = "UIDVALIDITY".intern ();
  public static final String UNSEEN = "UNSEEN".intern ();
  public static final String ACL = "ACL".intern ();
  public static final String QUOTA = "QUOTA".intern ();
  public static final String QUOTAROOT = "QUOTAROOT".intern ();

  // Select responses
  public static final String FLAGS = "FLAGS".intern ();
  public static final String EXISTS = "EXISTS".intern ();
  public static final String RECENT = "RECENT".intern ();
  public static final String FETCH_FLAGS = "FETCH FLAGS".intern ();

  // Status items
  public static final String MESSAGES = "MESSAGES".intern ();
  public static final String UIDNEXT = "UIDNEXT".intern ();

  // List responses
  public static final String LIST_NOINFERIORS = "\\Noinferiors".intern ();
  public static final String LIST_NOSELECT = "\\Noselect".intern ();
  public static final String LIST_MARKED = "\\Marked".intern ();
  public static final String LIST_UNMARKED = "\\Unmarked".intern ();

  // Flags
  public static final String FLAG_SEEN = "\\Seen".intern ();
  public static final String FLAG_ANSWERED = "\\Answered".intern ();
  public static final String FLAG_FLAGGED = "\\Flagged".intern ();
  public static final String FLAG_DELETED = "\\Deleted".intern ();
  public static final String FLAG_DRAFT = "\\Draft".intern ();
  public static final String FLAG_RECENT = "\\Recent".intern ();

  // Fetch data items
  public static final String BODY = "BODY".intern ();
  public static final String BODY_PEEK = "BODY.PEEK".intern ();
  public static final String HEADER = "HEADER".intern ();
  public static final String HEADER_FIELDS = "HEADER.FIELDS".intern ();
  public static final String HEADER_FIELDS_NOT = "HEADER.FIELDS.NOT".intern ();
  public static final String BODYSTRUCTURE = "BODYSTRUCTURE".intern ();
  public static final String ENVELOPE = "ENVELOPE".intern ();
  public static final String INTERNALDATE = "INTERNALDATE".intern ();
  public static final String RFC822 = "RFC822".intern ();
  public static final String RFC822_HEADER = "RFC822.HEADER".intern ();
  public static final String RFC822_SIZE = "RFC822.SIZE".intern ();
  public static final String RFC822_TEXT = "RFC822.TEXT".intern ();

  // Search criteria
  public static final String SEARCH_ALL = "ALL";
  public static final String SEARCH_ANSWERED = "ANSWERED";
  public static final String SEARCH_BCC = "BCC";
  public static final String SEARCH_BEFORE = "BEFORE";
  public static final String SEARCH_BODY = "BODY";
  public static final String SEARCH_CC = "CC";
  public static final String SEARCH_DELETED = "DELETED";
  public static final String SEARCH_DRAFT = "DRAFT";
  public static final String SEARCH_FLAGGED = "FLAGGED";
  public static final String SEARCH_FROM = "FROM";
  public static final String SEARCH_HEADER = "HEADER";
  public static final String SEARCH_KEYWORD = "KEYWORD";
  public static final String SEARCH_LARGER = "LARGER";
  public static final String SEARCH_NEW = "NEW";
  public static final String SEARCH_NOT = "NOT";
  public static final String SEARCH_OLD = "OLD";
  public static final String SEARCH_ON = "ON";
  public static final String SEARCH_OR = "OR";
  public static final String SEARCH_RECENT = "RECENT";
  public static final String SEARCH_SEEN = "SEEN";
  public static final String SEARCH_SENTBEFORE = "SENTBEFORE";
  public static final String SEARCH_SENTON = "SENTON";
  public static final String SEARCH_SENTSINCE = "SENTSINCE";
  public static final String SEARCH_SINCE = "SINCE";
  public static final String SEARCH_SMALLER = "SMALLER";
  public static final String SEARCH_SUBJECT = "SUBJECT";
  public static final String SEARCH_TEXT = "TEXT";
  public static final String SEARCH_TO = "TO";
  public static final String SEARCH_UID = "UID";
  public static final String SEARCH_UNANSWERED = "UNANSWERED";
  public static final String SEARCH_UNDELETED = "UNDELETED";
  public static final String SEARCH_UNDRAFT = "UNDRAFT";
  public static final String SEARCH_UNFLAGGED = "UNFLAGGED";
  public static final String SEARCH_UNKEYWORD = "UNKEYWORD";
  public static final String SEARCH_UNSEEN = "UNSEEN";

  // NIL
  public static final String NIL = "NIL".intern ();

  // ACL rights
  public static final int RIGHTS_LOOKUP = 0x0001;
  public static final int RIGHTS_READ = 0x0002;
  public static final int RIGHTS_SEEN = 0x0004;
  public static final int RIGHTS_WRITE = 0x0008;
  public static final int RIGHTS_INSERT = 0x0010;
  public static final int RIGHTS_POST = 0x0020;
  public static final int RIGHTS_CREATE = 0x0040;
  public static final int RIGHTS_DELETE = 0x0080;
  public static final int RIGHTS_ADMIN = 0x0100;

  // Quota limits
  public static final String STORAGE = "STORAGE";

  // Authentication mechanisms
  public static final String KERBEROS_V4 = "KERBEROS_V4";
  public static final String GSSAPI = "GSSAPI";
  public static final String SKEY = "SKEY";
  public static final String CRAM_MD5 = "CRAM-MD5";
  public static final String LOGINDISABLED = "LOGINDISABLED";

}
