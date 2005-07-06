/*
 * IMAPConstants.java
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
 */
public interface IMAPConstants
{

  // Client commands
  public static final String CAPABILITY = "CAPABILITY";
  public static final String NOOP = "NOOP";
  public static final String AUTHENTICATE = "AUTHENTICATE";
  public static final String LOGIN = "LOGIN";
  public static final String LOGOUT = "LOGOUT";
  public static final String SELECT = "SELECT";
  public static final String EXAMINE = "EXAMINE";
  public static final String CREATE = "CREATE";
  public static final String DELETE = "DELETE";
  public static final String RENAME = "RENAME";
  public static final String SUBSCRIBE = "SUBSCRIBE";
  public static final String UNSUBSCRIBE = "UNSUBSCRIBE";
  public static final String LIST = "LIST";
  public static final String LSUB = "LSUB";
  public static final String STATUS = "STATUS";
  public static final String APPEND = "APPEND";
  public static final String CHECK = "CHECK";
  public static final String CLOSE = "CLOSE";
  public static final String EXPUNGE = "EXPUNGE";
  public static final String SEARCH = "SEARCH";
  public static final String FETCH = "FETCH";
  public static final String STORE = "STORE";
  public static final String COPY = "COPY";
  public static final String UID = "UID";
  public static final String STARTTLS = "STARTTLS";
  public static final String NAMESPACE = "NAMESPACE";
  public static final String SETACL = "SETACL";
  public static final String DELETEACL = "DELETEACL";
  public static final String GETACL = "GETACL";
  public static final String LISTRIGHTS = "LISTRIGHTS";
  public static final String MYRIGHTS = "MYRIGHTS";
  public static final String SETQUOTA = "SETQUOTA";
  public static final String GETQUOTA = "GETQUOTA";
  public static final String GETQUOTAROOT = "GETQUOTAROOT";

  // Server responses
  public static final String OK = "OK";
  public static final String NO = "NO";
  public static final String BAD = "BAD";
  public static final String PREAUTH = "PREAUTH";
  public static final String BYE = "BYE";

  // Response codes
  public static final String ALERT = "ALERT";
  public static final String NEWNAME = "NEWNAME";
  public static final String PARSE = "PARSE";
  public static final String PERMANENTFLAGS = "PERMANENTFLAGS";
  public static final String READ_ONLY = "READ-ONLY";
  public static final String READ_WRITE = "READ-WRITE";
  public static final String TRYCREATE = "TRYCREATE";
  public static final String UIDVALIDITY = "UIDVALIDITY";
  public static final String UNSEEN = "UNSEEN";
  public static final String ACL = "ACL";
  public static final String QUOTA = "QUOTA";
  public static final String QUOTAROOT = "QUOTAROOT";

  // Select responses
  public static final String FLAGS = "FLAGS";
  public static final String EXISTS = "EXISTS";
  public static final String RECENT = "RECENT";
  public static final String FETCH_FLAGS = "FETCH FLAGS";

  // Status items
  public static final String MESSAGES = "MESSAGES";
  public static final String UIDNEXT = "UIDNEXT";

  // List responses
  public static final String LIST_NOINFERIORS = "\\Noinferiors";
  public static final String LIST_NOSELECT = "\\Noselect";
  public static final String LIST_MARKED = "\\Marked";
  public static final String LIST_UNMARKED = "\\Unmarked";

  // Flags
  public static final String FLAG_SEEN = "\\Seen";
  public static final String FLAG_ANSWERED = "\\Answered";
  public static final String FLAG_FLAGGED = "\\Flagged";
  public static final String FLAG_DELETED = "\\Deleted";
  public static final String FLAG_DRAFT = "\\Draft";
  public static final String FLAG_RECENT = "\\Recent";

  // Fetch data items
  public static final String BODY = "BODY";
  public static final String BODY_PEEK = "BODY.PEEK";
  public static final String HEADER = "HEADER";
  public static final String HEADER_FIELDS = "HEADER.FIELDS";
  public static final String HEADER_FIELDS_NOT = "HEADER.FIELDS.NOT";
  public static final String BODYSTRUCTURE = "BODYSTRUCTURE";
  public static final String ENVELOPE = "ENVELOPE";
  public static final String INTERNALDATE = "INTERNALDATE";
  public static final String RFC822 = "RFC822";
  public static final String RFC822_HEADER = "RFC822.HEADER";
  public static final String RFC822_SIZE = "RFC822.SIZE";
  public static final String RFC822_TEXT = "RFC822.TEXT";

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
  public static final String NIL = "NIL";

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

